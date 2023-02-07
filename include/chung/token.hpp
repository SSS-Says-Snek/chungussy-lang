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

    ARROW,

    DOT, COMMA, COLON, SEMICOLON
};

enum class Keyword {
    DEF, LET, __OMG
};

enum class TokenType {
    EOF, INVALID,

    IDENTIFIER,
    OPERATOR,
    SYMBOL,
    KEYWORD,

    // Primitives
    UINT64,
    INT64,
    FLOAT64,
    STRING
};

struct TokenVal {
    union {
        Operator op;
        Symbol symbol;
        Keyword keyword;
        uint64_t uint64;
        int64_t int64;
        double float64;
    };

    // L unions (I'm lazy to write a tagged one)
    std::string identifier;
    std::u32string string;
};

struct Token {
    TokenType type;
    TokenVal value;
    size_t beg;
    size_t end;

    size_t line_beg;
    size_t line_end;

    size_t line;
    size_t column;

    Token(TokenType type, size_t beg, size_t end):
        type{type}, beg{beg}, end{end}, line{0}, column{0} {}
    Token(TokenType type, TokenVal value, size_t beg, size_t end):
        type{type}, value{value}, beg{beg}, end{end}, line{0}, column{0} {}
};

bool is_keyword(const std::string& identifier);