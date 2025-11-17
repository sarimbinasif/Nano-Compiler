#pragma once

#include <string>
#include "token-type.h"

class Token {
public:
    TokenType type;
    std::string lexeme; // The actual text, e.g., "var" or "myVariable"
    int line;           // For error reporting

    Token(TokenType type, std::string lexeme, int line)
        : type(type), lexeme(lexeme), line(line) {}
};