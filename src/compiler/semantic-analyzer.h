#pragma once
#include "../data/AST.h"
#include "../util/symbol-table.h"

class SemanticAnalyzer : public ASTVisitor {
private:
    SymbolTable symbolTable;
    TokenType currentFunctionReturnType;
    bool inFunction = false;

    // Helper to map generic tokens to types if needed
    TokenType getResultType(TokenType t1, TokenType t2, TokenType op);

public:
    void analyze(const std::vector<std::unique_ptr<Statement>>& statements);

    // Visit methods
    void visitBlockStmt(BlockStmt* stmt) override;
    void visitVarStmt(VarStmt* stmt) override;
    void visitVariableExpr(VariableExpr* expr) override;
    void visitAssignExpr(AssignExpr* expr) override;
    void visitBinaryExpr(BinaryExpr* expr) override;
    
    // Minimal implementations for others to satisfy interface
    void visitGroupingExpr(GroupingExpr* expr) override;
    void visitLiteralExpr(LiteralExpr* expr) override;
    void visitUnaryExpr(UnaryExpr* expr) override;
    void visitCallExpr(CallExpr* expr) override;
    void visitExpressionStmt(ExpressionStmt* stmt) override;
    void visitFunctionStmt(FunctionStmt* stmt) override;
    void visitIfStmt(IfStmt* stmt) override;
    void visitPrintStmt(PrintStmt* stmt) override;
    void visitReturnStmt(ReturnStmt* stmt) override;
    void visitWhileStmt(WhileStmt* stmt) override;

    // We need a way to store the "Type" of the last evaluated expression
    // to check against expected types.
    TokenType lastComputedType; 
};