#include <algorithm>
#include <iterator>

#include "chung/stringify.hpp"

inline std::string indent(size_t indent_level) {
    std::string indentation;
    for (size_t i = 0; i < indent_level; i++) {
        indentation += "\t";
    }

    return indentation;
}

std::string stringify(const Operator& op, bool verbose) {
    static const char *op_names[] = {
        "Add", "Subtract", "Multiply", "Divide", "Modulo", "Power",
        "BitwiseAnd", "BitwiseOr", "BitwiseNot",

        "Assign"
    };
    static const char *ops[] = {
        "+", "-", "*", "/", "%", "**",
        "&", "|", "~",
        "="
    };

    if (verbose) {
        return op_names[static_cast<int>(op)];
    }
    return ops[static_cast<int>(op)];
}

std::string stringify(const Symbol& symbol, bool verbose) {
    static const char *symbol_names[] = {
        "OpenParentheses", "CloseParentheses", "OpenBrackets", "CloseBrackets",
        "OpenBraces", "CloseBraces",
        "Dot", "Comma", "Colon", "Semicolon"
    };
    static const char symbols[] = {
        '(', ')', '[', ']', '{', '}',
        '.', ',', ':', ';'
    };

    if (verbose) {
        return symbol_names[static_cast<int>(symbol)];
    }
    return std::string{symbols[static_cast<int>(symbol)]};
}

std::string stringify(const TokenType& type) {
    static const char *token_type_names[] = {
        "EndOfFile", "Invalid", "Identifier", "Operator", "Symbol",
        "Def", "Let", "__Omg",
        "UInt64", "Int64", "Float64"
    };
    return token_type_names[static_cast<int>(type)];
}

std::string stringify(const Token& token) {
    switch (token.type) {
        case TokenType::EOF:
            return "EOF";
        case TokenType::IDENTIFIER:
            return token.value.identifier;
        case TokenType::OPERATOR:
            return ::stringify(token.value.op);
        case TokenType::SYMBOL:
            return ::stringify(token.value.symbol);

        case TokenType::INT64:
            return std::to_string(token.value.int64);
        case TokenType::UINT64:
            return std::to_string(token.value.uint64);
        case TokenType::FLOAT64:
            return std::to_string(token.value.float64);
        
        default:
            return "unknown";
    }
}

std::string AST::stringify(size_t indent_level) {
    return indent(indent_level) + "You suck why do you have the base class here go away";
}

std::string StmtAST::stringify(size_t indent_level) {
    return indent(indent_level) + "You suck why do you have the stmt base class here go away";
}

std::string ExprAST::stringify(size_t indent_level) {
    return indent(indent_level) + "You suck why do you have the expr base class here go away";
}

std::string VarDeclareAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Variable Assignment:"};

    string += "\n\t" + indentation + "Name: " + name;
    string += "\n\t" + indentation + "Value:\n" + expr->stringify(indent_level + 2);
    
    return string;
}

std::string OmgAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Secret OMG:"};

    string += '\n' + indentation + expr->stringify(indent_level + 1);

    return string;
}

std::string ExprStmtAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Expression Statement:"};
    
    string += '\n' + indentation + expr->stringify(indent_level + 1);
    return string;
}

std::string BinaryExprAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Binary Operation:"};

    string += "\n\t" + indentation + "Operator: " + ::stringify(op);

    // 2 new indentation level: 1 for "Binary Operation" and another for the side
    string += "\n\t" + indentation + "Left Hand:\n" + lhs->stringify(indent_level + 2);
    string += "\n\t" + indentation + "Right Hand:\n" + rhs->stringify(indent_level + 2);

    return string;
}

std::string PrimitiveAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);

    switch (value_type) {
        case ValueType::INT64:
            return indentation + "Int64: " + std::to_string(int64);
        case ValueType::UINT64:
            return indentation + "UInt64: " + std::to_string(uint64);
        case ValueType::FLOAT64:
            return indentation + "Float64: " + std::to_string(float64);
        case ValueType::NONE:
            return indentation + "None";
        default:
            return indentation + "Invalid";
    }
}

std::string VariableAST::stringify(size_t indent_level) {
    return "Amogus";
}