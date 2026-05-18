#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  token.h — Token types and Token struct for CVM++
//
//  Every piece of source text scanned by the Lexer becomes one of these tokens.
//  Implemented in Phase 2 (Lexer).
// ─────────────────────────────────────────────────────────────────────────────
#include <string>

// ── Token type enumeration ───────────────────────────────────────────────────
enum class TokenType {
    // ── Literals ─────────────────────────────────────────
    NUMBER,         // e.g.  42  100  0
    TRUE_KW,        // true
    FALSE_KW,       // false
    IDENTIFIER,     // e.g.  x  sum  myVar

    // ── Arithmetic operators ──────────────────────────────
    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /

    // ── Assignment & equality ─────────────────────────────
    EQUAL,          // =
    EQUAL_EQUAL,    // ==
    BANG,           // !
    BANG_EQUAL,     // !=

    // ── Relational operators ──────────────────────────────
    LESS,           // <
    LESS_EQUAL,     // <=
    GREATER,        // >
    GREATER_EQUAL,  // >=

    // ── Delimiters ────────────────────────────────────────
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    SEMICOLON,      // ;

    // ── Keywords ──────────────────────────────────────────
    LET,            // let
    PRINT,          // print
    INPUT,          // input
    IF,             // if
    ELSE,           // else
    WHILE,          // while

    // ── Special ───────────────────────────────────────────
    EOF_TOKEN,      // End of file / source string
    ERROR           // Invalid / unrecognised character
};

// ── Token struct ─────────────────────────────────────────────────────────────
struct Token {
    TokenType   type;
    std::string lexeme;  // Raw text of the token as it appears in source
    int         line;    // 1-based line number (for error messages)
};

// ── Utility: human-readable name for a TokenType (handy for debug output) ───
const char* tokenTypeName(TokenType t);
