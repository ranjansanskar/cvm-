#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  compiler.h — Bytecode Compiler class declaration for CVM++
//
//  Phase 5: walks the AST and emits a Chunk of bytecode.
//  Usage:
//      Compiler compiler;
//      Chunk chunk = compiler.compile(*program);
// ─────────────────────────────────────────────────────────────────────────────
#include <stdexcept>
#include <string>
#include "ast.h"
#include "opcode.h"

// ── CompileError — thrown on semantic errors detected during compilation ─────
struct CompileError : std::runtime_error {
    explicit CompileError(const std::string& msg) : std::runtime_error(msg) {}
};

class Compiler {
public:
    Compiler() = default;

    // Compile a full program AST into a Chunk ready for the VM.
    Chunk compile(const Program& program);

    bool hadError() const { return m_hadError; }

private:
    Chunk m_chunk;
    bool  m_hadError = false;

    // ── Dispatch ─────────────────────────────────────────────
    void compileNode(const ASTNode& node);

    // ── Statements ───────────────────────────────────────────
    void compileLetStmt(const LetStmt& node);
    void compilePrintStmt(const PrintStmt& node);
    void compileInputStmt(const InputStmt& node);
    void compileIfStmt(const IfStmt& node);
    void compileWhileStmt(const WhileStmt& node);
    void compileExprStmt(const ExprStmt& node);
    void compileBlock(const NodeList& stmts);

    // ── Expressions ──────────────────────────────────────────
    void compileNumberLiteral(const NumberLiteral& node);
    void compileBoolLiteral(const BoolLiteral& node);
    void compileIdentifier(const Identifier& node);
    void compileUnaryExpr(const UnaryExpr& node);
    void compileBinaryExpr(const BinaryExpr& node);
    void compileAssignExpr(const AssignExpr& node);

    // ── Jump / backpatch helpers ──────────────────────────────
    // Emit a jump instruction with a placeholder offset; returns patch position.
    int  emitJump(OpCode jumpOp);
    // Fill in the real offset at the given patch position.
    void patchJump(int patchPos);

    // Emit a backward loop jump back to loopStart.
    void emitLoop(int loopStart);

    // ── Raw emitters ─────────────────────────────────────────
    void emit(OpCode op);
    void emit(OpCode op, uint8_t operand);
};
