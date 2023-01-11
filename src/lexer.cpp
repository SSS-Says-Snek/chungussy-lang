#include <string>
#include <cwctype>
#include <iostream>
#include <vector>

#include "chung/lexer.hpp"
#include "chung/stringify.hpp"

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

            value.identifier = stringify(source.substr(start, cursor - start));
            TokenType type;

            if (value.identifier == "def") {
                type = TokenType::DEF;
            } else if (value.identifier == "let") {
                type = TokenType::LET;
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
            std::string token_string = stringify(source.substr(start, cursor - start));
            TokenType type;

            switch (suffix) {
                case 'U' | 'u': // Unsigned
                    try {
                        value.uint64 = std::stoull(token_string);
                        type = TokenType::UINT64;
                        advance();
                    } catch (...) {
                        // L
                        std::cout << "L";
                    }
                    break;

                case '.': { // Floating point
                    size_t decimal_start = cursor;
                    advance();
                    while (std::iswdigit(peek())) {
                        advance();
                    }
                    std::string float_string = token_string + stringify(source.substr(decimal_start, cursor - decimal_start));

                    try {
                        value.float64 = std::stod(float_string);
                        type = TokenType::FLOAT64;
                    } catch (...) {
                        // L
                        std::cout << "L";
                    }
                    break;
                }

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
                    if (advance() == '/') { // Comment
                        advance();
                        while (peek() != '\0') {
                            if (advance() == '\n') {
                                break;
                            }
                        }
                    } else { // Division
                        value.operator_type = Operator::DIV;
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