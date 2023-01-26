#include <algorithm>
#include <iterator>

#include "chung/stringify.hpp"
#include "chung/utf.hpp"

inline std::string indent(size_t indent_level) {
    std::string indentation;
    for (size_t i = 0; i < indent_level; i++) {
        indentation += "\t";
    }

    return indentation;
}

std::string stringify(const Operator& op, bool verbose) {
    static const char* op_names[] = {
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
    static const char* symbol_names[] = {
        "OpenParentheses", "CloseParentheses", "OpenBrackets", "CloseBrackets",
        "OpenBraces", "CloseBraces",
        "Dot", "Comma", "Colon", "Semicolon"
    };
    static const char* symbols[] = {
        "(", ")", "[", "]", "{", "}",
        "->",
        ".", ",", ":", ";"
    };

    if (verbose) {
        return symbol_names[static_cast<int>(symbol)];
    }
    return std::string{symbols[static_cast<int>(symbol)]};
}

std::string stringify(const Keyword& keyword) {
    static const char* keyword_names[] = {
        "Def", "Let", "__OMG"
    };
    return keyword_names[static_cast<int>(keyword)];
}

std::string stringify(const TokenType& type) {
    static const char* token_type_names[] = {
        "EndOfFile", "Invalid", "Identifier", "Operator", "Symbol", "Keyword"
        "UInt64", "Int64", "Float64"
    };
    return token_type_names[static_cast<int>(type)];
}

std::string stringify(const Token& token) {
    switch (token.type) {
        case TokenType::EOF:
            return "EOF";
        case TokenType::INVALID:
            return "Invalid";

        case TokenType::IDENTIFIER:
            return token.value.identifier;
        case TokenType::OPERATOR:
            return ::stringify(token.value.op);
        case TokenType::SYMBOL:
            return ::stringify(token.value.symbol);
        case TokenType::KEYWORD:
            return ::stringify(token.value.keyword);

        case TokenType::INT64:
            return std::to_string(token.value.int64);
        case TokenType::UINT64:
            return std::to_string(token.value.uint64);
        case TokenType::FLOAT64:
            return std::to_string(token.value.float64);
        
        default:
            return "Unknown";
    }
}

std::string AST::stringify(size_t indent_level) {
    // OOF
    return indent(indent_level) + "Goofy ASF AST";
}

std::string StmtAST::stringify(size_t indent_level) {
    return indent(indent_level) + "Goofy statement";
}

std::string ExprAST::stringify(size_t indent_level) {
    return indent(indent_level) + "Goofy expression";
}

std::string VarDeclareAST::stringify(size_t indent_level) {
    // std::cout << "Yum 1";
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Variable Declaration:"};

    string += "\n\t" + indentation + "Name: " + name;
    string += "\n\t" + indentation + "Value:\n" + expr->stringify(indent_level + 2);
    
    return string;
}

std::string OmgAST::stringify(size_t indent_level) {
    // std::cout << "Yum 11";
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Secret OMG:"};

    string += '\n' + indentation + expr->stringify(indent_level + 1);

    return string;
}

std::string ExprStmtAST::stringify(size_t indent_level) {
    // std::cout << "Yum 1111";
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Expression Statement:"};
    
    string += '\n' + indentation + expr->stringify(indent_level + 1);
    return string;
}

std::string BinaryExprAST::stringify(size_t indent_level) {
    // std::cout << "Yum111111";
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Binary Operation:"};

    string += "\n\t" + indentation + "Operator: " + ::stringify(op);

    // 2 new indentation level: 1 for "Binary Operation" and another for the side
    string += "\n\t" + indentation + "Left Hand:\n" + lhs->stringify(indent_level + 2);
    string += "\n\t" + indentation + "Right Hand:\n" + rhs->stringify(indent_level + 2);

    return string;
}

std::string CallAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);
    std::string string{indentation + "Call:"};

    string += "\n\t" + indentation + "Name: " + callee;
    string += "\n\t" + indentation + "Arguments:\n";
    
    for (size_t i = 0; i < arguments.size(); i++) {
        string += indentation + "Argument " + std::to_string(i + 1) + ":\n" + arguments[i]->stringify(indent_level + 2);
    }

    return string;
}

std::string PrimitiveAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);

    switch (value_type) {
        case ValueType::INT64:
            return indentation + "Int64: " + std::to_string(int64) + '\n';
        case ValueType::UINT64:
            return indentation + "UInt64: " + std::to_string(uint64) + '\n';
        case ValueType::FLOAT64:
            return indentation + "Float64: " + std::to_string(float64) + '\n';
        case ValueType::STRING:
            return indentation + "String: \"" + u32tostring(string) + "\"\n";
        default:
            return indentation + "Invalid\n";
    }
}

std::string VariableAST::stringify(size_t indent_level) {
    return "Amogus";
}