#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "compiler/scanner.h"
#include "compiler/parser.h"
#include "util/error-handler.h"
#include "compiler/semantic-analyzer.h"
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: nanoc <script>" << std::endl;
        return 1;
    }

    // 1. Read File
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- Compiling " << argv[1] << " ---" << std::endl;

    // 2. Scanning (Lexical Analysis)
    std::cout << "[Phase 1] Scanning..." << std::endl;
    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();

    if (ErrorHandler::hadError) return 65;

    // Debug: Print tokens
    // for (const auto& t : tokens) std::cout << t.toString() << std::endl;

    // 3. Parsing (Syntax Analysis)
    std::cout << "[Phase 2] Parsing..." << std::endl;
    Parser parser(tokens);
    std::vector<std::unique_ptr<Statement>> ast = parser.parse();

    if (ErrorHandler::hadError) return 65;

    // 4. Semantic Analysis
    std::cout << "[Phase 3] Semantic Analysis..." << std::endl;
    SemanticAnalyzer analyzer;
    analyzer.analyze(ast);

    if (ErrorHandler::hadError) {
        std::cerr << "Compilation failed with semantic errors." << std::endl;
        return 70;
    }

    std::cout << "Success! Valid NanoScript code." << std::endl;
    return 0;
}