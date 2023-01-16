#pragma once

#include <vector>

#include "chung/token.hpp"
#include "chung/error.hpp"

class LexException: public Exception {
public:
    std::string exception_message;

    size_t start;
    size_t end;
    size_t line;
    size_t column;

    std::string source_line;

    LexException(const std::string &exception_message, size_t start, size_t end);
    std::string write();
};

class Lexer {
public:
    Lexer(const std::u32string& source);

    inline char32_t advance() {
        return source[cursor++];
    }

    inline char32_t peek() {
        return source[cursor];
    }

    inline std::vector<std::u32string> get_source_lines() {
        return source_lines;
    }

    std::pair<std::vector<Token>, std::vector<LexException>> lex();
private:
    const std::u32string source;
    std::vector<std::u32string> source_lines;
    size_t cursor;
};