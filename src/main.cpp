// ─────────────────────────────────────────────────────────────────────────────
//  CVM++ — main.cpp
//  Entry point: REPL mode (no args) or File-runner mode (one arg).
//
//  Phase 0 milestone: compiles and prints a version banner.
//  Phases 2-7 will wire in the real Lexer → Parser → Compiler → VM pipeline.
// ─────────────────────────────────────────────────────────────────────────────
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// ── Version ──────────────────────────────────────────────────────────────────
static constexpr const char* CVM_VERSION = "0.1.0";

// ── Forward declarations (implemented in later phases) ───────────────────────
void runSource(const std::string& source);
void runREPL();
void runFile(const std::string& path);

// ─────────────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    std::cout << "CVM++ v" << CVM_VERSION
              << " — Stack-Based Virtual Machine & Custom Compiler\n";

    if (argc == 1) {
        // No arguments → interactive REPL
        runREPL();
    } else if (argc == 2) {
        // One argument → execute a .cvm file
        runFile(argv[1]);
    } else {
        std::cerr << "Usage: cvm [script.cvm]\n";
        return 1;
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Stub: run a string of CVM source code through the pipeline.
//  Replaced incrementally as phases are implemented.
// ─────────────────────────────────────────────────────────────────────────────
void runSource(const std::string& source) {
    // TODO (Phase 2): Lexer   lexer(source);  auto tokens = lexer.tokenize();
    // TODO (Phase 3): Parser  parser(tokens); auto ast    = parser.parse();
    // TODO (Phase 5): Compiler compiler;      auto chunk  = compiler.compile(*ast);
    // TODO (Phase 6): VM      vm;             vm.execute(chunk);
    std::cout << "[stub] Running " << source.size() << " bytes of CVM source.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
//  REPL — Read-Eval-Print Loop
// ─────────────────────────────────────────────────────────────────────────────
void runREPL() {
    std::string line;
    std::cout << "Type 'exit' to quit, 'help' for usage hints.\n\n";

    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) {
            std::cout << "\nBye!\n";
            break;
        }
        if (line == "exit" || line == "quit") {
            std::cout << "Bye!\n";
            break;
        }
        if (line == "help") {
            std::cout << "CVM++ REPL — type any CVM statement and press Enter.\n"
                      << "Example:\n"
                      << "  >>> let x = 10;\n"
                      << "  >>> print x;\n";
            continue;
        }
        if (line.empty()) continue;
        runSource(line);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  File Runner
// ─────────────────────────────────────────────────────────────────────────────
void runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file '" << path << "'\n";
        std::exit(1);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    runSource(ss.str());
}
