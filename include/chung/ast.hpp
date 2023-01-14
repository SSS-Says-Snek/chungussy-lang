#pragma once

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

#include <memory>
#include <vector>

#include "chung/token.hpp"

class AST {
public:
    virtual ~AST() = default;
    virtual std::string stringify(size_t indent_level = 0) = 0;
    virtual llvm::Value* codegen() = 0;
};

class StmtAST: public AST {
public:
    virtual ~StmtAST() = default;
    virtual std::string stringify(size_t indent_level = 0) = 0;
    virtual llvm::Value* codegen() = 0;
};

class ExprAST: public AST {
public:
    virtual ~ExprAST() = default;
    virtual std::string stringify(size_t indent_level = 0) = 0;
    virtual llvm::Value* codegen() = 0;
};

class VarDeclareAST: public StmtAST {
public:
    std::string name;
    std::shared_ptr<ExprAST> expr;

    VarDeclareAST(const std::string& name, std::shared_ptr<ExprAST> expr): name{name}, expr{std::move(expr)} {}

    std::string stringify(size_t indent_level = 0);
    llvm::Value* codegen();
};

class FunctionAST: public StmtAST {
public:
    std::string name;
    std::vector<VarDeclareAST> parameters;
    std::vector<std::shared_ptr<AST>> body;
};

class OmgAST: public StmtAST {
public:
    std::shared_ptr<ExprAST> expr;

    OmgAST(std::shared_ptr<ExprAST> expr): expr{std::move(expr)} {}

    std::string stringify(size_t indent_level = 0);
    llvm::Value* codegen();
};

class ExprStmtAST: public StmtAST {
public:
    std::shared_ptr<ExprAST> expr;

    ExprStmtAST(std::shared_ptr<ExprAST> expr): expr{std::move(expr)} {}

    std::string stringify(size_t indent_level = 0);
    llvm::Value* codegen();
};

class BinaryExprAST: public ExprAST {
public:
    Operator op;
    std::shared_ptr<ExprAST> lhs;
    std::shared_ptr<ExprAST> rhs;

    BinaryExprAST(Operator op, std::shared_ptr<ExprAST> lhs, std::shared_ptr<ExprAST> rhs):
        op{op}, lhs{std::move(lhs)}, rhs{std::move(rhs)} {}
    
    std::string stringify(size_t indent_level);
    llvm::Value* codegen();
};

class CallAST: public ExprAST {
public:
    std::string callee;
    std::vector<VarDeclareAST> arguments;

    CallAST(const std::string& callee, std::vector<VarDeclareAST> arguments):
        callee{callee}, arguments{std::move(arguments)} {}
    
    llvm::Value* codegen();
};

class PrimitiveAST: public ExprAST {
public:
    union {
        int64_t int64;
        uint64_t uint64;
        double float64;
    };
    enum ValueType {INVALID, INT64, UINT64, FLOAT64, NONE} value_type;

    PrimitiveAST(): value_type{ValueType::INVALID} {}
    PrimitiveAST(int64_t int64): int64{int64}, value_type{ValueType::INT64} {}
    PrimitiveAST(uint64_t uint64): uint64{uint64}, value_type{ValueType::UINT64} {}
    PrimitiveAST(double float64): float64{float64}, value_type{ValueType::FLOAT64} {}
    PrimitiveAST(std::nullptr_t): value_type{ValueType::NONE} {}

    std::string stringify(size_t indent_level = 0);
    llvm::Value* codegen();
};

class VariableAST: public ExprAST {
public:
    std::string name;

    VariableAST(const std::string& name): name{name} {}

    std::string stringify(size_t indent_level = 0);
    llvm::Value* codegen();
};
