#include "chung/codegen.hpp"

void init_module() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("OMG", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

llvm::Value* VariableAST::codegen() {}

llvm::Value* VarDeclareAST::codegen() {
    // For now
    return expr->codegen();
}

llvm::Value* OmgAST::codegen() {}

llvm::Value* ExprStmtAST::codegen() {
    return expr->codegen();
}

llvm::Value* BinaryExprAST::codegen() {
    std::cout << "E";
    llvm::Value* lhs_code = lhs->codegen();
    llvm::Value* rhs_code = rhs->codegen();
    if (!lhs_code || !rhs_code) {
        return nullptr;
    }

    switch (op) {
        // TODO: Add type system (wow)
        case Operator::ADD:
            lhs_code->getType()->print(llvm::outs());
            return builder->CreateAdd(lhs_code, rhs_code);
    }
}

llvm::Value* PrimitiveAST::codegen() {
    switch (value_type) {
        case ValueType::INT64:
            std::cout << "Int\n";
            return llvm::ConstantInt::get(*context, llvm::APInt{64, static_cast<uint64_t>(int64), true});
        case ValueType::UINT64:
            std::cout << "Uint\n";
            return llvm::ConstantInt::get(*context, llvm::APInt{64, uint64, false});
        case ValueType::FLOAT64:
            std::cout << "Float\n";
            return llvm::ConstantFP::get(*context, llvm::APFloat{float64});
        default:
            std::cout << "L\n";
            return nullptr;
    }
}