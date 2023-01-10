#include <string>
#include <cwctype>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>

#include "chung/lexer.hpp"

#define HANDLE_OP(op, op_name)                                                   \
    case op_name:                                                                \
        value.operator_type = op;                                                \
        tokens.push_back(Token{TokenType::OPERATOR, value, cursor, cursor + 1}); \
        advance();                                                               \
        break;                                                                   \

#define HANDLE_SYMBOL(symbol, symbol_name)                                     \
    case symbol_name:                                                          \
        value.symbol_type = symbol;                                            \
        tokens.push_back(Token{TokenType::SYMBOL, value, cursor, cursor + 1}); \
        advance();                                                             \
        break;                                                                 \


std::string u32tostring(const std::u32string& s) {
    std::string out;
    std::transform(begin(s), end(s), back_inserter(out), [](char32_t c) {
        return c < 128 ? static_cast<char>(c) : '?';
    });
    return out;
}

bool is_identifier_char(char32_t c) {
    if (std::iswalpha(c) || c == '_') {
        return true;
    }
    return false;
}

Lexer::Lexer(const std::u32string& source): source{source}, cursor{0} {}

inline char32_t Lexer::advance() {
    return source[cursor++];
}

inline char32_t Lexer::peek() {
    return source[cursor];
}

std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens;
    
    while (true) {
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
                case 'U' | 'u':
                    try {
                        value.uint64 = std::stoull(token_string);
                        type = TokenType::UINT64;
                        advance();
                    } catch (...) {
                        // L
                        std::cout << "L";
                    }
                    break;
                default:
                    try {
                        value.int64 = std::stoll(token_string);
                        type = TokenType::INT64;
                    } catch (...) {
                        // L
                        std::cout << "L";
                    }
                    break;
            }

            tokens.push_back(Token{type, value, start, cursor});
        } else {
            switch (peek()) {
                case '/':
                    if (advance() == '/') {
                        //std::cout << "OMG it's a comment";
                        advance();
                        while (peek() != '\0') {
                            if (advance() == '\n') {
                                break;
                            }
                        }
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
            }
        }
    }
    return tokens;
}

// int main() {
//     // const std::u32string code =
//     //     U"def e(amogus: string) {\n"
//     //     U"    e: int64 = (1 + 2) * 69;\n"
//     //     U"    print(e, amogus);\n"
//     //     U"}";
//     const std::u32string code =
//         U"e: int64 = 4;";

//     Lexer lexer{code};
    
//     std::vector<Token> tokens = lexer.lex();
//     for (auto& token: tokens) {
//         std::cout << token << ' ' << u32tostring(code.substr(token.beg, token.end - token.beg)) << '\n';
//     }
// }