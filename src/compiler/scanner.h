#pragma once

#include <string>
#include <vector>
#include "../Data/Token.h"

class Scanner {
private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    // Helper functions
    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    // ... other helpers for numbers, strings, identifiers ...

public:
    Scanner(std::string source) : source(source) {}

    // The main function that runs the scan
    std::vector<Token> scanTokens();
};