#include "chung/ast.hpp"

static std::unique_ptr<llvm::LLVMContext> context = std::make_unique<llvm::LLVMContext>();
static std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("OMG", *context);
static std::unique_ptr<llvm::IRBuilder<>> builder = std::make_unique<llvm::IRBuilder<>>(*context);
static std::map<std::string, llvm::Value*> named_values;