#include "parser.h"
#include "../util/error-handler.h"

Parser::Parser(std::vector<Token> tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Statement>> Parser::parse() {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!isAtEnd()) {
        Statement* stmt = declaration();
        if (stmt) statements.emplace_back(stmt);
    }
    return statements;
}

Statement* Parser::declaration() {
    try {
        if (match({TokenType::FUNC})) return funcDeclaration("function");
        if (match({TokenType::VAR})) return varDeclaration();
        return statement();
    } catch (std::runtime_error& error) {
        synchronize();
        return nullptr;
    }
}

Statement* Parser::funcDeclaration(std::string kind) {
    Token returnType = consume(TokenType::TYPE_INT, "Expected return type (int/float/bool) before function name."); 
    // Note: For simplicity, assume int return type or you can check for others. 
    // In a real parser, check(TokenType::TYPE_INT) || check(TokenType::TYPE_FLOAT)...
    // Let's patch to allow any type:
    if (previous().type != TokenType::TYPE_INT && previous().type != TokenType::TYPE_FLOAT && previous().type != TokenType::TYPE_BOOL) {
         // If consume failed, we already threw, but for "OR" logic:
         // This is a simplified check.
    }
    
    // Actually, let's look at the previous token if we matched one, but match() advances.
    // Let's redo:
    // func int add ...
    // 'func' is consumed. Next should be TYPE.
    
    // Correct logic for type parsing:
    Token typeToken = peek();
    if (match({TokenType::TYPE_INT, TokenType::TYPE_FLOAT, TokenType::TYPE_BOOL})) {
        typeToken = previous();
    } else {
        throw std::runtime_error("Expected return type.");
    }

    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LPAREN, "Expect '(' after " + kind + " name.");
    
    std::vector<Token> parameters;
    std::vector<Token> paramTypes;

    if (!check(TokenType::RPAREN)) {
        do {
            if (parameters.size() >= 255) {
                ErrorHandler::error(peek().line, "Can't have more than 255 parameters.");
            }
            
            // Parse Param Type
            if (match({TokenType::TYPE_INT, TokenType::TYPE_FLOAT, TokenType::TYPE_BOOL})) {
                paramTypes.push_back(previous());
            } else {
                 throw std::runtime_error("Expect parameter type.");
            }

            Token param = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            parameters.push_back(param);
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, "Expect ')' after parameters.");
    consume(TokenType::LBRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::unique_ptr<Statement>> body = block();
    return new FunctionStmt(name, typeToken, parameters, paramTypes, std::move(body));
}

Statement* Parser::varDeclaration() {
    // var int x = 10;
    Token type = peek();
    if (!match({TokenType::TYPE_INT, TokenType::TYPE_FLOAT, TokenType::TYPE_BOOL})) {
         throw std::runtime_error("Expect variable type after 'var'.");
    }
    type = previous();

    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    
    Expression* initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return new VarStmt(name, type, std::unique_ptr<Expression>(initializer));
}

Statement* Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::PRINT})) return printStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::LBRACE})) return new BlockStmt(block());
    return expressionStatement();
}

Statement* Parser::ifStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'if'.");
    Expression* condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after if condition.");

    Statement* thenBranch = statement();
    Statement* elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }
    return new IfStmt(std::unique_ptr<Expression>(condition), std::unique_ptr<Statement>(thenBranch), std::unique_ptr<Statement>(elseBranch));
}

Statement* Parser::whileStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'while'.");
    Expression* condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after condition.");
    Statement* body = statement();
    return new WhileStmt(std::unique_ptr<Expression>(condition), std::unique_ptr<Statement>(body));
}

Statement* Parser::printStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'print'.");
    Expression* value = expression();
    consume(TokenType::RPAREN, "Expect ')' after value.");
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return new PrintStmt(std::unique_ptr<Expression>(value));
}

Statement* Parser::returnStatement() {
    Token keyword = previous();
    Expression* value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return new ReturnStmt(keyword, std::unique_ptr<Expression>(value));
}

std::vector<std::unique_ptr<Statement>> Parser::block() {
    std::vector<std::unique_ptr<Statement>> statements;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        Statement* stmt = declaration();
        if(stmt) statements.emplace_back(stmt);
    }
    consume(TokenType::RBRACE, "Expect '}' after block.");
    return statements;
}

Statement* Parser::expressionStatement() {
    Expression* expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return new ExpressionStmt(std::unique_ptr<Expression>(expr));
}

Expression* Parser::expression() { return assignment(); }

