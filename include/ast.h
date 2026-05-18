#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  ast.h — Abstract Syntax Tree (AST) node definitions for CVM++
//
//  The Parser (Phase 3) constructs a tree of these nodes.
//  The Compiler (Phase 5) walks the tree to emit bytecode.
//
//  Design choices:
//    • Base class ASTNode with a virtual destructor.
//    • All child pointers are std::unique_ptr to express sole ownership.
//    • Statements and expressions share the same base class for simplicity.
// ─────────────────────────────────────────────────────────────────────────────
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "token.h"   // for TokenType (used in BinaryExpr / UnaryExpr operators)

// ── Forward declaration for the visitor / dispatch pattern ───────────────────
struct ASTNode {
    virtual ~ASTNode() = default;
};

using NodePtr = std::unique_ptr<ASTNode>;
using NodeList = std::vector<NodePtr>;

// ═════════════════════════════════════════════════════════════════════════════
//  Expression nodes
// ═════════════════════════════════════════════════════════════════════════════

// Numeric literal — e.g.  42
struct NumberLiteral : ASTNode {
    int64_t value;
    explicit NumberLiteral(int64_t v) : value(v) {}
};

// Boolean literal — true / false
struct BoolLiteral : ASTNode {
    bool value;
    explicit BoolLiteral(bool v) : value(v) {}
};

// Variable reference — e.g.  x
struct Identifier : ASTNode {
    std::string name;
    int         line;   // for runtime error messages
    Identifier(std::string n, int l) : name(std::move(n)), line(l) {}
};

// Unary expression — e.g.  -x  !flag
struct UnaryExpr : ASTNode {
    TokenType op;
    NodePtr   operand;
    UnaryExpr(TokenType op, NodePtr operand)
        : op(op), operand(std::move(operand)) {}
};

// Binary expression — e.g.  a + b  x == y
struct BinaryExpr : ASTNode {
    TokenType op;
    NodePtr   left;
    NodePtr   right;
    BinaryExpr(TokenType op, NodePtr left, NodePtr right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
};

// Assignment expression — e.g.  x = 10 + 5
struct AssignExpr : ASTNode {
    std::string name;
    NodePtr     value;
    int         line;
    AssignExpr(std::string name, NodePtr value, int line)
        : name(std::move(name)), value(std::move(value)), line(line) {}
};

// ═════════════════════════════════════════════════════════════════════════════
//  Statement nodes
// ═════════════════════════════════════════════════════════════════════════════

// let x = <expr>;
struct LetStmt : ASTNode {
    std::string name;
    NodePtr     initializer;
    LetStmt(std::string name, NodePtr init)
        : name(std::move(name)), initializer(std::move(init)) {}
};

// print <expr>;
struct PrintStmt : ASTNode {
    NodePtr expression;
    explicit PrintStmt(NodePtr expr) : expression(std::move(expr)) {}
};

// input <identifier>;
struct InputStmt : ASTNode {
    std::string variableName;
    explicit InputStmt(std::string varName) : variableName(std::move(varName)) {}
};

// if (<condition>) <thenBranch> [else <elseBranch>]
struct IfStmt : ASTNode {
    NodePtr  condition;
    NodeList thenBranch;
    NodeList elseBranch;  // empty if no else clause
    IfStmt(NodePtr cond, NodeList then_, NodeList else_)
        : condition(std::move(cond)),
          thenBranch(std::move(then_)),
          elseBranch(std::move(else_)) {}
};

// while (<condition>) <body>
struct WhileStmt : ASTNode {
    NodePtr  condition;
    NodeList body;
    WhileStmt(NodePtr cond, NodeList body_)
        : condition(std::move(cond)), body(std::move(body_)) {}
};

// { <statement>* }
struct Block : ASTNode {
    NodeList statements;
    explicit Block(NodeList stmts) : statements(std::move(stmts)) {}
};

// A standalone expression used as a statement — e.g.  x = x + 1;
struct ExprStmt : ASTNode {
    NodePtr expression;
    explicit ExprStmt(NodePtr expr) : expression(std::move(expr)) {}
};

// The root of the whole program
struct Program : ASTNode {
    NodeList statements;
    explicit Program(NodeList stmts) : statements(std::move(stmts)) {}
};
