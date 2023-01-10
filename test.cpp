#include "chung/lexer.hpp"
#include "chung/parser.hpp"

int main() {
    std::u32string code{U"6 * (4u + 8) - 7"};
    Lexer lexer{code};

    auto tokens = lexer.lex();
    std::cout << "Lexer passes\n";
    Parser parser{tokens};

    auto expr = parser.parse_expression();
    std::cout << "Parser passes!!!!\n";

    std::cout << "Your expression: 6 * (4u + 8) - 7\n";
    std::cout << expr->stringify();
}