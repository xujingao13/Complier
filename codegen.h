#include <stack>
#include <typeinfo>
#include <algorithm>

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>       

using namespace llvm;

class NBlock;

class CodeGenBlock 
{
public:
    BasicBlock *block;
    std::map<std::string, Value*> locals;
};

class CodeGenContext 
{
    std::stack<CodeGenBlock *> blocks;
    std::map<std::string, Value*> global_vars;

    Function *mainFunction;
    Function *currentFunction;

    BasicBlock *returnBlock;
    Value *returnValue;
public:
    bool isTopLevel = true;
    
    Module *module;

    CodeGenContext() 
    { 
        module = new Module("main", getGlobalContext()); 
        /*module->setDataLayout("e-m:e-p:32:32-f64:32:64-f80:32-n8:16:32-S128");
        module->setTargetTriple("i386-pc-linux-gnu");*/
    }
    
    void generateCode(NBlock& root);

    GenericValue runCode();

    std::map<std::string, Value*>& locals() 
    { 
        return blocks.top()->locals; 
    }

    std::map<std::string, Value*>& globals() 
    { 
        return global_vars; 
    }

    BasicBlock *currentBlock() 
    { 
        return blocks.top()->block; 
    }

    void pushBlock(BasicBlock *block, bool copyLocals = true) 
    { 
        CodeGenBlock* newBlock = new CodeGenBlock();

        newBlock->block = block;

        if(copyLocals)
        {
            std::map<std::string, Value*> prevLocals = std::map<std::string, Value*>(blocks.top()->locals);
            newBlock->locals = prevLocals;
        }

        blocks.push(newBlock); 
    }

    void popBlock()
    { 
        CodeGenBlock *top = blocks.top();
        blocks.pop(); 

        delete top; 
    }

    void replaceBlock(BasicBlock* block)
    {
        blocks.top()->block = block;
    }

    void setCurrentReturnValue(Value *value) 
    { 
        returnValue = value;
    }

    Value* getCurrentReturnValue() 
    { 
        return returnValue; 
    }
    Function* getCurrentFunction()
    {
        return currentFunction;
    }
    void setCurrentFunction(Function *function)
    {
        currentFunction = function;
    }
    void writeToFile()
    {
        std::cout << "Dumping module to file\n";
        std::string ErrInfo = "";
        llvm::raw_ostream *out = new llvm::raw_fd_ostream("out.ll", ErrInfo, llvm::sys::fs::F_None );
        llvm::WriteBitcodeToFile(module, *out);

        delete out;
    }
    void setReturnBlock(BasicBlock* block)
    {
        returnBlock = block;
    }
    BasicBlock* getReturnBlock()
    {
        return returnBlock;
    }
};
