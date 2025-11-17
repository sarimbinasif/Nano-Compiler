#pragma once

#include "../data/AST.h"
#include "../util/symbol-table.h"
#include <iostream>

class SemanticAnalyzer : public ASTVisitor {
private:
    SymbolTable symbolTable;
    // You'll need an ErrorHandler class here

public:
    // This is the main function you call
    void analyze(const std::vector<std::unique_ptr<Statement>>& statements) {
        for (const auto& stmt : statements) {
            stmt->accept(this);
        }
    }

    // --- Visitor Methods ---
    void visitVarDeclStmt(VarDeclStmt* stmt) override {
        // 1. Check if 'stmt->name' is already in 'symbolTable'
        // 2. If not, add it to 'symbolTable' with its type
        // 3. Analyze the initializer: stmt->initializer->accept(this)
        std::cout << "Semantically checking var " << stmt->name.lexeme << std::endl;
    }

    void visitBinaryExpr(BinaryExpr* expr) override {
        // 1. Get type of left: expr->left->accept(this)
        // 2. Get type of right: expr->right->accept(this)
        // 3. Check if 'expr->op' is valid for those two types
        //    (e.g., error if you try to "int" * "bool")
        std::cout << "Semantically checking a binary expression" << std::endl;
    }

    void visitLiteralExpr(LiteralExpr* expr) override {
        // Base case: just note its type (e.g., "10" is int)
    }

    void visitIfStmt(IfStmt* stmt) override {
        // 1. Analyze the condition: stmt->condition->accept(this)
        // 2. Check if the resulting type of the condition is a 'bool'
        // 3. Analyze the 'then' branch: stmt->thenBranch->accept(this)
        // 4. (If it exists) Analyze the 'else' branch
    }

    // ... etc. for all other visit methods
};