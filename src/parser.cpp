#include <memory>
#include <vector>
#include <unordered_map>

#include "chung/parser.hpp"
#include "chung/stringify.hpp"

int get_op_precedence(Operator op) {
    static const std::unordered_map<Operator, int> op_lookup {
        {Operator::ADD, 1}, {Operator::SUB, 1},
        {Operator::MUL, 2}, {Operator::DIV, 2},
        {Operator::POW, 3}
    };

    auto result = op_lookup.find(op);
    if (result == op_lookup.end()) {
        return -1;
    }
    return result->second;
}

Parser::Parser(const std::vector<Token> tokens): tokens{std::move(tokens)}, tokens_idx{0} {}
    
Token Parser::current_token() {
    return tokens[tokens_idx];
}

Token Parser::eat_token() {
    return tokens[tokens_idx++];
}

std::unique_ptr<ExprAST> Parser::parse_identifier() {
    Token token = eat_token();

    if (token.value.symbol_type != Symbol::OPEN_PARENTHESES) {
        return std::make_unique<VariableAST>(token.value.identifier);
    }
}

std::unique_ptr<ExprAST> Parser::parse_parentheses() {
    // Eat '('
    eat_token();
    std::unique_ptr<ExprAST> expr = parse_expression();

    // Eat ')' if it's there, otherwise do something >:(
    std::cout << stringify(current_token().type) << current_token().value.uint64;
    if (current_token().value.symbol_type != Symbol::CLOSE_PARENTHESES) {
        std::cout << "L";
    }
    eat_token();
    return expr;
}

std::unique_ptr<ExprAST> Parser::parse_bin_op(int min_op_precedence, std::unique_ptr<ExprAST> lhs) {
    while (true) {
        Token op = current_token();
        int op_precedence = get_op_precedence(op.value.operator_type);

        if (op_precedence < min_op_precedence || op.type != TokenType::OPERATOR) {
            return lhs;
        }
        std::cout << "Binary op: " << stringify(op.value.operator_type) << '\n';

        eat_token();
        std::unique_ptr<ExprAST> rhs = parse_primary();

        int next_op_precedence = get_op_precedence(current_token().value.operator_type);
        if (op_precedence < next_op_precedence) {
            rhs = parse_bin_op(min_op_precedence + 1, std::move(rhs));
        }

        lhs = std::make_unique<BinaryExprAST>(op.value.operator_type, std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<ExprAST> Parser::parse_literal() {
    Token token = eat_token();

    switch (token.type) {
        case TokenType::INT64:
            return std::make_unique<LiteralAST>(token.value.int64);
        case TokenType::UINT64:
            return std::make_unique<LiteralAST>(token.value.uint64);
        case TokenType::FLOAT64:
            return std::make_unique<LiteralAST>(token.value.float64);
    }
}

std::unique_ptr<ExprAST> Parser::parse_primary() {
    Token token = current_token();
    switch (token.type) {
        case TokenType::IDENTIFIER:
            return parse_identifier();
        case TokenType::SYMBOL:
            if (token.value.symbol_type == Symbol::OPEN_PARENTHESES) {
                return parse_parentheses();
            }
            break;
        default:
            std::cout << "Literal\n";
            return parse_literal();
    }
}

std::unique_ptr<ExprAST> Parser::parse_expression() {
    std::unique_ptr<ExprAST> lhs = parse_primary();

    return parse_bin_op(0, std::move(lhs));
}