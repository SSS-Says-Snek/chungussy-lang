#include "chung/lexer.hpp"
#include "chung/parser.hpp"
#include "chung/stringify.hpp"

int main() {
    std::u32string code{U"6 * (4.0 + 8) - (69420 * (3 + (1 + 1)))"};
    Lexer lexer{code};

    auto tokens = lexer.lex();
    for (auto& token: tokens) {
        std::cout << '|' << stringify(token.type) << '|';
    }

    std::cout << "Lexer passes\n";
    Parser parser{tokens};

    auto expr = parser.parse_expression();
    std::cout << "Parser passes!!!!\n";

    std::cout << "Your expression: " + stringify(code) + '\n';
    std::cout << expr->stringify();
}