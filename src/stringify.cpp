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

// Goofy rn
std::string stringify(const std::u32string& s) {
    std::string out;
    std::transform(begin(s), end(s), back_inserter(out), [](char32_t c) {
        return c < 128 ? static_cast<char>(c) : '?';
    });
    return out;
}

std::string stringify(const Operator& op) {
    static const char *op_type_names[] = {
        "Add", "Subtract", "Multiply", "Divide", "Modulo", "Power",
        "BitwiseAnd", "BitwiseOr", "BitwiseNot",

        "Assign"
    };
    return op_type_names[static_cast<int>(op)];
}

std::string stringify(const TokenType& type) {
    static const char *token_type_names[] = {
        "EndOfFile", "Invalid", "Def", "Let", "Identifier", "Operator", "Symbol",
        "UInt64", "Int64", "Float64"
    };
    return token_type_names[static_cast<int>(type)];
}

std::string ExprAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);

    return indentation + "You suck why do you have the base class here go away";
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

std::string LiteralAST::stringify(size_t indent_level) {
    std::string indentation = indent(indent_level);

    switch (value_type) {
        case ValueType::INT64:
            return indentation + "Int64: " + std::to_string(int64);
        case ValueType::UINT64:
            return indentation + "UInt64: " + std::to_string(uint64);
        case ValueType::FLOAT64:
            return indentation + "Float64: " + std::to_string(float64);
    }
}

std::string VariableAST::stringify(size_t indent_level) {
    return "Amogus";
}