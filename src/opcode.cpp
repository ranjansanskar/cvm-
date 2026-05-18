#include "opcode.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

void Chunk::emit(OpCode op) {
    code.push_back(static_cast<uint8_t>(op));
}

void Chunk::emit(OpCode op, uint8_t operand) {
    code.push_back(static_cast<uint8_t>(op));
    code.push_back(operand);
}

void Chunk::emit(OpCode op, uint16_t operand) {
    code.push_back(static_cast<uint8_t>(op));
    code.push_back((operand >> 8) & 0xFF);
    code.push_back(operand & 0xFF);
}

int Chunk::addConstant(Value value) {
    constants.push_back(value);
    return constants.size() - 1;
}

int Chunk::addName(const std::string& name) {
    auto it = std::find(names.begin(), names.end(), name);
    if (it != names.end()) {
        return std::distance(names.begin(), it);
    }
    names.push_back(name);
    return names.size() - 1;
}

static int simpleInstruction(const std::string& name, int offset) {
    std::cout << name << "\n";
    return offset + 1;
}

static int constantInstruction(const std::string& name, const Chunk& chunk, int offset) {
    uint8_t constant = chunk.code[offset + 1];
    std::cout << std::left << std::setw(16) << name << " " 
              << std::right << std::setw(4) << static_cast<int>(constant) << " '";
    std::cout << chunk.constants[constant] << "'\n";
    return offset + 2;
}

static int globalInstruction(const std::string& name, const Chunk& chunk, int offset) {
    uint8_t constant = chunk.code[offset + 1];
    std::cout << std::left << std::setw(16) << name << " " 
              << std::right << std::setw(4) << static_cast<int>(constant) << " '" 
              << chunk.names[constant] << "'\n";
    return offset + 2;
}

static int jumpInstruction(const std::string& name, int sign, const Chunk& chunk, int offset) {
    uint16_t jump = static_cast<uint16_t>(chunk.code[offset + 1] << 8);
    jump |= chunk.code[offset + 2];
    std::cout << std::left << std::setw(16) << name << " " 
              << std::right << std::setw(4) << offset << " -> " 
              << offset + 3 + sign * jump << "\n";
    return offset + 3;
}

void Chunk::disassemble(const std::string& title) const {
    if (!title.empty()) {
        std::cout << "=== " << title << " ===\n";
    }

    for (int offset = 0; offset < code.size();) {
        std::cout << std::setfill('0') << std::setw(4) << offset << " " << std::setfill(' ');

        uint8_t instruction = code[offset];
        switch (static_cast<OpCode>(instruction)) {
            case OpCode::OP_CONSTANT:
                offset = constantInstruction("OP_CONSTANT", *this, offset);
                break;
            case OpCode::OP_POP:
                offset = simpleInstruction("OP_POP", offset);
                break;
            case OpCode::OP_ADD:
                offset = simpleInstruction("OP_ADD", offset);
                break;
            case OpCode::OP_SUBTRACT:
                offset = simpleInstruction("OP_SUBTRACT", offset);
                break;
            case OpCode::OP_MULTIPLY:
                offset = simpleInstruction("OP_MULTIPLY", offset);
                break;
            case OpCode::OP_DIVIDE:
                offset = simpleInstruction("OP_DIVIDE", offset);
                break;
            case OpCode::OP_NEGATE:
                offset = simpleInstruction("OP_NEGATE", offset);
                break;
            case OpCode::OP_EQUAL:
                offset = simpleInstruction("OP_EQUAL", offset);
                break;
            case OpCode::OP_NOT_EQUAL:
                offset = simpleInstruction("OP_NOT_EQUAL", offset);
                break;
            case OpCode::OP_LESS:
                offset = simpleInstruction("OP_LESS", offset);
                break;
            case OpCode::OP_GREATER:
                offset = simpleInstruction("OP_GREATER", offset);
                break;
            case OpCode::OP_LESS_EQUAL:
                offset = simpleInstruction("OP_LESS_EQUAL", offset);
                break;
            case OpCode::OP_GREATER_EQUAL:
                offset = simpleInstruction("OP_GREATER_EQUAL", offset);
                break;
            case OpCode::OP_NOT:
                offset = simpleInstruction("OP_NOT", offset);
                break;
            case OpCode::OP_TRUE:
                offset = simpleInstruction("OP_TRUE", offset);
                break;
            case OpCode::OP_FALSE:
                offset = simpleInstruction("OP_FALSE", offset);
                break;
            case OpCode::OP_SET_GLOBAL:
                offset = globalInstruction("OP_SET_GLOBAL", *this, offset);
                break;
            case OpCode::OP_GET_GLOBAL:
                offset = globalInstruction("OP_GET_GLOBAL", *this, offset);
                break;
            case OpCode::OP_JUMP:
                offset = jumpInstruction("OP_JUMP", 1, *this, offset);
                break;
            case OpCode::OP_JUMP_IF_FALSE:
                offset = jumpInstruction("OP_JUMP_IF_FALSE", 1, *this, offset);
                break;
            case OpCode::OP_LOOP:
                offset = jumpInstruction("OP_LOOP", -1, *this, offset);
                break;
            case OpCode::OP_PRINT:
                offset = simpleInstruction("OP_PRINT", offset);
                break;
            case OpCode::OP_INPUT:
                offset = simpleInstruction("OP_INPUT", offset);
                break;
            case OpCode::OP_HALT:
                offset = simpleInstruction("OP_HALT", offset);
                break;
            default:
                std::cout << "Unknown opcode " << static_cast<int>(instruction) << "\n";
                offset++;
                break;
        }
    }
}
