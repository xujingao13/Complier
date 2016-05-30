#include <iostream>
#include "codegen.h"
#include "node.h"

using namespace std;

extern int yyparse();
extern NBlock* programBlock;


llvm::Function* createPrintfFunction(CodeGenContext& context)
{
    std::vector<llvm::Type*> printf_arg_types;
    printf_arg_types.push_back(llvm::Type::getInt8PtrTy(getGlobalContext())); //char*

    llvm::FunctionType* printf_type =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(getGlobalContext()), printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                llvm::Twine("printf"),
                context.module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

llvm::Function* createStrlenFunction(CodeGenContext& context)
{
    std::vector<llvm::Type*> strlen_arg_types;
    strlen_arg_types.push_back(llvm::Type::getInt8PtrTy(getGlobalContext())); //char*

    llvm::FunctionType* strlen_type =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(getGlobalContext()), strlen_arg_types, false);

    llvm::Function *func = llvm::Function::Create(
                strlen_type, llvm::Function::ExternalLinkage,
                llvm::Twine("strlen"),
                context.module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

void createCoreFunctions(CodeGenContext& context)
{
	createPrintfFunction(context);
	createStrlenFunction(context);
}
