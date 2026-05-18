#include "compiler.h"
#include <iostream>

Chunk Compiler::compile(const Program& program) {
    m_chunk = Chunk();
    m_hadError = false;

    for (const auto& stmt : program.statements) {
        compileNode(*stmt);
    }
    
    emit(OpCode::OP_HALT);
    return m_chunk;
}

void Compiler::compileNode(const ASTNode& node) {
    if (auto n = dynamic_cast<const NumberLiteral*>(&node)) {
        compileNumberLiteral(*n);
    } else if (auto n = dynamic_cast<const BoolLiteral*>(&node)) {
        compileBoolLiteral(*n);
    } else if (auto n = dynamic_cast<const Identifier*>(&node)) {
        compileIdentifier(*n);
    } else if (auto n = dynamic_cast<const UnaryExpr*>(&node)) {
        compileUnaryExpr(*n);
    } else if (auto n = dynamic_cast<const BinaryExpr*>(&node)) {
        compileBinaryExpr(*n);
    } else if (auto n = dynamic_cast<const AssignExpr*>(&node)) {
        compileAssignExpr(*n);
    } else if (auto n = dynamic_cast<const LetStmt*>(&node)) {
        compileLetStmt(*n);
    } else if (auto n = dynamic_cast<const PrintStmt*>(&node)) {
        compilePrintStmt(*n);
    } else if (auto n = dynamic_cast<const InputStmt*>(&node)) {
        compileInputStmt(*n);
    } else if (auto n = dynamic_cast<const IfStmt*>(&node)) {
        compileIfStmt(*n);
    } else if (auto n = dynamic_cast<const WhileStmt*>(&node)) {
        compileWhileStmt(*n);
    } else if (auto n = dynamic_cast<const ExprStmt*>(&node)) {
        compileExprStmt(*n);
    } else if (auto n = dynamic_cast<const Block*>(&node)) {
        compileBlock(n->statements);
    } else {
        m_hadError = true;
        std::cerr << "Compiler Error: Unknown AST node type." << std::endl;
    }
}

// ── Statements ───────────────────────────────────────────

void Compiler::compileLetStmt(const LetStmt& node) {
    if (node.initializer) {
        compileNode(*node.initializer);
    } else {
        int constIdx = m_chunk.addConstant(0);
        emit(OpCode::OP_CONSTANT, static_cast<uint8_t>(constIdx));
    }
    int nameIdx = m_chunk.addName(node.name);
    emit(OpCode::OP_SET_GLOBAL, static_cast<uint8_t>(nameIdx));
}

void Compiler::compilePrintStmt(const PrintStmt& node) {
    compileNode(*node.expression);
    emit(OpCode::OP_PRINT);
}

void Compiler::compileInputStmt(const InputStmt& node) {
    emit(OpCode::OP_INPUT);
    int nameIdx = m_chunk.addName(node.variableName);
    emit(OpCode::OP_SET_GLOBAL, static_cast<uint8_t>(nameIdx));
}

void Compiler::compileIfStmt(const IfStmt& node) {
    compileNode(*node.condition);
    int thenJump = emitJump(OpCode::OP_JUMP_IF_FALSE);
    
    // Evaluate then branch
    for (const auto& stmt : node.thenBranch) {
        compileNode(*stmt);
    }

    if (!node.elseBranch.empty()) {
        int elseJump = emitJump(OpCode::OP_JUMP);
        patchJump(thenJump);
        for (const auto& stmt : node.elseBranch) {
            compileNode(*stmt);
        }
        patchJump(elseJump);
    } else {
        patchJump(thenJump);
    }
}

void Compiler::compileWhileStmt(const WhileStmt& node) {
    int loopStart = m_chunk.code.size();
    
    compileNode(*node.condition);
    int exitJump = emitJump(OpCode::OP_JUMP_IF_FALSE);
    
    for (const auto& stmt : node.body) {
        compileNode(*stmt);
    }
    
    emitLoop(loopStart);
    patchJump(exitJump);
}

void Compiler::compileExprStmt(const ExprStmt& node) {
    compileNode(*node.expression);
    emit(OpCode::OP_POP);
}

