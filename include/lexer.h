#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  lexer.h — Lexer (Tokenizer) class declaration for CVM++
//
//  Phase 2: converts a raw source string into a flat vector of Tokens.
//  Usage:
//      Lexer lexer(source);
//      auto tokens = lexer.tokenize();
// ─────────────────────────────────────────────────────────────────────────────
#include <string>
#include <unordered_map>
#include <vector>
#include "token.h"

class Lexer {
public:
    explicit Lexer(std::string source);

    // Scan all tokens and return the complete token list (including EOF_TOKEN).
    std::vector<Token> tokenize();

    // True if any error tokens were emitted during scanning.
    bool hadError() const { return m_hadError; }

private:
    // ── Source tracking ──────────────────────────────────────
    std::string m_source;
    int         m_start   = 0;   // Start of the current lexeme
    int         m_current = 0;   // Current read position
    int         m_line    = 1;   // 1-based line counter

    bool        m_hadError = false;

    std::vector<Token> m_tokens;

    // ── Scanning helpers ─────────────────────────────────────
    bool isAtEnd() const;
    char advance();                // Consume and return current char
    char peek() const;             // Look at current char without consuming
    char peekNext() const;         // Look one char ahead
    bool match(char expected);     // Consume if next char matches

    // ── Token emission ────────────────────────────────────────
    void addToken(TokenType type);
    void addError(const std::string& message);

    // ── Scanning sub-routines ─────────────────────────────────
    void scanToken();
    void scanNumber();
    void scanIdentifierOrKeyword();
    void skipLineComment();        // // …  style comments

    // ── Keyword table ─────────────────────────────────────────
    static const std::unordered_map<std::string, TokenType> s_keywords;
};
