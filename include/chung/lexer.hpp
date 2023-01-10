#pragma once

#include <vector>

#include "chung/token.hpp"

class Lexer {
public:
    Lexer(const std::u32string& source);

    inline char32_t advance();
    inline char32_t peek();

    std::vector<Token> lex();

private:
    const std::u32string source;
    size_t cursor;
};