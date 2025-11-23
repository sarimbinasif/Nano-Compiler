#pragma once
#include <vector>
#include <string>
#include <map>
#include "../data/token.h"

class Scanner {
private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    static std::map<std::string, TokenType> keywords;

    bool isAtEnd();
    char advance();
    void addToken(TokenType type);
    void addToken(TokenType type, std::string literal);
    bool match(char expected);
    char peek();
    char peekNext();
    void string();
    void number();
    void identifier();

public:
    Scanner(std::string source);
    std::vector<Token> scanTokens();
};