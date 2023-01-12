#include "chung/lexer.hpp"
#include "chung/parser.hpp"
#include "chung/utf.hpp"

int main() {
    // std::u32string code{U"6 * (4.0 + 8) - (69420 * (3 + (1 + 1)))"};
    // std::u32string code{U"let e = 64 + 133 - 2 * 3\nlet sus = 43408637547754347534753737337454774353573454754375"};
    std::u32string code{U"let sus = 1 + (2 + 3;\nlet sussy = 6940603203967203760730760376033726023760;"};
    Lexer lexer{code};

    auto [tokens, exceptions] = lexer.lex();

    std::cout << "\nLexer passes\n\n";
    std::cout << "\033[1;31m";
    for (auto& exception: exceptions) {
        std::cout << exception.write() << '\n';
    }
    std::cout << "\033[0m\n";

    Parser parser{tokens, lexer.source_lines};
    auto statements = parser.parse();
    std::cout << "Parser passes\n\n";

    std::cout << "\033[1;31m";
    for (auto& exception: parser.exceptions) {
        std::cout << exception.write() << '\n';
    }
    std::cout << "\033[0m";

    std::cout << "\nYour code: " + u32tostring(code) + '\n';

    for (auto& statement: statements) {
        std::cout << statement->stringify() << '\n';
    }
}