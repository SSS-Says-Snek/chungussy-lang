#include <memory>
#include <vector>
#include <unordered_map>

#include "chung/parser.hpp"
#include "chung/stringify.hpp"

#define MATCH_SIMPLE(condition, exception_string)                \
    if (current_token().condition) {                             \
        throw push_exception(exception_string, current_token()); \
    }                                                            \
    eat_token();                                                 \


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

    for (size_t i = 0; i <= source_line.length(); i++) {
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
        Token prev = previous_token();
        if (prev.type == TokenType::SYMBOL && prev.value.symbol == Symbol::SEMICOLON) {
            // std::cout << "Done synchronizing\n";
            // std::cout << stringify(next_token());
            return;
        } else {
            Token next = next_token();
            if (next.type == TokenType::KEYWORD && next.value.keyword == Keyword::LET) {
                // std::cout << "Done synchronizing\n";
                return;
            }
        }

        eat_token();
    }
}

std::shared_ptr<ExprAST> Parser::parse_call() {
    // Eat function callee
    Token callee = eat_token();

    // Eats '('
    MATCH_SIMPLE(value.symbol != Symbol::OPEN_PARENTHESES, "Expected '(' after function callee")
    std::vector<std::shared_ptr<ExprAST>> arguments;

    bool running = true;
    while (running) {
        if (auto argument = parse_expression()) {
            arguments.push_back(argument);
        } else {
            return nullptr;
        }

        switch (current_token().value.symbol) {
            case Symbol::CLOSE_PARENTHESES:
                running = false;
                break;
            case Symbol::COMMA:
                eat_token();
                break;
            default:
                throw push_exception("Expected ',' or ')' within function call", current_token());
        }
    }

    // Eat ')'
    eat_token();
    return std::make_shared<CallAST>(callee.value.identifier, std::move(arguments));
}

std::shared_ptr<ExprAST> Parser::parse_identifier() {
    Token token = current_token();
    Token next = next_token();

    if (next.value.symbol != Symbol::OPEN_PARENTHESES) {
        // Eat identifier
        eat_token();
        return std::make_shared<VariableAST>(token.value.identifier);
    }

    // A call
    return parse_call();
}

std::shared_ptr<ExprAST> Parser::parse_parentheses() {
    // Eat '('
    eat_token();
    std::shared_ptr<ExprAST> expr = parse_expression();
    if (!expr) {
        return nullptr;
    }

    // Eat ')'
    MATCH_SIMPLE(value.symbol != Symbol::CLOSE_PARENTHESES, "Expected closing parenthesis ')'")
    return expr;
}

std::shared_ptr<ExprAST> Parser::parse_bin_op(int min_op_precedence, std::shared_ptr<ExprAST> lhs) {
    while (true) {
        Token op = current_token();
        int op_precedence = get_op_precedence(op.value.op);

        if (op_precedence < min_op_precedence || op.type != TokenType::OPERATOR) {
            return lhs;
        }
        // std::cout << "Binary op: " << stringify(op.value.op) << '\n';

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
        case TokenType::STRING:
            return std::make_shared<PrimitiveAST>(token.value.string);
        default:
            // Invalid token
            throw push_exception("Invalid token in expression", token);
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
            return nullptr;
        default:
            // std::cout << "Primitive\n";
            return parse_primitive();
    }
}

std::vector<std::shared_ptr<StmtAST>> Parser::parse_block() {
    // Eat '{'
    MATCH_SIMPLE(value.symbol != Symbol::OPEN_BRACES, "Expected '{' at start of block")

    std::vector<std::shared_ptr<StmtAST>> statements;
    while (current_token().value.symbol != Symbol::CLOSE_BRACES) {
        statements.push_back(parse_statement());
    }

    // Eat '}'
    eat_token();

    return statements;
}

