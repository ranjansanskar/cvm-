# CVM++ 

**A Stack-Based Virtual Machine & Custom Compiler built in C++**

CVM++ is a lightweight, custom scripting language that parses a proprietary syntax, compiles it down to bytecode, and executes it on a custom-built stack-based Virtual Machine. The project features a complete compiler frontend (lexer and recursive-descent parser) and backend (bytecode generator and VM engine).

---

## 🏗️ Architecture

The compilation and execution pipeline is modeled as follows:

1. **Lexer (Tokenizer)**: Scans the raw source code string and converts it into a sequence of meaningful Tokens.
2. **Parser**: A recursive-descent parser that processes the tokens and constructs an Abstract Syntax Tree (AST).
3. **Compiler**: Traverses the AST and emits proprietary bytecode (opcodes) into a "Chunk".
4. **Virtual Machine**: A stack-based execution engine that runs the compiled bytecode instructions.

---

## ⚡ Language Features

CVM++ supports a variety of modern programming constructs:

- **Variables**: Declaration via `let`, dynamic reassignment.
- **Data Types**: `Integer` (64-bit) and `Boolean` (`true` / `false`).
- **Arithmetic**: Addition (`+`), Subtraction (`-`), Multiplication (`*`), Division (`/`).
- **Comparison**: `==`, `!=`, `<`, `>`, `<=`, `>=`.
- **Logical**: Unary NOT (`!`).
- **Control Flow**: `if / else` branching and `while` loops.
- **I/O Operations**: `input` for reading user input, `print` for outputting to the console.
- **Comments**: Supports `//` single-line comments.

---

## 💻 Sample Code

Here is a quick example of a CVM++ script calculating the first 10 numbers in the Fibonacci sequence:

```cvm
// fibonacci.cvm
let a = 0;
let b = 1;
let n = 10;
let i = 0;

while (i < n) {
    print a;

    let temp = a + b;
    a = b;
    b = temp;

    i = i + 1;
}
```

*You can find more examples in the `scripts/` directory!*

---

## 🚀 Build Instructions

### Option 1: Using Make / CMake
If you have CMake installed:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Option 2: Using g++ directly
If you prefer compiling directly via g++ from the root directory:
```bash
g++ -I include src/main.cpp src/lexer.cpp src/parser.cpp src/compiler.cpp src/vm.cpp src/opcode.cpp -o cvm.exe
```

---

## 🎮 Usage

CVM++ provides a REPL (Read-Eval-Print Loop) for interactive programming, as well as a File Runner to execute scripts.

### 1. Interactive REPL Mode
Run the executable without any arguments to start the interactive shell:
```bash
./cvm
```
```
CVM++ v0.1.0 — Stack-Based Virtual Machine & Custom Compiler
Type 'exit' to quit, 'help' for usage hints.

>>> let x = 10;
>>> let y = 20;
>>> print x + y;
30
```

### 2. File Runner Mode
Pass the path to a `.cvm` script as an argument to execute it:
```bash
./cvm scripts/arithonacci.cvm
```

---

## 🧪 Included Test Scripts

The `scripts/` directory contains a variety of scripts to test different functionalities:

- `arithmetic.cvm`: Operator precedence and basic math.
- `variables.cvm`: Variable declarations and reassignments.
- `booleans.cvm`: Truth values and logical operations.
- `comparison.cvm`: Relational checking.
- `if_else.cvm`: Simple and nested conditional branching.
- `while_loop.cvm`: Iterative loops and sum accumulation.
- `fibonacci.cvm`: Sequence generation.
- `input_output.cvm`: Testing I/O capabilities.
- `edge_cases.cvm`: Runtime error handling (e.g. Division by zero, Undefined Variables).