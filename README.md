# CVM++

CVM++ is a lightweight, custom scripting language in C++ that compiles down to a proprietary bytecode, which is executed by a custom-built, stack-based Virtual Machine (VM).

## Architecture
- Lexer (Tokenizer)
- Parser (Recursive Descent)
- AST (Abstract Syntax Tree)
- Compiler (Bytecode Generator)
- Virtual Machine (Stack-based)

## Build Instructions
```bash
mkdir build
cd build
cmake ..
make
```

## Running
```bash
./cvm scripts/hello.cvm
```