#include <memory>
#include <vector>
#include <unordered_map>

#include "chung/parser.hpp"
#include "chung/stringify.hpp"

int get_op_precedence(Operator op) {
    static const std::unordered_map<Operator, int> op_lookup {
        {Operator::ADD, 1}, {Operator::SUB, 1},
        {Operator::MUL, 2}, {Operator::DIV, 2}, {Operator::MOD, 2},
        {Operator::POW, 3}
    };

    auto result = op_lookup.find(op);
    if (result == op_lookup.end()) {
        return -1;
    }
    return result->second;
}


ParseException::ParseException(const std::string& exception_message, const Token& token, const std::string& source_line):
    exception_message{exception_message}, token{token}, source_line{source_line} {}

std::string ParseException::write() {
    std::string string{"ParseException at line " + std::to_string(token.line) + " column " + std::to_string(token.column) + ":\n"};
    std::string carets;

    for (size_t i = 1; i <= source_line.length(); i++) {
        if (token.line_beg <= i && i < token.line_end) {
            carets += '^';
        } else {
            carets += '~';
        }
    }

    string += '\t' + source_line + '\n';
    string += '\t' + carets + '\n';
    string += exception_message + '\n';
    
    return string;
}


Parser::Parser(const std::vector<Token> tokens, const std::vector<std::u32string> source_lines):
    tokens{std::move(tokens)}, source_lines{std::move(source_lines)}, tokens_idx{0} {}

void Parser::synchronize() {
    eat_token();

    while (current_token().type != TokenType::EOF) {
        Token token = tokens[tokens_idx];
        if (token.type == TokenType::SYMBOL && token.value.symbol == Symbol::SEMICOLON) {
            return;
        }

        eat_token();
    }
}

std::shared_ptr<ExprAST> Parser::parse_identifier() {
    Token token = eat_token();

    if (token.value.symbol != Symbol::OPEN_PARENTHESES) {
        return std::make_shared<VariableAST>(token.value.identifier);
    }

    // // A call
    // // Eat '('
    // eat_token();

    // std::vector<std::unique_ptr<ExprAST>> arguments;
    // while (true) {

    // }
}

std::shared_ptr<ExprAST> Parser::parse_parentheses() {
    // Eat '('
    eat_token();
    std::shared_ptr<ExprAST> expr = parse_expression();

    // Eat ')' if it's there, otherwise push exception
    if (current_token().value.symbol != Symbol::CLOSE_PARENTHESES) {
        push_exception("Expected closing parenthesis ')'", current_token());
        synchronize();
    }
    eat_token();
    return expr;
}

std::shared_ptr<ExprAST> Parser::parse_bin_op(int min_op_precedence, std::shared_ptr<ExprAST> lhs) {
    while (true) {
        Token op = current_token();
        int op_precedence = get_op_precedence(op.value.op);

        if (op_precedence < min_op_precedence || op.type != TokenType::OPERATOR) {
            return lhs;
        }
        std::cout << "Binary op: " << stringify(op.value.op) << '\n';

        // Eat operator
        eat_token();
        std::shared_ptr<ExprAST> rhs = parse_primary();

        int next_op_precedence = get_op_precedence(current_token().value.op);
        if (op_precedence < next_op_precedence) {
            rhs = parse_bin_op(min_op_precedence + 1, std::move(rhs));
        }

        lhs = std::make_shared<BinaryExprAST>(op.value.op, std::move(lhs), std::move(rhs));
    }
}

std::shared_ptr<ExprAST> Parser::parse_primitive() {
    Token token = eat_token();

    switch (token.type) {
        case TokenType::INT64:
            return std::make_shared<PrimitiveAST>(token.value.int64);
        case TokenType::UINT64:
            return std::make_shared<PrimitiveAST>(token.value.uint64);
        case TokenType::FLOAT64:
            return std::make_shared<PrimitiveAST>(token.value.float64);
        default:
            // Invalid token
            push_exception("Invalid token in expression", token);
            synchronize();
            std::cout << token.beg << ' ' << token.end << '\n';
            return std::make_shared<PrimitiveAST>();
    }
}

std::shared_ptr<ExprAST> Parser::parse_primary() {
    Token token = current_token();
    switch (token.type) {
        case TokenType::IDENTIFIER:
            return parse_identifier();
        case TokenType::SYMBOL:
            if (token.value.symbol == Symbol::OPEN_PARENTHESES) {
                return parse_parentheses();
            }
            break;
        default:
            std::cout << "Primitive\n";
            return parse_primitive();
    }
}

std::shared_ptr<StmtAST> Parser::parse_var_declaration() {
    // Eat 'let'
    eat_token();
    // Eat identifier
    Token identifier = eat_token();
    if (identifier.type != TokenType::IDENTIFIER) {
        push_exception("Expected identifier to assign expression to", identifier);
        synchronize();
    }
    
    std::shared_ptr<ExprAST> expr = std::make_shared<PrimitiveAST>(nullptr);
    if (current_token().type == TokenType::OPERATOR && current_token().value.op == Operator::ASSIGN) {
        // Eat '='
        eat_token();

        expr = parse_expression();
    }

    Token token = eat_token();
    if (token.type != TokenType::SYMBOL || token.value.symbol != Symbol::SEMICOLON) {
        std::cout << stringify(token.type);
        push_exception("Expected ';' after expression", token);
    }

    return std::make_shared<VarDeclareAST>(identifier.value.identifier, std::move(expr));
}

std::shared_ptr<StmtAST> Parser::parse_omg() {
    // Eat '__omg'
    eat_token();

    std::shared_ptr<ExprAST> expr = parse_expression();

    Token token = eat_token();
    if (token.type != TokenType::SYMBOL || token.value.symbol != Symbol::SEMICOLON) {
        push_exception("Expected ';' after value", token);
    }

    return std::make_shared<OmgAST>(expr);
}

std::shared_ptr<StmtAST> Parser::parse_expression_statement() {
    std::shared_ptr<ExprAST> expr = parse_expression();

    Token token = eat_token();
    if (token.type != TokenType::SYMBOL || token.value.symbol != Symbol::SEMICOLON) {
        push_exception("Expected ';' after expression", token);
    }

    return std::make_shared<ExprStmtAST>(expr);
}

std::shared_ptr<ExprAST> Parser::parse_expression() {
    std::shared_ptr<ExprAST> lhs = parse_primary();

    return parse_bin_op(0, std::move(lhs));
}

std::shared_ptr<StmtAST> Parser::parse_statement() {
    switch (current_token().type) {
        case TokenType::__OMG:
            return parse_omg();
        case TokenType::LET:
            return parse_var_declaration();
        
        default:
            return parse_expression_statement();
    }
}

std::vector<std::shared_ptr<StmtAST>> Parser::parse() {
    std::vector<std::shared_ptr<StmtAST>> statements;

    while (current_token().type != TokenType::EOF) {
        statements.push_back(parse_statement());
    }

    return statements;
}