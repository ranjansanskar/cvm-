// tests/test_compiler.cpp
// Unit tests for the Compiler (Phase 5).
#include <iostream>
#include <cassert>
#include <memory>
#include "compiler.h"
#include "ast.h"

// Helper to create simple program
Program make_program(NodeList stmts) {
    return Program(std::move(stmts));
}

void test_compile_number() {
    Compiler compiler;
    NodeList stmts;
    stmts.push_back(std::make_unique<ExprStmt>(std::make_unique<NumberLiteral>(42)));
    Program program = make_program(std::move(stmts));

    Chunk chunk = compiler.compile(program);
    assert(!compiler.hadError());
    assert(chunk.code.size() > 0);
    assert(chunk.constants.size() == 1);
    assert(chunk.constants[0] == 42); 
    // code: OP_CONSTANT <idx>, OP_POP, OP_HALT
    assert(chunk.code[0] == static_cast<uint8_t>(OpCode::OP_CONSTANT));
    assert(chunk.code[1] == 0); // index
    assert(chunk.code[2] == static_cast<uint8_t>(OpCode::OP_POP));
    assert(chunk.code[3] == static_cast<uint8_t>(OpCode::OP_HALT));
    std::cout << "[test_compiler] test_compile_number PASS\n";
}

void test_compile_binary() {
    Compiler compiler;
    NodeList stmts;
    stmts.push_back(std::make_unique<ExprStmt>(
        std::make_unique<BinaryExpr>(
            TokenType::PLUS,
            std::make_unique<NumberLiteral>(10),
            std::make_unique<NumberLiteral>(20)
        )
    ));
    Program program = make_program(std::move(stmts));

    Chunk chunk = compiler.compile(program);
    assert(!compiler.hadError());
    assert(chunk.constants.size() == 2);
    // code: OP_CONSTANT 0, OP_CONSTANT 1, OP_ADD, OP_POP, OP_HALT
    assert(chunk.code[0] == static_cast<uint8_t>(OpCode::OP_CONSTANT));
    assert(chunk.code[2] == static_cast<uint8_t>(OpCode::OP_CONSTANT));
    assert(chunk.code[4] == static_cast<uint8_t>(OpCode::OP_ADD));
    assert(chunk.code[5] == static_cast<uint8_t>(OpCode::OP_POP));
    assert(chunk.code[6] == static_cast<uint8_t>(OpCode::OP_HALT));
    std::cout << "[test_compiler] test_compile_binary PASS\n";
}

void test_compile_let() {
    Compiler compiler;
    NodeList stmts;
    stmts.push_back(std::make_unique<LetStmt>("x", std::make_unique<NumberLiteral>(100)));
    Program program = make_program(std::move(stmts));

    Chunk chunk = compiler.compile(program);
    assert(!compiler.hadError());
    assert(chunk.names.size() == 1);
    assert(chunk.names[0] == "x");
    // OP_CONSTANT 0, OP_SET_GLOBAL 0, OP_HALT
    assert(chunk.code[0] == static_cast<uint8_t>(OpCode::OP_CONSTANT));
    assert(chunk.code[2] == static_cast<uint8_t>(OpCode::OP_SET_GLOBAL));
    assert(chunk.code[3] == 0); // index of x
    std::cout << "[test_compiler] test_compile_let PASS\n";
}

int main() {
    test_compile_number();
    test_compile_binary();
    test_compile_let();
    std::cout << "[test_compiler] Phase 5 tests PASS\n";
    return 0;
}
