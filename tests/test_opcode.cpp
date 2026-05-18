#include <iostream>
#include <cassert>
#include "opcode.h"

void testEmitAndConstants() {
    Chunk chunk;
    int constantIdx = chunk.addConstant(42);
    assert(constantIdx == 0);
    
    chunk.emit(OpCode::OP_CONSTANT, static_cast<uint8_t>(constantIdx));
    chunk.emit(OpCode::OP_ADD);
    chunk.emit(OpCode::OP_HALT);

    assert(chunk.code.size() == 4);
    assert(static_cast<OpCode>(chunk.code[0]) == OpCode::OP_CONSTANT);
    assert(chunk.code[1] == 0);
    assert(static_cast<OpCode>(chunk.code[2]) == OpCode::OP_ADD);
    assert(static_cast<OpCode>(chunk.code[3]) == OpCode::OP_HALT);

    std::cout << "testEmitAndConstants passed!\n";
}

void testEmitJump() {
    Chunk chunk;
    chunk.emit(OpCode::OP_JUMP, static_cast<uint16_t>(258)); // 258 is 0x0102
    
    assert(chunk.code.size() == 3);
    assert(static_cast<OpCode>(chunk.code[0]) == OpCode::OP_JUMP);
    assert(chunk.code[1] == 0x01);
    assert(chunk.code[2] == 0x02);

    std::cout << "testEmitJump passed!\n";
}

void testAddName() {
    Chunk chunk;
    int idx1 = chunk.addName("x");
    int idx2 = chunk.addName("y");
    int idx3 = chunk.addName("x"); // Deduplication test

    assert(idx1 == 0);
    assert(idx2 == 1);
    assert(idx3 == 0);
    assert(chunk.names.size() == 2);

    std::cout << "testAddName passed!\n";
}

int main() {
    testEmitAndConstants();
    testEmitJump();
    testAddName();
    
    std::cout << "All OpCode / Chunk tests passed!\n";
    return 0;
}
