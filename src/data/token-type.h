#pragma once

enum class TokenType {
    // Single-character tokens
    LPAREN, RPAREN, LBRACE, RBRACE, COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, INT_LITERAL, FLOAT_LITERAL,

    // Keywords
    AND, ELSE, FALSE_KEYWORD, FUNC, IF, OR, PRINT, RETURN, TRUE_KEYWORD, VAR, WHILE,
    TYPE_INT, TYPE_FLOAT, TYPE_BOOL, // for 'int', 'float', 'bool'

    FOR, CLASS,
    END_OF_FILE
};