void Compiler::compileBlock(const NodeList& stmts) {
    for (const auto& stmt : stmts) {
        compileNode(*stmt);
    }
}

// ── Expressions ──────────────────────────────────────────

void Compiler::compileNumberLiteral(const NumberLiteral& node) {
    int constIdx = m_chunk.addConstant(node.value);
    emit(OpCode::OP_CONSTANT, static_cast<uint8_t>(constIdx));
}

void Compiler::compileBoolLiteral(const BoolLiteral& node) {
    emit(node.value ? OpCode::OP_TRUE : OpCode::OP_FALSE);
}

void Compiler::compileIdentifier(const Identifier& node) {
    int nameIdx = m_chunk.addName(node.name);
    emit(OpCode::OP_GET_GLOBAL, static_cast<uint8_t>(nameIdx));
}

void Compiler::compileUnaryExpr(const UnaryExpr& node) {
    compileNode(*node.operand);
    switch (node.op) {
        case TokenType::MINUS: emit(OpCode::OP_NEGATE); break;
        case TokenType::BANG:  emit(OpCode::OP_NOT); break;
        default: break;
    }
}

void Compiler::compileBinaryExpr(const BinaryExpr& node) {
    compileNode(*node.left);
    compileNode(*node.right);
    
    switch (node.op) {
        case TokenType::PLUS:          emit(OpCode::OP_ADD); break;
        case TokenType::MINUS:         emit(OpCode::OP_SUBTRACT); break;
        case TokenType::STAR:          emit(OpCode::OP_MULTIPLY); break;
        case TokenType::SLASH:         emit(OpCode::OP_DIVIDE); break;
        case TokenType::EQUAL_EQUAL:   emit(OpCode::OP_EQUAL); break;
        case TokenType::BANG_EQUAL:    emit(OpCode::OP_NOT_EQUAL); break;
        case TokenType::LESS:          emit(OpCode::OP_LESS); break;
        case TokenType::GREATER:       emit(OpCode::OP_GREATER); break;
        case TokenType::LESS_EQUAL:    emit(OpCode::OP_LESS_EQUAL); break;
        case TokenType::GREATER_EQUAL: emit(OpCode::OP_GREATER_EQUAL); break;
        default: break;
    }
}

void Compiler::compileAssignExpr(const AssignExpr& node) {
    compileNode(*node.value);
    int nameIdx = m_chunk.addName(node.name);
    // Since assign expression evaluates to the assigned value, we set global,
    // and then immediately get it so it stays on the stack for any surrounding expression.
    emit(OpCode::OP_SET_GLOBAL, static_cast<uint8_t>(nameIdx));
    emit(OpCode::OP_GET_GLOBAL, static_cast<uint8_t>(nameIdx));
}

// ── Jump / backpatch helpers ──────────────────────────────

int Compiler::emitJump(OpCode jumpOp) {
    emit(jumpOp);
    m_chunk.code.push_back(0xFF);
    m_chunk.code.push_back(0xFF);
    return m_chunk.code.size() - 2;
}

void Compiler::patchJump(int patchPos) {
    int jump = m_chunk.code.size() - patchPos - 2;
    if (jump > UINT16_MAX) {
        m_hadError = true;
        std::cerr << "Compiler Error: Jump offset too large." << std::endl;
        return;
    }
    m_chunk.code[patchPos] = (jump >> 8) & 0xFF;
    m_chunk.code[patchPos + 1] = jump & 0xFF;
}

void Compiler::emitLoop(int loopStart) {
    emit(OpCode::OP_LOOP);
    int jump = m_chunk.code.size() - loopStart + 2;
    if (jump > UINT16_MAX) {
        m_hadError = true;
        std::cerr << "Compiler Error: Loop body too large." << std::endl;
        return;
    }
    m_chunk.code.push_back((jump >> 8) & 0xFF);
    m_chunk.code.push_back(jump & 0xFF);
}

// ── Raw emitters ─────────────────────────────────────────

void Compiler::emit(OpCode op) {
    m_chunk.emit(op);
}

void Compiler::emit(OpCode op, uint8_t operand) {
    m_chunk.emit(op, operand);
}
