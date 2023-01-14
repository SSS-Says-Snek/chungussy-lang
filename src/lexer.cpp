#include <string>
#include <cwctype>
#include <iostream>
#include <sstream>
#include <vector>

#include "chung/lexer.hpp"
#include "chung/utf.hpp"

#define HANDLE_OP(op_, op_name)                                                   \
    case op_name:                                                                 \
        value.op = op_;                                                           \
        tokens.push_back(Token{TokenType::OPERATOR, value, cursor, cursor + 1});  \
        advance();                                                                \
        break;                                                                    \

#define HANDLE_SYMBOL(symbol_, symbol_name)                                     \
    case symbol_name:                                                           \
        value.symbol = symbol_;                                                 \
        tokens.push_back(Token{TokenType::SYMBOL, value, cursor, cursor + 1});  \
        advance();                                                              \
        break;                                                                  \

inline bool is_identifier_char(char32_t c) {
    if (std::iswalpha(c) || c == '_') {
        return true;
    }
    return false;
}

LexException::LexException(const std::string& exception_message, size_t start, size_t end):
    exception_message{exception_message}, start{start}, end{end} {}

std::string LexException::write() {
    std::string string{"LexException at line " + std::to_string(line) + " column " + std::to_string(column) + ":\n"};
    string += '\t' + source_line + '\n';
    string += exception_message + '\n';
    
    return string;
}

Lexer::Lexer(const std::u32string& source): source{source}, source_lines{}, cursor{0} {
    size_t start = 0;
    size_t end = 0;

    while ((end = source.find('\n', start)) != std::string::npos) {
        source_lines.push_back(source.substr(start, end - start));
        start = end + 1;
    }
    source_lines.push_back(source.substr(start));
}

std::pair<std::vector<Token>, std::vector<LexException>> Lexer::lex() {
    std::vector<Token> tokens;
    std::vector<LexException> exceptions;
    
    while (true) {
        try { 
            // Skips whitespace
            while (std::iswspace(peek()) || peek() == '\n') {
                advance();
            }

            TokenVal value;

            if (peek() == '\0') {
                tokens.push_back(Token{TokenType::EOF, cursor, cursor + 1});
                break;
            } else if (is_identifier_char(peek())) {
                size_t start = cursor;
                advance();
                while (is_identifier_char(peek())) {
                    advance();
                }

                value.identifier = u32tostring(source.substr(start, cursor - start));
                TokenType type;

                if (value.identifier == "def") {
                    type = TokenType::DEF;
                } else if (value.identifier == "let") {
                    type = TokenType::LET;
                } else if (value.identifier == "__omg") {
                    type = TokenType::__OMG;
                } else {
                    type = TokenType::IDENTIFIER;
                }
                tokens.push_back({type, value, start, cursor});
            } else if (std::iswdigit(peek())) {
                size_t start = cursor;
                advance();
                while (std::iswdigit(peek())) {
                    advance();
                }

                char32_t suffix = peek();
                std::string token_string = u32tostring(source.substr(start, cursor - start));
                TokenType type;

                switch (suffix) {
                    case 'U' | 'u': // Unsigned
                        try {
                            value.uint64 = std::stoull(token_string);
                            type = TokenType::UINT64;
                            advance();
                        } catch (...) {
                            // L
                            tokens.push_back(Token{TokenType::INVALID, start, cursor});
                            throw LexException{"Value " + token_string + " too large to store in an uint64", start, cursor};
                        }
                        break;

                    case '.': { // Floating point
                        size_t decimal_start = cursor;
                        advance();
                        while (std::iswdigit(peek())) {
                            advance();
                        }
                        std::string float_string = token_string + u32tostring(source.substr(decimal_start, cursor - decimal_start));

                        try {
                            value.float64 = std::stod(float_string);
                            type = TokenType::FLOAT64;
                        } catch (...) {
                            tokens.push_back(Token{TokenType::INVALID, start, cursor});
                            throw LexException{"Value " + float_string + " too large to store in an float64", start, cursor};
                        }
                        break;
                    }

                    default:
                        try {
                            value.int64 = std::stoll(token_string);
                            type = TokenType::INT64;
                        } catch (...) {
                            // L
                            tokens.push_back(Token{TokenType::INVALID, start, cursor});
                            throw LexException{"Value " + token_string + " too large to store in an int64", start, cursor};
                        }
                        break;
                }

                tokens.push_back(Token{type, value, start, cursor});
            } else {
                switch (peek()) {
                    case '/':
                        if (advance() == '/') { // Comment
                            advance();
                            while (peek() != '\0') {
                                if (advance() == '\n') {
                                    break;
                                }
                            }
                        } else { // Division
                            value.op = Operator::DIV;
                            tokens.push_back(Token{TokenType::OPERATOR, value, cursor, cursor + 1});
                            advance();
                        }
                        break;

                    HANDLE_OP(Operator::ADD, '+')
                    HANDLE_OP(Operator::SUB, '-')
                    HANDLE_OP(Operator::MUL, '*')
                    HANDLE_OP(Operator::ASSIGN, '=')

                    HANDLE_SYMBOL(Symbol::OPEN_PARENTHESES, '(')
                    HANDLE_SYMBOL(Symbol::CLOSE_PARENTHESES, ')')
                    HANDLE_SYMBOL(Symbol::OPEN_BRACKETS, '[')
                    HANDLE_SYMBOL(Symbol::CLOSE_BRACKETS, ']')
                    HANDLE_SYMBOL(Symbol::OPEN_BRACES, '{')
                    HANDLE_SYMBOL(Symbol::CLOSE_BRACES, '}')

                    HANDLE_SYMBOL(Symbol::DOT, '.')
                    HANDLE_SYMBOL(Symbol::COMMA, ',')
                    HANDLE_SYMBOL(Symbol::COLON, ':')
                    HANDLE_SYMBOL(Symbol::SEMICOLON, ';')

                    default:
                        tokens.push_back(Token{TokenType::INVALID, value, cursor, cursor + 1});
                        advance();
                }
            }
        } catch (LexException& exception) {
            // Compute lines and columns based on index of src
            size_t line = 1;
            size_t column = 1;
            for (size_t i = 0; i < exception.end; i++) {
                if (source[i] == '\n') {
                    column = 0;
                    line++;
                }
                column++;
            }
            exception.source_line = u32tostring(source_lines[line - 1]);
            exception.line = line;
            exception.column = column;

            exceptions.push_back(exception);
        }
    }

    size_t line = 1;
    size_t column = 1;
    size_t token_idx = 0;
    size_t line_idx = 0;
    for (size_t i = 0; i <= source.length(); i++) {
        if (source[i] == '\n') {
            column = 0;
            line++;
            line_idx = i + 1;
        }
        
        if (tokens[token_idx].beg == i) {
            tokens[token_idx].line = line;
            tokens[token_idx].column = column - 1;

            tokens[token_idx].line_beg = i - line_idx;
            tokens[token_idx].line_end = tokens[token_idx].end - line_idx;
            // std::cout << stringify(tokens[token_idx].type) << " line: " << line << " column: " << column << '\n';
            token_idx++;
        }

        column++;
    }

    return std::make_pair(tokens, exceptions);
}
