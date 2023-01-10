#pragma once

#include "chung/ast.hpp"

class Parser {
public:
    Parser(const std::vector<Token> tokens);

    Token current_token();
    Token eat_token();

    std::unique_ptr<ExprAST> parse_identifier();
    std::unique_ptr<ExprAST> parse_parentheses();
    std::unique_ptr<ExprAST> parse_bin_op(int min_op_precedence, std::unique_ptr<ExprAST> lhs);
    std::unique_ptr<ExprAST> parse_literal();
    std::unique_ptr<ExprAST> parse_primary();
    std::unique_ptr<ExprAST> parse_expression();
private:
    std::vector<Token> tokens;
    size_t tokens_idx;
};