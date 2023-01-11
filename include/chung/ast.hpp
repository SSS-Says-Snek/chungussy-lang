#pragma once

#include <memory>

#include "chung/token.hpp"

class AST {
public:
    virtual ~AST() = default;
    virtual std::string stringify(size_t indent_level = 0) = 0;
};

class StmtAST: public AST {
public:
    virtual ~StmtAST() = default;
    virtual std::string stringify(size_t indent_level = 0) = 0;
};

class ExprAST: public AST {
public:
    virtual ~ExprAST() = default;
    virtual std::string stringify(size_t indent_level = 0) = 0;
};

class BinaryExprAST: public ExprAST {
public:
    Operator op;
    std::shared_ptr<ExprAST> lhs;
    std::shared_ptr<ExprAST> rhs;

    BinaryExprAST(Operator op, std::shared_ptr<ExprAST> lhs, std::shared_ptr<ExprAST> rhs):
        op{op}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}
    
    std::string stringify(size_t indent_level);
};

class LiteralAST: public ExprAST {
public:
    union {
        int64_t int64;
        uint64_t uint64;
        double float64;
    };
    enum ValueType {INT64, UINT64, FLOAT64} value_type;

    LiteralAST(int64_t int64): int64{int64}, value_type{ValueType::INT64} {}
    LiteralAST(uint64_t uint64): uint64{uint64}, value_type{ValueType::UINT64} {}
    LiteralAST(double float64): float64{float64}, value_type{ValueType::FLOAT64} {}

    std::string stringify(size_t indent_level = 0);
};

class VariableAST: public ExprAST {
public:
    std::string name;

    VariableAST(const std::string& name): name{name} {}

    std::string stringify(size_t indent_level = 0);
};

class AssignAST: public StmtAST {
    std::string name;
    std::shared_ptr<ExprAST> expr;
};