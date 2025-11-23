#include "scanner.h"
#include "../util/error-handler.h"

std::map<std::string, TokenType> Scanner::keywords = {
    {"and", TokenType::AND},
    {"else", TokenType::ELSE},
    {"false", TokenType::FALSE_KEYWORD},
    {"func", TokenType::FUNC},
    {"if", TokenType::IF},
    {"or", TokenType::OR},
    {"print", TokenType::PRINT},
    {"return", TokenType::RETURN},
    {"true", TokenType::TRUE_KEYWORD},
    {"var", TokenType::VAR},
    {"while", TokenType::WHILE},
    {"int", TokenType::TYPE_INT},
    {"float", TokenType::TYPE_FLOAT},
    {"bool", TokenType::TYPE_BOOL}
};

Scanner::Scanner(std::string source) : source(source) {}

std::vector<Token> Scanner::scanTokens() {
    while (!isAtEnd()) {
        start = current;
        char c = advance();
        switch (c) {
            case '(': addToken(TokenType::LPAREN); break;
            case ')': addToken(TokenType::RPAREN); break;
            case '{': addToken(TokenType::LBRACE); break;
            case '}': addToken(TokenType::RBRACE); break;
            case ',': addToken(TokenType::COMMA); break;
            case '.': addToken(TokenType::DOT); break;
            case '-': addToken(TokenType::MINUS); break;
            case '+': addToken(TokenType::PLUS); break;
            case ';': addToken(TokenType::SEMICOLON); break;
            case '*': addToken(TokenType::STAR); break;
            case '!': addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); break;
            case '=': addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); break;
            case '<': addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS); break;
            case '>': addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER); break;
            case '/':
                if (match('/')) {
                    // A comment goes until the end of the line.
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else {
                    addToken(TokenType::SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                // Ignore whitespace.
                break;
            case '\n':
                line++;
                break;
            case '"': string(); break;
            default:
                if (isdigit(c)) {
                    number();
                } else if (isalpha(c) || c == '_') {
                    identifier();
                } else {
                    ErrorHandler::error(line, "Unexpected character.");
                }
                break;
        }
    }
    tokens.emplace_back(TokenType::END_OF_FILE, "", "", line);
    return tokens;
}

bool Scanner::isAtEnd() { return current >= source.length(); }
char Scanner::advance() { return source[current++]; }

void Scanner::addToken(TokenType type) { addToken(type, ""); }
void Scanner::addToken(TokenType type, std::string literal) {
    std::string text = source.substr(start, current - start);
    tokens.emplace_back(type, text, literal, line);
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (source[current] != expected) return false;
    current++;
    return true;
}

char Scanner::peek() { return isAtEnd() ? '\0' : source[current]; }
char Scanner::peekNext() { return (current + 1 >= source.length()) ? '\0' : source[current + 1]; }

void Scanner::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }
    if (isAtEnd()) {
        ErrorHandler::error(line, "Unterminated string.");
        return;
    }
    advance(); // The closing ".
    std::string value = source.substr(start + 1, current - start - 2);
    addToken(TokenType::STRING, value);
}

void Scanner::number() {
    while (isdigit(peek())) advance();
    bool isFloat = false;
    if (peek() == '.' && isdigit(peekNext())) {
        isFloat = true;
        advance(); // Consume the "."
        while (isdigit(peek())) advance();
    }
    addToken(isFloat ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL, 
             source.substr(start, current - start));
}

void Scanner::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    TokenType type = TokenType::IDENTIFIER;
    if (keywords.find(text) != keywords.end()) {
        type = keywords[text];
    }
    addToken(type);
}