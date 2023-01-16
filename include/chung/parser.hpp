#pragma once

#include "chung/ast.hpp"
#include "chung/error.hpp"
#include "chung/utf.hpp"

#define VALIDATE_TOKEN(token_, type, condition)         \
    if (current_token().type == TokenType::EOF || ) {       \
        return false;                                   \
    }                                                   \
    return current_token().type == type && conditional; \

class ParseException: public Exception {
public:
    std::string exception_message;
    Token token;

    std::string source_line;

    ParseException(const std::string& exception_message, const Token& token, const std::string& source_line);
    std::string write();
};

class Parser {
public:
    Parser(const std::vector<Token> tokens, const std::vector<std::u32string> source_lines);

    inline Token current_token() {
        if (tokens_idx >= tokens.size()) {
            return tokens[tokens.size() - 1];
        }
        return tokens[tokens_idx];
    }

    inline Token previous_token() {
        if (tokens_idx == 0) {
            return tokens[0];
        }
        return tokens[tokens_idx - 1];
    }

    inline Token next_token() {
        if (tokens_idx >= tokens.size()) {
            return tokens[tokens.size() - 1];
        }
        return tokens[tokens_idx + 1];
    }

    inline Token eat_token() {
        if (tokens_idx >= tokens.size()) {
            return tokens[tokens.size() - 1];
        }
        return tokens[tokens_idx++];
    }

    inline ParseException push_exception(const std::string& exception_message, const Token& token) {
        ParseException exception{exception_message, token, u32tostring(source_lines[token.line - 1])};
        exceptions.push_back(exception);
        return exception;
    }

    inline std::vector<ParseException> get_exceptions() {
        return exceptions;
    }

    void synchronize();

    std::shared_ptr<ExprAST> parse_identifier();
    std::shared_ptr<ExprAST> parse_parentheses();
    std::shared_ptr<ExprAST> parse_bin_op(int min_op_precedence, std::shared_ptr<ExprAST> lhs);
    std::shared_ptr<ExprAST> parse_primitive();
    std::shared_ptr<ExprAST> parse_primary();
    
    // Statements
    std::shared_ptr<StmtAST> parse_var_declaration();
    std::shared_ptr<StmtAST> parse_omg();
    std::shared_ptr<StmtAST> parse_expression_statement();
    
    // Heheheha
    std::shared_ptr<ExprAST> parse_expression();
    std::shared_ptr<StmtAST> parse_statement();

    // For now
    std::vector<std::shared_ptr<StmtAST>> parse();

private:
    std::vector<Token> tokens;
    std::vector<std::u32string> source_lines;
    std::vector<ParseException> exceptions;
    size_t tokens_idx;
};