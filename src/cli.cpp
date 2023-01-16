#include <tuple>

#include "chung/file.hpp"
#include "chung/lexer.hpp"
#include "chung/parser.hpp"

#define CHUNG_VER_MAJOR 0
#define CHUNG_VER_MINOR 0
#define CHUNG_VER_PATCH 1

#define ANSI_RED "\033[1;31m"
#define ANSI_GREEN "\033[1;32m"
#define ANSI_RESET "\033[0m"

inline std::string chung_ver_string() {
    return std::to_string(CHUNG_VER_MAJOR) + '.' + std::to_string(CHUNG_VER_MINOR) + '.' + std::to_string(CHUNG_VER_PATCH);
}

void run_help() {
    std::cout << "Usage: chung [options]\n";
    std::cout << "IDK something something\n";  
}

void run_parse(std::vector<std::string>& args) {
    std::cout << "Running Chungussy " << chung_ver_string() << '\n';
    if (args.size() != 2) {
        std::cerr << ANSI_RED << "Expected 1 argument, received " << args.size() - 1 << '\n' << ANSI_RESET;
        std::exit(1);
    }
    
    std::string file_path = args[1];
    if (!file_exists(file_path)) {
        std::cerr << ANSI_RED << "File not found: \"" << file_path << "\" cannot be located" << '\n' << ANSI_RESET;
        std::exit(1);
    }
    std::cout << "Parsing " << file_path << '\n';

    std::u32string source = read_source(file_path);
    Lexer lexer{source};

    std::vector<Token> tokens;
    std::vector<LexException> lex_exceptions;
    std::tie(tokens, lex_exceptions) = lexer.lex();

    if (!lex_exceptions.empty()) {
        std::cout << ANSI_RED;
        for (auto& lex_exception: lex_exceptions) {
            std::cout << lex_exception.write() << '\n';
        }
        std::cout << ANSI_RESET;
    } else {
        std::cout << ANSI_GREEN << "Successfully lexed with no exceptions!\n" << ANSI_RESET;
    }

    Parser parser{tokens, lexer.get_source_lines()};
    auto statements = parser.parse();
    auto parse_exceptions = parser.get_exceptions();

    if (!parse_exceptions.empty()) {
        std::cout << ANSI_RED;
        for (auto& parse_exception: parse_exceptions) {
            std::cout << parse_exception.write() << '\n';
        }
        std::cout << ANSI_RESET;
    } else {
        std::cout << ANSI_GREEN << "Successfully parsed with no exceptions!\n\n" << ANSI_RESET;
    }

    if (!statements.empty()) {
        std::cout << "Program AST\n";
        for (auto& statement: statements) {
            std::cout << statement->stringify() << '\n';
        }
    }
}

int main(const int argc, const char** argv) {
    std::vector<std::string> args;
    // Goofy ahh first argument
    for (int i = 1; i < argc; i++) {
        args.push_back(argv[i]);
    }

    std::string command = args[0];
    if (command == "help" || args.empty()) {
        run_help();
    } else if (command == "parse") {
        run_parse(args);
    }
}