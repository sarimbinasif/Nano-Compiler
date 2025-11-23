#include "semantic-analyzer.h"
#include "../util/error-handler.h"

void SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        stmt->accept(this);
    }
}

// --- Scopes ---

void SemanticAnalyzer::visitBlockStmt(BlockStmt* stmt) {
    symbolTable.beginScope();
    for (const auto& s : stmt->statements) {
        s->accept(this);
    }
    symbolTable.endScope();
}

void SemanticAnalyzer::visitVarStmt(VarStmt* stmt) {
    // 1. Check initializer
    if (stmt->initializer) {
        stmt->initializer->accept(this);
        // Check if initializer type matches variable type
        if (lastComputedType != stmt->type.type && lastComputedType != TokenType::END_OF_FILE) {
             // Note: END_OF_FILE used here as a placeholder for "unknown/void"
             // Simplified check. In real compiler, we check implicit casting (int->float).
             if (stmt->type.type == TokenType::TYPE_FLOAT && lastComputedType == TokenType::TYPE_INT) {
                 // Allowed (Implicit cast)
             } else {
                 ErrorHandler::error(stmt->name.line, "Type mismatch in initialization.");
             }
        }
    }

    // 2. Declare variable
    if (!symbolTable.declare(stmt->name.lexeme, stmt->type.type)) {
        ErrorHandler::error(stmt->name.line, "Variable with this name already declared in this scope.");
    }
}

void SemanticAnalyzer::visitVariableExpr(VariableExpr* expr) {
    SymbolInfo info;
    if (!symbolTable.get(expr->name.lexeme, info)) {
        ErrorHandler::error(expr->name.line, "Undefined variable '" + expr->name.lexeme + "'.");
        lastComputedType = TokenType::END_OF_FILE;
    } else {
        lastComputedType = info.type;
    }
}

void SemanticAnalyzer::visitAssignExpr(AssignExpr* expr) {
    expr->value->accept(this);
    TokenType valType = lastComputedType;

    SymbolInfo info;
    if (!symbolTable.get(expr->name.lexeme, info)) {
        ErrorHandler::error(expr->name.line, "Undefined variable '" + expr->name.lexeme + "'.");
    } else {
        if (info.type != valType) {
            // Very basic strict type checking
             ErrorHandler::error(expr->name.line, "Type mismatch in assignment.");
        }
        lastComputedType = info.type;
    }
}

void SemanticAnalyzer::visitBinaryExpr(BinaryExpr* expr) {
    expr->left->accept(this);
    TokenType leftType = lastComputedType;
    expr->right->accept(this);
    TokenType rightType = lastComputedType;

    // Simple type checking logic
    if (expr->op.type == TokenType::PLUS || expr->op.type == TokenType::MINUS || 
        expr->op.type == TokenType::STAR || expr->op.type == TokenType::SLASH) {
        
        if (leftType == TokenType::TYPE_INT && rightType == TokenType::TYPE_INT) {
            lastComputedType = TokenType::TYPE_INT;
        } else if ((leftType == TokenType::TYPE_INT || leftType == TokenType::TYPE_FLOAT) &&
                   (rightType == TokenType::TYPE_INT || rightType == TokenType::TYPE_FLOAT)) {
            lastComputedType = TokenType::TYPE_FLOAT;
        } else {
            ErrorHandler::error(expr->op.line, "Operands must be numbers.");
            lastComputedType = TokenType::END_OF_FILE;
        }
    } 
    else if (expr->op.type == TokenType::GREATER || expr->op.type == TokenType::LESS ||
             expr->op.type == TokenType::GREATER_EQUAL || expr->op.type == TokenType::LESS_EQUAL ||
             expr->op.type == TokenType::EQUAL_EQUAL || expr->op.type == TokenType::BANG_EQUAL) {
        // Comparison returns BOOL
        // Allow comparing int with int, float with float, or int with float
        if ((leftType == TokenType::TYPE_INT || leftType == TokenType::TYPE_FLOAT) &&
            (rightType == TokenType::TYPE_INT || rightType == TokenType::TYPE_FLOAT)) {
            // Numeric comparison is valid
        } else if (leftType == TokenType::TYPE_BOOL && rightType == TokenType::TYPE_BOOL) {
            // Bool comparison is valid
        } else {
            ErrorHandler::error(expr->op.line, "Cannot compare incompatible types.");
        }
        lastComputedType = TokenType::TYPE_BOOL;
    }
}

