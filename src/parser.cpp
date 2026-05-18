// parser.cpp — Phase 3 implementation
#include "parser.h"
#include <iostream>

Parser::Parser(std::vector<Token> tokens) 
    : m_tokens(std::move(tokens)), m_current(0), m_hadError(false) {}

std::unique_ptr<Program> Parser::parse() {
    NodeList statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(parseStatement());
        } catch (const ParseError& error) {
            synchronize();
        }
    }
    
    // If there were any errors during parsing, we could return nullptr
    // but returning the partial tree might be useful.
    // For now, if we had errors, we might still return what we have.
    return std::make_unique<Program>(std::move(statements));
}

NodePtr Parser::parseStatement() {
    if (match(TokenType::LET)) return parseLetStmt();
    if (match(TokenType::PRINT)) return parsePrintStmt();
    if (match(TokenType::INPUT)) return parseInputStmt();
    if (match(TokenType::IF)) return parseIfStmt();
    if (match(TokenType::WHILE)) return parseWhileStmt();
    if (match(TokenType::LBRACE)) return std::make_unique<Block>(parseBlock());
    
    return parseExprStmt();
}

NodePtr Parser::parseLetStmt() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    
    NodePtr initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = parseExpression();
    }
    
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<LetStmt>(name.lexeme, std::move(initializer));
}

NodePtr Parser::parsePrintStmt() {
    NodePtr value = parseExpression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<PrintStmt>(std::move(value));
}

NodePtr Parser::parseInputStmt() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name for input.");
    consume(TokenType::SEMICOLON, "Expect ';' after input statement.");
    return std::make_unique<InputStmt>(name.lexeme);
}

NodePtr Parser::parseIfStmt() {
    consume(TokenType::LPAREN, "Expect '(' after 'if'.");
    NodePtr condition = parseExpression();
    consume(TokenType::RPAREN, "Expect ')' after if condition.");
    
    NodeList thenBranch;
    if (match(TokenType::LBRACE)) {
        thenBranch = parseBlock();
    } else {
        thenBranch.push_back(parseStatement());
    }
    
    NodeList elseBranch;
    if (match(TokenType::ELSE)) {
        if (match(TokenType::LBRACE)) {
            elseBranch = parseBlock();
        } else {
            elseBranch.push_back(parseStatement());
        }
    }
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

NodePtr Parser::parseWhileStmt() {
    consume(TokenType::LPAREN, "Expect '(' after 'while'.");
    NodePtr condition = parseExpression();
    consume(TokenType::RPAREN, "Expect ')' after while condition.");
    
    NodeList body;
    if (match(TokenType::LBRACE)) {
        body = parseBlock();
    } else {
        body.push_back(parseStatement());
    }
    
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

NodePtr Parser::parseExprStmt() {
    NodePtr expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExprStmt>(std::move(expr));
}

NodeList Parser::parseBlock() {
    NodeList statements;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        try {
            statements.push_back(parseStatement());
        } catch (const ParseError& error) {
            synchronize();
        }
    }
    consume(TokenType::RBRACE, "Expect '}' after block.");
    return statements;
}

NodePtr Parser::parseExpression() {
    return parseAssignment();
}

NodePtr Parser::parseAssignment() {
    NodePtr expr = parseEquality();
    
    if (match(TokenType::EQUAL)) {
        Token equals = previous();
        NodePtr value = parseAssignment();
        
        if (auto* id = dynamic_cast<Identifier*>(expr.get())) {
            return std::make_unique<AssignExpr>(id->name, std::move(value), id->line);
        }
        
        error("Invalid assignment target.");
    }
    
    return expr;
}

NodePtr Parser::parseEquality() {
    NodePtr expr = parseComparison();
    
    while (matchAny({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        NodePtr right = parseComparison();
        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }
    
    return expr;
}

NodePtr Parser::parseComparison() {
    NodePtr expr = parseTerm();
    
    while (matchAny({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        NodePtr right = parseTerm();
        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }
    
    return expr;
}

NodePtr Parser::parseTerm() {
    NodePtr expr = parseFactor();
    
    while (matchAny({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        NodePtr right = parseFactor();
        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }
    
    return expr;
}

NodePtr Parser::parseFactor() {
    NodePtr expr = parseUnary();
    
    while (matchAny({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        NodePtr right = parseUnary();
        expr = std::make_unique<BinaryExpr>(op.type, std::move(expr), std::move(right));
    }
    
    return expr;
}

NodePtr Parser::parseUnary() {
    if (matchAny({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        NodePtr right = parseUnary();
        return std::make_unique<UnaryExpr>(op.type, std::move(right));
    }
    
    return parsePrimary();
}

NodePtr Parser::parsePrimary() {
    if (match(TokenType::FALSE_KW)) return std::make_unique<BoolLiteral>(false);
    if (match(TokenType::TRUE_KW)) return std::make_unique<BoolLiteral>(true);
    
    if (match(TokenType::NUMBER)) {
        return std::make_unique<NumberLiteral>(std::stoll(previous().lexeme));
    }
    
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<Identifier>(previous().lexeme, previous().line);
    }
    
    if (match(TokenType::LPAREN)) {
        NodePtr expr = parseExpression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return expr;
    }
    
    throw error("Expect expression.");
}

Token Parser::peek() const {
    return m_tokens[m_current];
}

Token Parser::previous() const {
    return m_tokens[m_current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) m_current++;
    return previous();
}

bool Parser::check(TokenType t) const {
    if (isAtEnd()) return false;
    return peek().type == t;
}

bool Parser::match(TokenType t) {
    if (check(t)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchAny(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::consume(TokenType t, const std::string& errorMsg) {
    if (check(t)) return advance();
    throw error(errorMsg);
}

ParseError Parser::error(const std::string& msg) {
    std::cerr << "[line " << peek().line << "] Error at ";
    if (peek().type == TokenType::EOF_TOKEN) {
        std::cerr << "end";
    } else {
        std::cerr << "'" << peek().lexeme << "'";
    }
    std::cerr << ": " << msg << std::endl;
    
    m_hadError = true;
    return ParseError(msg, peek().line);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::INPUT:
                return;
            default:
                // Do nothing.
                break;
        }
        
        advance();
    }
}
