cmake_minimum_required(VERSION 3.15)
project(CVMPlusPlus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ──────────────────────────────────────────────
#  Main CVM executable
# ──────────────────────────────────────────────
add_executable(cvm
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/opcode.cpp
    src/compiler.cpp
    src/vm.cpp
)
target_include_directories(cvm PRIVATE include)

# ──────────────────────────────────────────────
#  Unit tests (enable with -DBUILD_TESTS=ON)
# ──────────────────────────────────────────────
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()

    # A small shared library of the core modules so tests can link against it
    add_library(cvm_lib STATIC
        src/lexer.cpp
        src/parser.cpp
        src/opcode.cpp
        src/compiler.cpp
        src/vm.cpp
    )
    target_include_directories(cvm_lib PUBLIC include)

    function(add_cvm_test name src)
        add_executable(${name} ${src})
        target_link_libraries(${name} PRIVATE cvm_lib)
        target_include_directories(${name} PRIVATE include)
        add_test(NAME ${name} COMMAND ${name})
    endfunction()

    add_cvm_test(test_lexer    tests/test_lexer.cpp)
    add_cvm_test(test_parser   tests/test_parser.cpp)
    add_cvm_test(test_opcode   tests/test_opcode.cpp)
    add_cvm_test(test_compiler tests/test_compiler.cpp)
    add_cvm_test(test_vm       tests/test_vm.cpp)
endif()
