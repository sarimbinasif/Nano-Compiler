#pragma once
#include <vector>
#include <memory>
#include "../data/token.h"
#include "../data/AST.h"

class Parser {
private:
    std::vector<Token> tokens;
    int current = 0;

    Statement* declaration();
    Statement* varDeclaration();
    Statement* funcDeclaration(std::string kind);
    Statement* statement();
    Statement* ifStatement();
    Statement* whileStatement();
    Statement* forStatement();
    Statement* returnStatement();
    Statement* printStatement();
    std::vector<std::unique_ptr<Statement>> block();
    Statement* expressionStatement();
    
    Expression* expression();
    Expression* assignment();
    Expression* orExpr();
    Expression* andExpr();
    Expression* equality();
    Expression* comparison();
    Expression* term();
    Expression* factor();
    Expression* unary();
    Expression* call();
    Expression* finishCall(Expression* callee);
    Expression* primary();

    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type);
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    Token consume(TokenType type, std::string message);
    void synchronize();

public:
    Parser(std::vector<Token> tokens);
    std::vector<std::unique_ptr<Statement>> parse();
};