std::shared_ptr<StmtAST> Parser::parse_var_declaration() {
    // Eat 'let'
    eat_token();

    // Eat identifier
    Token identifier = current_token();
    if (identifier.type != TokenType::IDENTIFIER) {
         throw push_exception("Expected identifier to assign expression to", identifier);
    }
    eat_token();
    
    std::shared_ptr<ExprAST> expr = std::make_shared<PrimitiveAST>();
    if (current_token().type == TokenType::OPERATOR && current_token().value.op == Operator::ASSIGN) {
        // Eat '='
        eat_token();

        expr = parse_expression();
    }
    if (!expr) {
        return nullptr;
    }

    MATCH_SIMPLE(value.symbol != Symbol::SEMICOLON, "Expected ';' after identifier")

    return std::make_shared<VarDeclareAST>(identifier.value.identifier, std::move(expr));
}

std::shared_ptr<StmtAST> Parser::parse_function() {
    // Eat 'def'
    eat_token();

    // Get and eat function name
    Token name = current_token();
    MATCH_SIMPLE(type != TokenType::IDENTIFIER, "Expected function name after 'def'")

    // Eat '('
    MATCH_SIMPLE(value.symbol != Symbol::OPEN_PARENTHESES, "Expected '(' after function declaration")

    std::vector<VarDeclareAST> parameters;
    while (current_token().value.symbol != Symbol::CLOSE_PARENTHESES) {
        // Get and eat parameter name
        Token parameter = current_token();
        MATCH_SIMPLE(type != TokenType::IDENTIFIER, "Expected parameter name in function declaration")

        // No default values FOR NOW
        parameters.push_back(VarDeclareAST{parameter.value.identifier, nullptr});

        switch (current_token().value.symbol) {
            case Symbol::COMMA:
                eat_token();
            case Symbol::CLOSE_PARENTHESES:
                break;
            default:
                throw push_exception("Expected either '(' or ',' in function parameter list", current_token());
        }
    }

    // Eat ')'
    MATCH_SIMPLE(value.symbol != Symbol::CLOSE_PARENTHESES, "Expected ')' after parameter list")

    std::vector<std::shared_ptr<StmtAST>> body = parse_block();
    return std::make_shared<FunctionAST>(name.value.identifier, parameters, body);
}

std::shared_ptr<StmtAST> Parser::parse_omg() {
    // Eat '__omg'
    eat_token();

    std::shared_ptr<ExprAST> expr = parse_expression();
    if (!expr) {
        return nullptr;
    }

    // Eat ';'
    MATCH_SIMPLE(value.symbol != Symbol::SEMICOLON, "Expected ';' after value")

    return std::make_shared<OmgAST>(expr);
}

std::shared_ptr<ExprAST> Parser::parse_expression() {
    std::shared_ptr<ExprAST> lhs = parse_primary();
    if (lhs == nullptr) {
        return nullptr;
    }

    return parse_bin_op(0, std::move(lhs));
}

std::shared_ptr<StmtAST> Parser::parse_expression_statement() {
    std::shared_ptr<ExprAST> expr = parse_expression();

    // Eat ';'
    MATCH_SIMPLE(value.symbol != Symbol::SEMICOLON, "Expected ';' after expression")

    if (!expr) {
        return nullptr;
    }

    return std::make_shared<ExprStmtAST>(expr);
}

std::shared_ptr<StmtAST> Parser::parse_statement() {
    try {
        Token token = current_token();
        switch (token.type) {
            case TokenType::KEYWORD:
                if (token.value.keyword == Keyword::LET) {
                    return parse_var_declaration();
                } else if (token.value.keyword == Keyword::DEF) {
                    return parse_function();
                } else if (token.value.keyword == Keyword::__OMG) {
                    return parse_omg();
                } else {
                    std::cout << "You failed me.\n";
                    return nullptr;
                }
            
            default:
                return parse_expression_statement();
        }
    } catch (ParseException& exception) {
        synchronize();
        // std::cout << exception.write() << '\n';
        return nullptr;
    }
}

std::vector<std::shared_ptr<StmtAST>> Parser::parse() {
    std::vector<std::shared_ptr<StmtAST>> statements;

    while (current_token().type != TokenType::EOF) {
        std::shared_ptr<StmtAST> statement = parse_statement();
        if (statement) {
            statements.push_back(statement);
        }
    }

    return statements;
}