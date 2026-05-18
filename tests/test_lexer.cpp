#include "lexer.h"
#include <cassert>
#include <iostream>

void testBasicTokens() {
    Lexer lexer("let x = 10;");
    auto tokens = lexer.tokenize();
    
    assert(!lexer.hadError());
    assert(tokens.size() == 6);
    
    assert(tokens[0].type == TokenType::LET);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[1].lexeme == "x");
    assert(tokens[2].type == TokenType::EQUAL);
    assert(tokens[3].type == TokenType::NUMBER);
    assert(tokens[3].lexeme == "10");
    assert(tokens[4].type == TokenType::SEMICOLON);
    assert(tokens[5].type == TokenType::EOF_TOKEN);
}

void testKeywordsAndOperators() {
    Lexer lexer("if (a != 5) { print true; }");
    auto tokens = lexer.tokenize();
    
    assert(!lexer.hadError());
    
    assert(tokens[0].type == TokenType::IF);
    assert(tokens[1].type == TokenType::LPAREN);
    assert(tokens[2].type == TokenType::IDENTIFIER);
    assert(tokens[3].type == TokenType::BANG_EQUAL);
    assert(tokens[4].type == TokenType::NUMBER);
    assert(tokens[5].type == TokenType::RPAREN);
    assert(tokens[6].type == TokenType::LBRACE);
    assert(tokens[7].type == TokenType::PRINT);
    assert(tokens[8].type == TokenType::TRUE_KW);
    assert(tokens[9].type == TokenType::SEMICOLON);
    assert(tokens[10].type == TokenType::RBRACE);
    assert(tokens[11].type == TokenType::EOF_TOKEN);
}

void testCommentsAndWhitespace() {
    Lexer lexer("let y = 20; // This is a comment\n let z = 30;");
    auto tokens = lexer.tokenize();
    
    assert(!lexer.hadError());
    
    assert(tokens[0].type == TokenType::LET);
    assert(tokens[1].type == TokenType::IDENTIFIER);
    assert(tokens[2].type == TokenType::EQUAL);
    assert(tokens[3].type == TokenType::NUMBER);
    assert(tokens[4].type == TokenType::SEMICOLON);
    
    assert(tokens[5].type == TokenType::LET);
    assert(tokens[5].line == 2); // The new line should increment the line counter
    assert(tokens[6].type == TokenType::IDENTIFIER);
    assert(tokens[7].type == TokenType::EQUAL);
    assert(tokens[8].type == TokenType::NUMBER);
    assert(tokens[9].type == TokenType::SEMICOLON);
    
    assert(tokens[10].type == TokenType::EOF_TOKEN);
}

int main() {
    std::cout << "[test_lexer] Running Lexer tests...\n";
    
    testBasicTokens();
    testKeywordsAndOperators();
    testCommentsAndWhitespace();
    
    std::cout << "[test_lexer] All Phase 2 tests PASS.\n";
    return 0;
}
