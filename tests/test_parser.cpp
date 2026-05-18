// tests/test_parser.cpp
// Unit tests for the Parser (Phase 3).
#include <cassert>
#include <iostream>
#include "parser.h"
#include "lexer.h"
#include "ast.h"

// Helper function to tokenize and parse source code
std::unique_ptr<Program> parseString(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parse();
}

void test_basic_expression() {
    auto program = parseString("let x = 10 + 20 * 3;");
    assert(program != nullptr);
    assert(program->statements.size() == 1);
    
    auto letStmt = dynamic_cast<LetStmt*>(program->statements[0].get());
    assert(letStmt != nullptr);
    assert(letStmt->name == "x");
    
    auto binary = dynamic_cast<BinaryExpr*>(letStmt->initializer.get());
    assert(binary != nullptr);
    assert(binary->op == TokenType::PLUS);
    
    auto left = dynamic_cast<NumberLiteral*>(binary->left.get());
    assert(left != nullptr);
    assert(left->value == 10);
    
    auto right = dynamic_cast<BinaryExpr*>(binary->right.get());
    assert(right != nullptr);
    assert(right->op == TokenType::STAR);
}

void test_if_statement() {
    auto program = parseString("if (x > 5) { print x; } else { print 0; }");
    assert(program != nullptr);
    assert(program->statements.size() == 1);
    
    auto ifStmt = dynamic_cast<IfStmt*>(program->statements[0].get());
    assert(ifStmt != nullptr);
    
    auto cond = dynamic_cast<BinaryExpr*>(ifStmt->condition.get());
    assert(cond != nullptr);
    assert(cond->op == TokenType::GREATER);
    
    assert(ifStmt->thenBranch.size() == 1);
    assert(dynamic_cast<PrintStmt*>(ifStmt->thenBranch[0].get()) != nullptr);
    
    assert(ifStmt->elseBranch.size() == 1);
    assert(dynamic_cast<PrintStmt*>(ifStmt->elseBranch[0].get()) != nullptr);
}

void test_syntax_error() {
    Lexer lexer("let x = ;");
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parse();
    assert(parser.hadError() == true);
}

int main() {
    test_basic_expression();
    test_if_statement();
    test_syntax_error();
    std::cout << "[test_parser] Phase 3 tests passed!\n";
    return 0;
}
