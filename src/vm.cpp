#include "vm.h"
#include <iostream>

void VM::reset() {
    m_stack.clear();
    m_globals.clear();
    m_ip = 0;
    m_chunk = nullptr;
}

Value VM::getGlobal(const std::string& name) const {
    auto it = m_globals.find(name);
    if (it == m_globals.end()) {
        throw RuntimeError("Undefined variable '" + name + "'.");
    }
    return it->second;
}

uint8_t VM::readByte() {
    if (m_ip >= m_chunk->code.size()) {
        runtimeError("Unexpected end of bytecode.");
    }
    return m_chunk->code[m_ip++];
}

uint16_t VM::readShort() {
    if (m_ip + 1 >= m_chunk->code.size()) {
        runtimeError("Unexpected end of bytecode reading short.");
    }
    uint16_t result = (m_chunk->code[m_ip] << 8) | m_chunk->code[m_ip + 1];
    m_ip += 2;
    return result;
}

void VM::push(Value v) {
    m_stack.push_back(v);
}

Value VM::pop() {
    if (m_stack.empty()) {
        runtimeError("Stack underflow.");
    }
    Value v = m_stack.back();
    m_stack.pop_back();
    return v;
}

Value VM::peek() const {
    if (m_stack.empty()) {
        // Can't use runtimeError here as it's not marked const, 
        // but peek doesn't mutate. Actually runtimeError throws, so it's fine.
        throw RuntimeError("Stack underflow on peek.");
    }
    return m_stack.back();
}

void VM::runtimeError(const std::string& msg) {
    throw RuntimeError(msg);
}

void VM::execute(const Chunk& chunk) {
    m_chunk = &chunk;
    m_ip = 0;

    while (true) {
        OpCode instruction = static_cast<OpCode>(readByte());

        switch (instruction) {
            case OpCode::OP_CONSTANT: {
                uint8_t index = readByte();
                if (index >= m_chunk->constants.size()) runtimeError("Invalid constant index.");
                push(m_chunk->constants[index]);
                break;
            }
            case OpCode::OP_POP: {
                pop();
                break;
            }
            case OpCode::OP_ADD: {
                Value b = pop();
                Value a = pop();
                push(a + b);
                break;
            }
            case OpCode::OP_SUBTRACT: {
                Value b = pop();
                Value a = pop();
                push(a - b);
                break;
            }
            case OpCode::OP_MULTIPLY: {
                Value b = pop();
                Value a = pop();
                push(a * b);
                break;
            }
            case OpCode::OP_DIVIDE: {
                Value b = pop();
                Value a = pop();
                if (b == 0) runtimeError("Division by zero.");
                push(a / b);
                break;
            }
            case OpCode::OP_NEGATE: {
                Value a = pop();
                push(-a);
                break;
            }
            case OpCode::OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(a == b ? 1 : 0);
                break;
            }
            case OpCode::OP_NOT_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(a != b ? 1 : 0);
                break;
            }
            case OpCode::OP_LESS: {
                Value b = pop();
                Value a = pop();
                push(a < b ? 1 : 0);
                break;
            }
            case OpCode::OP_GREATER: {
                Value b = pop();
                Value a = pop();
                push(a > b ? 1 : 0);
                break;
            }
            case OpCode::OP_LESS_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(a <= b ? 1 : 0);
                break;
            }
            case OpCode::OP_GREATER_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(a >= b ? 1 : 0);
                break;
            }
            case OpCode::OP_NOT: {
                Value a = pop();
                push(a == 0 ? 1 : 0);
                break;
            }
            case OpCode::OP_TRUE: {
                push(1);
                break;
            }
            case OpCode::OP_FALSE: {
                push(0);
                break;
            }
            case OpCode::OP_SET_GLOBAL: {
                uint8_t nameIdx = readByte();
                if (nameIdx >= m_chunk->names.size()) runtimeError("Invalid variable name index.");
                m_globals[m_chunk->names[nameIdx]] = pop();
                break;
            }
            case OpCode::OP_GET_GLOBAL: {
                uint8_t nameIdx = readByte();
                if (nameIdx >= m_chunk->names.size()) runtimeError("Invalid variable name index.");
                auto it = m_globals.find(m_chunk->names[nameIdx]);
                if (it == m_globals.end()) runtimeError("Undefined variable.");
                push(it->second);
                break;
            }
            case OpCode::OP_JUMP: {
                uint16_t offset = readShort();
                m_ip += offset;
                break;
            }
            case OpCode::OP_JUMP_IF_FALSE: {
                uint16_t offset = readShort();
                if (pop() == 0) m_ip += offset;
                break;
            }
            case OpCode::OP_LOOP: {
                uint16_t offset = readShort();
                m_ip -= offset;
                break;
            }
            case OpCode::OP_PRINT: {
                std::cout << pop() << std::endl;
                break;
            }
            case OpCode::OP_INPUT: {
                int64_t val;
                std::cin >> val;
                push(val);
                break;
            }
            case OpCode::OP_HALT: {
                return;
            }
            default: {
                runtimeError("Unknown opcode.");
            }
        }
    }
}
