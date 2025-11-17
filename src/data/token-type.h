#pragma once

enum class TokenType {
    // Single-character tokens
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON,
    PLUS, MINUS, STAR, SLASH,
    EQUAL, LESS, GREATER,

    // Two-character tokens
    EQUAL_EQUAL, BANG_EQUAL, LESS_EQUAL, GREATER_EQUAL,

    // Literals
    IDENTIFIER, INT_LITERAL, FLOAT_LITERAL, BOOL_LITERAL,

    // Keywords
    VAR, INT, FLOAT, BOOL,
    IF, ELSE, WHILE,
    FUNC, RETURN, PRINT,
    TRUE_KEYWORD, FALSE_KEYWORD,

    END_OF_FILE
};