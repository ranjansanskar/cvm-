// tests/test_vm.cpp
// Integration tests for the VM (Phase 6).
#include <iostream>
#include <cassert>
#include "vm.h"
#include "opcode.h"

void test_vm_arithmetic() {
    Chunk chunk;
    int c1 = chunk.addConstant(10);
    int c2 = chunk.addConstant(20);
    
    chunk.emit(OpCode::OP_CONSTANT, static_cast<uint8_t>(c1));
    chunk.emit(OpCode::OP_CONSTANT, static_cast<uint8_t>(c2));
    chunk.emit(OpCode::OP_ADD);
    chunk.emit(OpCode::OP_HALT);

    VM vm;
    vm.execute(chunk);

    assert(vm.getStack().size() == 1);
    assert(vm.getStack().back() == 30);
    std::cout << "[test_vm] test_vm_arithmetic PASS\n";
}

void test_vm_variables() {
    Chunk chunk;
    int const_idx = chunk.addConstant(42);
    int name_idx = chunk.addName("myVar");
    
    // let myVar = 42;
    chunk.emit(OpCode::OP_CONSTANT, static_cast<uint8_t>(const_idx));
    chunk.emit(OpCode::OP_SET_GLOBAL, static_cast<uint8_t>(name_idx));
    // get myVar
    chunk.emit(OpCode::OP_GET_GLOBAL, static_cast<uint8_t>(name_idx));
    chunk.emit(OpCode::OP_HALT);

    VM vm;
    vm.execute(chunk);

    assert(vm.getGlobal("myVar") == 42);
    assert(vm.getStack().size() == 1);
    assert(vm.getStack().back() == 42);
    std::cout << "[test_vm] test_vm_variables PASS\n";
}

void test_vm_control_flow() {
    // Simulate:
    // push 0 (false)
    // jump_if_false over the 'push 100'
    // push 100
    // halt
    Chunk chunk;
    chunk.emit(OpCode::OP_FALSE);
    
    chunk.emit(OpCode::OP_JUMP_IF_FALSE, static_cast<uint16_t>(2));
    
    int const_idx = chunk.addConstant(100);
    chunk.emit(OpCode::OP_CONSTANT, static_cast<uint8_t>(const_idx));
    
    chunk.emit(OpCode::OP_HALT);
    
    VM vm;
    vm.execute(chunk);
    
    // Since condition was false, the OP_CONSTANT is skipped. Stack should be empty (condition was popped)
    assert(vm.getStack().empty());
    std::cout << "[test_vm] test_vm_control_flow PASS\n";
}

int main() {
    test_vm_arithmetic();
    test_vm_variables();
    test_vm_control_flow();
    std::cout << "[test_vm] Phase 6 tests PASS\n";
    return 0;
}