void SemanticAnalyzer::visitLiteralExpr(LiteralExpr* expr) {
    lastComputedType = expr->typeHint;
}

void SemanticAnalyzer::visitGroupingExpr(GroupingExpr* expr) {
    expr->expression->accept(this);
}

void SemanticAnalyzer::visitUnaryExpr(UnaryExpr* expr) {
    expr->right->accept(this);
    // If operator is BANG (!), type must be BOOL
    if (expr->op.type == TokenType::BANG && lastComputedType != TokenType::TYPE_BOOL) {
        ErrorHandler::error(expr->op.line, "Expected boolean for '!' operator.");
    }
    // If operator is MINUS (-), type must be Number
}

void SemanticAnalyzer::visitIfStmt(IfStmt* stmt) {
    stmt->condition->accept(this);
    if (lastComputedType != TokenType::TYPE_BOOL) {
        // ErrorHandler::error(..., "Condition must be boolean."); // Location hard to get without token in IfStmt
        // For simplicity, we skip line number here or add Token to IfStmt
    }
    stmt->thenBranch->accept(this);
    if (stmt->elseBranch) stmt->elseBranch->accept(this);
}

void SemanticAnalyzer::visitWhileStmt(WhileStmt* stmt) {
    stmt->condition->accept(this);
    if (lastComputedType != TokenType::TYPE_BOOL) {
         // Error: Condition must be bool
    }
    stmt->body->accept(this);
}

void SemanticAnalyzer::visitFunctionStmt(FunctionStmt* stmt) {
    symbolTable.declare(stmt->name.lexeme, stmt->returnType.type); // Add func to scope
    
    // Save old state
    bool enclosingFunction = inFunction;
    TokenType enclosingType = currentFunctionReturnType;
    
    inFunction = true;
    currentFunctionReturnType = stmt->returnType.type;

    symbolTable.beginScope();
    for(size_t i=0; i < stmt->params.size(); i++) {
        symbolTable.declare(stmt->params[i].lexeme, stmt->paramTypes[i].type);
    }
    
    for (const auto& s : stmt->body) {
        s->accept(this);
    }

    symbolTable.endScope();
    
    inFunction = enclosingFunction;
    currentFunctionReturnType = enclosingType;
}

void SemanticAnalyzer::visitReturnStmt(ReturnStmt* stmt) {
    if (!inFunction) {
        ErrorHandler::error(stmt->keyword.line, "Cannot return from top-level code.");
    }
    if (stmt->value) {
        stmt->value->accept(this);
        if (lastComputedType != currentFunctionReturnType) {
             ErrorHandler::error(stmt->keyword.line, "Return value does not match function type.");
        }
    }
}

void SemanticAnalyzer::visitPrintStmt(PrintStmt* stmt) {
    stmt->expression->accept(this);
}

void SemanticAnalyzer::visitExpressionStmt(ExpressionStmt* stmt) {
    stmt->expression->accept(this);
}

void SemanticAnalyzer::visitCallExpr(CallExpr* expr) {
    // 1. Check if callee exists (usually a function name)
    // For this simple project, we aren't tracking Function Signatures in the SymbolTable 
    // (just that the name exists).
    // In a real compiler, SymbolInfo would hold vector<Type> paramTypes.
    
    expr->callee->accept(this); // Check if function name exists
    TokenType functionReturnType = lastComputedType; // Store the function's return type
    
    // Check arguments
    for (const auto& arg : expr->arguments) {
        arg->accept(this);
    }
    
    // Set the result type to the function's return type
    lastComputedType = functionReturnType;
}