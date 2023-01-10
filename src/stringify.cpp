#include "chung/stringify.hpp"

inline std::string indent(size_t indent_level) {
    std::string indentation;
    for (size_t i = 0; i < indent_level; i++) {
        indentation += "\t";
    }

    return indentation;
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
        "EndOfFile", "Invalid", "Def", "Identifier", "Operator", "Symbol",
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
            return indentation + "Integer64: " + std::to_string(int64);
    }
}

std::string VariableAST::stringify(size_t indent_level) {
    return "Amogus";
}