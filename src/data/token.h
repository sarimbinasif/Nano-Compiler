#pragma once

#include <string>
#include <iostream>
#include "token-type.h"

class Token {
public:
    TokenType type;
    std::string lexeme;
    std::string literal; // Storing values as strings for simplicity
    int line;

    Token(TokenType type, std::string lexeme, std::string literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}

    std::string toString() const {
        return std::to_string((int)type) + " " + lexeme + " " + literal;
    }
};