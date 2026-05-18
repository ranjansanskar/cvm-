#pragma once
// ─────────────────────────────────────────────────────────────────────────────
//  vm.h — Stack-Based Virtual Machine class declaration for CVM++
//
//  Phase 6: executes a Chunk of bytecode.
//  Usage:
//      VM vm;
//      vm.execute(chunk);
// ─────────────────────────────────────────────────────────────────────────────
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "opcode.h"
#include "value.h"

// ── RuntimeError — thrown when execution cannot continue ─────────────────────
struct RuntimeError : std::runtime_error {
    explicit RuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class VM {
public:
    VM() = default;

    // Execute a compiled Chunk.  Throws RuntimeError on runtime faults.
    void execute(const Chunk& chunk);

    // Reset VM state (stack + globals) between executions in REPL mode.
    void reset();

private:
    // ── Execution state ──────────────────────────────────────
    const Chunk*                             m_chunk   = nullptr;
    int                                      m_ip      = 0;
    std::vector<Value>                       m_stack;
    std::unordered_map<std::string, Value>   m_globals;

    // ── Byte-stream reading ────────────────────────────────────
    uint8_t  readByte();
    uint16_t readShort();  // Reads two bytes big-endian (for jump offsets)

    // ── Stack operations ─────────────────────────────────────
    void  push(Value v);
    Value pop();
    Value peek() const;    // Look at top without popping

    // ── Runtime error helper ──────────────────────────────────
    [[noreturn]] void runtimeError(const std::string& msg);
};