Expression* Parser::assignment() {
    Expression* expr = orExpr();
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        Expression* value = assignment(); // Recursive for right-associativity
        
        // Check if left side is a valid assignment target (variable)
        VariableExpr* varExpr = dynamic_cast<VariableExpr*>(expr);
        if (varExpr != nullptr) {
            Token name = varExpr->name;
            // We release the pointer from expr because AssignExpr takes ownership
            // Note: In strict C++, we should manage memory better, but this is a simplified flow.
            // Since `expr` was allocated, we convert it to AssignExpr. 
            // Better to delete expr (since it's just a variable access wrapper) and create new node.
            delete expr; 
            return new AssignExpr(name, std::unique_ptr<Expression>(value));
        }
        ErrorHandler::error(equals.line, "Invalid assignment target.");
    }
    return expr;
}

Expression* Parser::orExpr() {
    Expression* expr = andExpr();
    while (match({TokenType::OR})) {
        Token op = previous();
        Expression* right = andExpr();
        expr = new BinaryExpr(std::unique_ptr<Expression>(expr), op, std::unique_ptr<Expression>(right));
    }
    return expr;
}

Expression* Parser::andExpr() {
    Expression* expr = equality();
    while (match({TokenType::AND})) {
        Token op = previous();
        Expression* right = equality();
        expr = new BinaryExpr(std::unique_ptr<Expression>(expr), op, std::unique_ptr<Expression>(right));
    }
    return expr;
}

Expression* Parser::equality() {
    Expression* expr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        Expression* right = comparison();
        expr = new BinaryExpr(std::unique_ptr<Expression>(expr), op, std::unique_ptr<Expression>(right));
    }
    return expr;
}

Expression* Parser::comparison() {
    Expression* expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        Expression* right = term();
        expr = new BinaryExpr(std::unique_ptr<Expression>(expr), op, std::unique_ptr<Expression>(right));
    }
    return expr;
}

Expression* Parser::term() {
    Expression* expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        Expression* right = factor();
        expr = new BinaryExpr(std::unique_ptr<Expression>(expr), op, std::unique_ptr<Expression>(right));
    }
    return expr;
}

Expression* Parser::factor() {
    Expression* expr = unary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        Expression* right = unary();
        expr = new BinaryExpr(std::unique_ptr<Expression>(expr), op, std::unique_ptr<Expression>(right));
    }
    return expr;
}

Expression* Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token op = previous();
        Expression* right = unary();
        return new UnaryExpr(op, std::unique_ptr<Expression>(right));
    }
    return call();
}

Expression* Parser::call() {
    Expression* expr = primary();
    while (true) {
        if (match({TokenType::LPAREN})) {
            expr = finishCall(expr);
        } else {
            break;
        }
    }
    return expr;
}

Expression* Parser::finishCall(Expression* callee) {
    std::vector<std::unique_ptr<Expression>> arguments;
    if (!check(TokenType::RPAREN)) {
        do {
            if (arguments.size() >= 255) {
                ErrorHandler::error(peek().line, "Can't have more than 255 arguments.");
            }
            arguments.push_back(std::unique_ptr<Expression>(expression()));
        } while (match({TokenType::COMMA}));
    }
    Token paren = consume(TokenType::RPAREN, "Expect ')' after arguments.");
    return new CallExpr(std::unique_ptr<Expression>(callee), paren, std::move(arguments));
}

Expression* Parser::primary() {
    if (match({TokenType::FALSE_KEYWORD})) return new LiteralExpr("false", TokenType::TYPE_BOOL);
    if (match({TokenType::TRUE_KEYWORD})) return new LiteralExpr("true", TokenType::TYPE_BOOL);
    if (match({TokenType::INT_LITERAL})) return new LiteralExpr(previous().literal, TokenType::TYPE_INT);
    if (match({TokenType::FLOAT_LITERAL})) return new LiteralExpr(previous().literal, TokenType::TYPE_FLOAT);
    if (match({TokenType::IDENTIFIER})) return new VariableExpr(previous());
    if (match({TokenType::LPAREN})) {
        Expression* expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return new GroupingExpr(std::unique_ptr<Expression>(expr));
    }
    throw std::runtime_error("Expect expression.");
}

// Helpers
bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
Token Parser::peek() { return tokens[current]; }
Token Parser::previous() { return tokens[current - 1]; }

Token Parser::consume(TokenType type, std::string message) {
    if (check(type)) return advance();
    ErrorHandler::error(peek().line, message);
    throw std::runtime_error(message);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUNC:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default: break;
        }
        advance();
    }
}