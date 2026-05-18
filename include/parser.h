#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  parser.h — Recursive-Descent Parser class declaration for CVM++
//
//  Phase 3: converts a flat token vector into an AST (Program node).
//  Usage:
//      Parser parser(tokens);
//      auto program = parser.parse();   // returns std::unique_ptr<Program>
// ─────────────────────────────────────────────────────────────────────────────
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "ast.h"
#include "token.h"

// ── ParseError — thrown (and caught internally) on syntax errors ─────────────
struct ParseError : std::runtime_error {
    int line;
    explicit ParseError(const std::string& msg, int line)
        : std::runtime_error(msg), line(line) {}
};

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);

    // Parse the entire token stream into a Program AST node.
    // Returns nullptr if an unrecoverable error occurred.
    std::unique_ptr<Program> parse();

    bool hadError() const { return m_hadError; }

private:
    std::vector<Token> m_tokens;
    int                m_current  = 0;
    bool               m_hadError = false;

    // ── Grammar rules (one method per rule) ─────────────────
    NodePtr parseStatement();
    NodePtr parseLetStmt();
    NodePtr parsePrintStmt();
    NodePtr parseInputStmt();
    NodePtr parseIfStmt();
    NodePtr parseWhileStmt();
    NodePtr parseExprStmt();
    NodeList parseBlock();         // Parses { stmt* } and returns the list

    NodePtr parseExpression();
    NodePtr parseAssignment();
    NodePtr parseEquality();
    NodePtr parseComparison();
    NodePtr parseTerm();
    NodePtr parseFactor();
    NodePtr parseUnary();
    NodePtr parsePrimary();

    // ── Token utilities ──────────────────────────────────────
    Token peek() const;
    Token previous() const;
    Token advance();
    bool  check(TokenType t) const;
    bool  match(TokenType t);
    bool  matchAny(std::initializer_list<TokenType> types);
    bool  isAtEnd() const;

    // Consume the current token if it matches, otherwise throw ParseError.
    Token consume(TokenType t, const std::string& errorMsg);

    // ── Error recovery ────────────────────────────────────────
    ParseError error(const std::string& msg);
    void       synchronize();   // Skip to next statement boundary
};
