#include "lexer.h"
#include <iostream>
#include <cctype>

const std::unordered_map<std::string, TokenType> Lexer::s_keywords = {
    {"let",    TokenType::LET},
    {"print",  TokenType::PRINT},
    {"input",  TokenType::INPUT},
    {"if",     TokenType::IF},
    {"else",   TokenType::ELSE},
    {"while",  TokenType::WHILE},
    {"true",   TokenType::TRUE_KW},
    {"false",  TokenType::FALSE_KW}
};

const char* tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::NUMBER:        return "NUMBER";
        case TokenType::TRUE_KW:       return "TRUE_KW";
        case TokenType::FALSE_KW:      return "FALSE_KW";
        case TokenType::IDENTIFIER:    return "IDENTIFIER";
        case TokenType::PLUS:          return "PLUS";
        case TokenType::MINUS:         return "MINUS";
        case TokenType::STAR:          return "STAR";
        case TokenType::SLASH:         return "SLASH";
        case TokenType::EQUAL:         return "EQUAL";
        case TokenType::EQUAL_EQUAL:   return "EQUAL_EQUAL";
        case TokenType::BANG:          return "BANG";
        case TokenType::BANG_EQUAL:    return "BANG_EQUAL";
        case TokenType::LESS:          return "LESS";
        case TokenType::LESS_EQUAL:    return "LESS_EQUAL";
        case TokenType::GREATER:       return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LPAREN:        return "LPAREN";
        case TokenType::RPAREN:        return "RPAREN";
        case TokenType::LBRACE:        return "LBRACE";
        case TokenType::RBRACE:        return "RBRACE";
        case TokenType::SEMICOLON:     return "SEMICOLON";
        case TokenType::LET:           return "LET";
        case TokenType::PRINT:         return "PRINT";
        case TokenType::INPUT:         return "INPUT";
        case TokenType::IF:            return "IF";
        case TokenType::ELSE:          return "ELSE";
        case TokenType::WHILE:         return "WHILE";
        case TokenType::EOF_TOKEN:     return "EOF_TOKEN";
        case TokenType::ERROR:         return "ERROR";
        default:                       return "UNKNOWN";
    }
}

Lexer::Lexer(std::string source) : m_source(std::move(source)) {}

std::vector<Token> Lexer::tokenize() {
    while (!isAtEnd()) {
        m_start = m_current;
        scanToken();
    }

    m_tokens.push_back(Token{TokenType::EOF_TOKEN, "", m_line});
    return m_tokens;
}

bool Lexer::isAtEnd() const {
    return m_current >= m_source.length();
}

char Lexer::advance() {
    return m_source[m_current++];
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return m_source[m_current];
}

char Lexer::peekNext() const {
    if (m_current + 1 >= m_source.length()) return '\0';
    return m_source[m_current + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (m_source[m_current] != expected) return false;
    m_current++;
    return true;
}

void Lexer::addToken(TokenType type) {
    std::string text = m_source.substr(m_start, m_current - m_start);
    m_tokens.push_back(Token{type, text, m_line});
}

void Lexer::addError(const std::string& message) {
    m_hadError = true;
    std::cerr << "[line " << m_line << "] Error: " << message << "\n";
    m_tokens.push_back(Token{TokenType::ERROR, message, m_line});
}

void Lexer::scanToken() {
    char c = advance();
    switch (c) {
        case '(': addToken(TokenType::LPAREN); break;
        case ')': addToken(TokenType::RPAREN); break;
        case '{': addToken(TokenType::LBRACE); break;
        case '}': addToken(TokenType::RBRACE); break;
        case ';': addToken(TokenType::SEMICOLON); break;
        case '+': addToken(TokenType::PLUS); break;
        case '-': addToken(TokenType::MINUS); break;
        case '*': addToken(TokenType::STAR); break;
        
        case '!':
            addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
            break;
            
        case '/':
            if (match('/')) {
                skipLineComment();
            } else {
                addToken(TokenType::SLASH);
            }
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;

        case '\n':
            m_line++;
            break;

        default:
            if (std::isdigit(c)) {
                scanNumber();
            } else if (std::isalpha(c) || c == '_') {
                scanIdentifierOrKeyword();
            } else {
                addError("Unexpected character.");
            }
            break;
    }
}

void Lexer::skipLineComment() {
    // A comment goes until the end of the line.
    while (peek() != '\n' && !isAtEnd()) {
        advance();
    }
}

void Lexer::scanNumber() {
    while (std::isdigit(peek())) {
        advance();
    }
    addToken(TokenType::NUMBER);
}

void Lexer::scanIdentifierOrKeyword() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = m_source.substr(m_start, m_current - m_start);
    TokenType type = TokenType::IDENTIFIER;
    
    auto it = s_keywords.find(text);
    if (it != s_keywords.end()) {
        type = it->second;
    }
    
    addToken(type);
}
