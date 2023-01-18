#include "chung/ast.hpp"

llvm::Value* codegen(Context& ctx) {}

llvm::Value* VarDeclareAST::codegen(Context& ctx) {
    // For now
    return expr->codegen(ctx);
}

llvm::Value* OmgAST::codegen(Context& ctx) {
}

llvm::Value* ExprStmtAST::codegen(Context& ctx) {
    return expr->codegen(ctx);
}

llvm::Value* BinaryExprAST::codegen(Context& ctx) {
    std::cout << "E";
    llvm::Value* lhs_code = lhs->codegen(ctx);
    llvm::Value* rhs_code = rhs->codegen(ctx);
    if (!lhs_code || !rhs_code) {
        return nullptr;
    }

    switch (op) {
        // TODO: Add type system (wow)
        case Operator::ADD:
            lhs_code->getType()->print(llvm::outs());
            return ctx.builder.CreateAdd(lhs_code, rhs_code);
    }
}

llvm::Value* CallAST::codegen(Context& ctx) {}

llvm::Value* PrimitiveAST::codegen(Context& ctx) {
    switch (value_type) {
        case ValueType::INT64:
            std::cout << "Int\n";
            return llvm::ConstantInt::get(ctx.context, llvm::APInt{64, static_cast<uint64_t>(int64), true});
        case ValueType::UINT64:
            std::cout << "Uint\n";
            return llvm::ConstantInt::get(ctx.context, llvm::APInt{64, uint64, false});
        case ValueType::FLOAT64:
            std::cout << "Float\n";
            return llvm::ConstantFP::get(ctx.context, llvm::APFloat{float64});
        default:
            std::cout << "L\n";
            return nullptr;
    }
}

llvm::Value* VariableAST::codegen(Context& ctx) {}