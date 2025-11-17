#pragma once

#include <vector>
#include "../Data/Token.h"
#include "../Data/AST.h"

class Parser {
private:
    std::vector<Token> tokens;
    int current = 0;

    // --- Grammar rule helper methods ---
    // These functions map to your language's grammar
    Statement* statement();
    Statement* varDeclaration();
    Statement* ifStatement();
    // ...
    Expression* expression();
    Expression* term();
    Expression* factor();
    // ...

    // --- Utility methods ---
    bool isAtEnd();
    Token advance();
    Token peek();
    bool check(TokenType type);
    bool match(std::vector<TokenType> types);
    // ...

public:
    Parser(std::vector<Token> tokens) : tokens(tokens) {}

    // The main function that runs the parse
    std::vector<std::unique_ptr<Statement>> parse(); 
};