#pragma once
#include "Token.h"
#include <vector>
#include <memory>
#include <string>


class BinaryExpr;
class GroupingExpr;
class LiteralExpr;
class UnaryExpr;
class VariableExpr;
class AssignExpr;
class CallExpr;

class BlockStmt;
class ExpressionStmt;
class FunctionStmt;
class IfStmt;
class PrintStmt;
class ReturnStmt;
class VarStmt;
class WhileStmt;

class ASTVisitor {
public:
    virtual void visitBinaryExpr(BinaryExpr* expr) = 0;
    virtual void visitGroupingExpr(GroupingExpr* expr) = 0;
    virtual void visitLiteralExpr(LiteralExpr* expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr* expr) = 0;
    virtual void visitVariableExpr(VariableExpr* expr) = 0;
    virtual void visitAssignExpr(AssignExpr* expr) = 0;
    virtual void visitCallExpr(CallExpr* expr) = 0;

    virtual void visitBlockStmt(BlockStmt* stmt) = 0;
    virtual void visitExpressionStmt(ExpressionStmt* stmt) = 0;
    virtual void visitFunctionStmt(FunctionStmt* stmt) = 0;
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    virtual void visitPrintStmt(PrintStmt* stmt) = 0;
    virtual void visitReturnStmt(ReturnStmt* stmt) = 0;
    virtual void visitVarStmt(VarStmt* stmt) = 0;
    virtual void visitWhileStmt(WhileStmt* stmt) = 0;
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
};

class Expression : public ASTNode {};
class Statement : public ASTNode {};

// --- Expression Implementations ---

class BinaryExpr : public Expression {
public:
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
    BinaryExpr(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitBinaryExpr(this); }
};

class GroupingExpr : public Expression {
public:
    std::unique_ptr<Expression> expression;
    GroupingExpr(std::unique_ptr<Expression> expression) : expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitGroupingExpr(this); }
};

class LiteralExpr : public Expression {
public:
    std::string value;
    TokenType typeHint; // Helper to know if it's int/float/bool
    LiteralExpr(std::string value, TokenType typeHint) : value(value), typeHint(typeHint) {}
    void accept(ASTVisitor* visitor) override { visitor->visitLiteralExpr(this); }
};

class UnaryExpr : public Expression {
public:
    Token op;
    std::unique_ptr<Expression> right;
    UnaryExpr(Token op, std::unique_ptr<Expression> right) : op(op), right(std::move(right)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitUnaryExpr(this); }
};

class VariableExpr : public Expression {
public:
    Token name;
    VariableExpr(Token name) : name(name) {}
    void accept(ASTVisitor* visitor) override { visitor->visitVariableExpr(this); }
};

class AssignExpr : public Expression {
public:
    Token name;
    std::unique_ptr<Expression> value;
    AssignExpr(Token name, std::unique_ptr<Expression> value) : name(name), value(std::move(value)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitAssignExpr(this); }
};

class CallExpr : public Expression {
public:
    std::unique_ptr<Expression> callee;
    Token paren; // Location for errors
    std::vector<std::unique_ptr<Expression>> arguments;
    CallExpr(std::unique_ptr<Expression> callee, Token paren, std::vector<std::unique_ptr<Expression>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitCallExpr(this); }
};

// --- Statement Implementations ---

class BlockStmt : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    BlockStmt(std::vector<std::unique_ptr<Statement>> statements) : statements(std::move(statements)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitBlockStmt(this); }
};

class ExpressionStmt : public Statement {
public:
    std::unique_ptr<Expression> expression;
    ExpressionStmt(std::unique_ptr<Expression> expression) : expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitExpressionStmt(this); }
};

class FunctionStmt : public Statement {
public:
    Token name;
    Token returnType; 
    std::vector<Token> params;      // Parameter names
    std::vector<Token> paramTypes;  // Parameter types
    std::vector<std::unique_ptr<Statement>> body;
    FunctionStmt(Token name, Token returnType, std::vector<Token> params, std::vector<Token> paramTypes, std::vector<std::unique_ptr<Statement>> body)
        : name(name), returnType(returnType), params(params), paramTypes(paramTypes), body(std::move(body)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitFunctionStmt(this); }
};

class IfStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch;
    IfStmt(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> thenBranch, std::unique_ptr<Statement> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitIfStmt(this); }
};

class PrintStmt : public Statement {
public:
    std::unique_ptr<Expression> expression;
    PrintStmt(std::unique_ptr<Expression> expression) : expression(std::move(expression)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitPrintStmt(this); }
};

class ReturnStmt : public Statement {
public:
    Token keyword;
    std::unique_ptr<Expression> value;
    ReturnStmt(Token keyword, std::unique_ptr<Expression> value) : keyword(keyword), value(std::move(value)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitReturnStmt(this); }
};

class VarStmt : public Statement {
public:
    Token name;
    Token type; 
    std::unique_ptr<Expression> initializer;
    VarStmt(Token name, Token type, std::unique_ptr<Expression> initializer)
        : name(name), type(type), initializer(std::move(initializer)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitVarStmt(this); }
};

class WhileStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    WhileStmt(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : condition(std::move(condition)), body(std::move(body)) {}
    void accept(ASTVisitor* visitor) override { visitor->visitWhileStmt(this); }
};