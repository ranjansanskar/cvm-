#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  opcode.h — Instruction Set Architecture (ISA) for CVM++
//
//  Each opcode fits in a uint8_t.  Operands (if any) follow in the byte stream.
//  Designed to be stack-based: most instructions pop operands and push results.
//  Implemented / emitted in Phase 5 (Compiler), dispatched in Phase 6 (VM).
// ─────────────────────────────────────────────────────────────────────────────
#include <cstdint>
#include <string>
#include <vector>
#include "value.h"

// ── Opcode enumeration ───────────────────────────────────────────────────────
enum class OpCode : uint8_t {
    // ── Stack management ─────────────────────────────────────
    OP_CONSTANT,        // Push constants[operand8] onto the stack
    OP_POP,             // Discard the top-of-stack value

    // ── Arithmetic ────────────────────────────────────────────
    OP_ADD,             // pop b, pop a → push a + b
    OP_SUBTRACT,        // pop b, pop a → push a - b
    OP_MULTIPLY,        // pop b, pop a → push a * b
    OP_DIVIDE,          // pop b, pop a → push a / b  (runtime error if b==0)
    OP_NEGATE,          // pop a        → push -a

    // ── Comparison (result is 1=true or 0=false) ─────────────
    OP_EQUAL,           // pop b, pop a → push (a == b)
    OP_NOT_EQUAL,       // pop b, pop a → push (a != b)
    OP_LESS,            // pop b, pop a → push (a < b)
    OP_GREATER,         // pop b, pop a → push (a > b)
    OP_LESS_EQUAL,      // pop b, pop a → push (a <= b)
    OP_GREATER_EQUAL,   // pop b, pop a → push (a >= b)

    // ── Logical ───────────────────────────────────────────────
    OP_NOT,             // pop a        → push (!a)  i.e. 0→1, non-zero→0

    // ── Boolean literals ──────────────────────────────────────
    OP_TRUE,            // Push 1 (true)
    OP_FALSE,           // Push 0 (false)

    // ── Global variables ──────────────────────────────────────
    OP_SET_GLOBAL,      // pop value; globals[names[operand8]] = value
    OP_GET_GLOBAL,      // push globals[names[operand8]]  (runtime error if undefined)

    // ── Control flow ──────────────────────────────────────────
    OP_JUMP,            // ip += operand16  (unconditional forward jump)
    OP_JUMP_IF_FALSE,   // pop cond; if (!cond) ip += operand16
    OP_LOOP,            // ip -= operand16  (unconditional backward jump for while)

    // ── I/O ───────────────────────────────────────────────────
    OP_PRINT,           // pop and print the top of stack
    OP_INPUT,           // read int64 from stdin, push onto stack

    // ── Program control ───────────────────────────────────────
    OP_HALT             // Stop execution
};

// ── Chunk — the bytecode container ──────────────────────────────────────────
//  A Chunk holds the raw byte stream, the constant pool, and the variable name pool.
struct Chunk {
    std::vector<uint8_t>     code;       // Flat byte stream: opcodes + operands
    std::vector<Value>       constants;  // Constant pool (numbers)
    std::vector<std::string> names;      // Variable name pool

    // ── Emitters ─────────────────────────────────────────────
    void emit(OpCode op);
    void emit(OpCode op, uint8_t  operand);
    void emit(OpCode op, uint16_t operand);  // Used for jump offsets

    // ── Pool helpers ──────────────────────────────────────────
    int addConstant(Value value);                   // Returns pool index
    int addName(const std::string& name);           // Returns pool index (deduplicating)

    // ── Disassembler ─────────────────────────────────────────
    void disassemble(const std::string& title = "") const;
};
