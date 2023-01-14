#include "chung/codegen.hpp"

void init_module() {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>("OMG", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

llvm::Value* VariableAST::codegen() {}
llvm::Value* VarDeclareAST::codegen() {}
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
        case Operator::ADD:
        std::cout << "A";
            lhs_code->getType()->print(llvm::outs());
    }
}

llvm::Value* PrimitiveAST::codegen() {
    switch (value_type) {
        case ValueType::INT64:
            std::cout << "Int";
            return llvm::ConstantInt::get(*context, llvm::APInt{64, static_cast<uint64_t>(int64), true});
        case ValueType::UINT64:
            std::cout << "Uint";
            return llvm::ConstantInt::get(*context, llvm::APInt{64, uint64, false});
        case ValueType::FLOAT64:
            std::cout << "Float";
            return llvm::ConstantFP::get(*context, llvm::APFloat{float64});
        default:
            std::cout << "L";
            return nullptr;
    }
}