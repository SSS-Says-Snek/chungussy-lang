#pragma once

#include <iostream>

#undef EOF

enum class Operator {
    ADD, SUB, MUL, DIV, MOD, POW,
    BITWISE_AND, BITWISE_OR, BITWISE_NOT,

    ASSIGN
};

enum class Symbol {
    OPEN_PARENTHESES, CLOSE_PARENTHESES,
    OPEN_BRACKETS, CLOSE_BRACKETS,
    OPEN_BRACES, CLOSE_BRACES,

    DOT, COMMA, COLON, SEMICOLON
};

enum class TokenType {
    EOF, INVALID,

    DEF, LET,
    IDENTIFIER,
    OPERATOR,
    SYMBOL,

    UINT64,
    INT64,
    FLOAT64
};

struct TokenVal {
    union {
        Operator operator_type;
        Symbol symbol_type;
        uint64_t uint64;
        int64_t int64;
        double float64;
    };

    // L unions (I'm lazy to write a tagged one)
    std::string identifier;
};

struct Token {
    TokenType type;
    TokenVal value;
    size_t beg;
    size_t end;

    Token(TokenType type, size_t beg, size_t end):
        type{type}, beg{beg}, end{end} {}
    Token(TokenType type, TokenVal value, size_t beg, size_t end):
        type{type}, value{value}, beg{beg}, end{end} {}
};

