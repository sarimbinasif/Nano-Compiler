#pragma once

#include "Token.h"
#include <vector>
#include <memory> // For std::unique_ptr

// --- Visitor Interface ---
// Forward-declare all your node types
class BinaryExpr;
class LiteralExpr;
class VarDeclStmt;
class IfStmt;
// ... etc.

class ASTVisitor {
public:
    virtual void visitBinaryExpr(BinaryExpr* expr) = 0;
    virtual void visitLiteralExpr(LiteralExpr* expr) = 0;
    virtual void visitVarDeclStmt(VarDeclStmt* stmt) = 0;
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    // ... one visit method for each node class
};

// --- Base Classes ---
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor* visitor) = 0;
};

class Expression : public ASTNode {};
class Statement : public ASTNode {};

// --- Concrete Expression Nodes ---
class BinaryExpr : public Expression {
public:
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;

    BinaryExpr(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right)
        : left(std::move(left)), op(op), right(std::move(right)) {}

    void accept(ASTVisitor* visitor) override {
        visitor->visitBinaryExpr(this);
    }
};

class LiteralExpr : public Expression {
public:
    // In a real implementation, this would be a std::any or std::variant
    std::string value; // Simple version

    LiteralExpr(std::string value) : value(value) {}

    void accept(ASTVisitor* visitor) override {
        visitor->visitLiteralExpr(this);
    }
};

// --- Concrete Statement Nodes ---
class VarDeclStmt : public Statement {
public:
    Token type; // e.g., "int"
    Token name; // e.g., "x"
    std::unique_ptr<Expression> initializer;

    VarDeclStmt(Token type, Token name, std::unique_ptr<Expression> init)
        : type(type), name(name), initializer(std::move(init)) {}
    
    void accept(ASTVisitor* visitor) override {
        visitor->visitVarDeclStmt(this);
    }
};

// ... etc. for IfStmt, WhileStmt, FuncDefStmt ...