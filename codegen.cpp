#include "node.h"
#include "codegen.h"
#include "parser.hpp"																																						

using namespace std;
																																																	
bool error = false;

static Value* cast(Value* value, Type* type, CodeGenContext& context)
{
	if(type == value->getType())
		return value;

	if(type == Type::getDoubleTy(getGlobalContext()))
	{
		if(value->getType() == Type::getInt64Ty(getGlobalContext()) || value->getType() == Type::getInt8Ty(getGlobalContext()))
		{
			std::cout << "Cast integer to double.\n"; 
			value = new SIToFPInst(value, type, "", context.currentBlock());
		}
		else
		{
			std::cout << "Cannot cast1.\n";
		}
	}
	else if(type == Type::getInt64Ty(getGlobalContext()))
	{
		if(value->getType() == Type::getDoubleTy(getGlobalContext()))
		{
			std::cout << "Cast double to int.\n";
			value = new FPToSIInst(value, type, "", context.currentBlock());
		}
		else if(value->getType() == Type::getInt8Ty(getGlobalContext()))
		{
			std::cout << "Cast byte to int.\n";
			value = new SExtInst(value, type, "", context.currentBlock());
		}
		else
		{
			std::cout << "Cannot cast2.\n";
		}
	}
	else if(type == Type::getInt8Ty(getGlobalContext()))
	{
		if(value->getType() == Type::getDoubleTy(getGlobalContext()))
		{
			std::cout << "Cast double to byte.\n";
			value = new FPToSIInst(value, type, "", context.currentBlock());
		}
		else if(value->getType() == Type::getInt64Ty(getGlobalContext()))
		{
			std::cout << "Cast int to byte.\n";
			value = new TruncInst(value, type, "", context.currentBlock());
		}
		else
		{
			std::cout << "Cannot cast3\n";
		}
	}
	else
    {
		std::cout << "Cannot cast4.\n";
	}

	return value;
}

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock& root)
{
	std::cout << "Generating code...\n";
	
	/* Create the top level interpreter function to call as entry */
	vector<Type*> argTypes;
	FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), makeArrayRef(argTypes), false);
	//mainFunction = Function::Create(ftype, GlobalValue::ExternalLinkage, "main", module);
	//BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);
	//mainFunction->setCallingConv(CallingConv::C);

	AttributeSet func_func_PAL;
	{
	 	SmallVector<AttributeSet, 4> Attrs;
	  	AttributeSet PAS;
	  	{
	    	AttrBuilder B;
	    	B.addAttribute(Attribute::NoUnwind);
	    	PAS = AttributeSet::get(getGlobalContext(), ~0U, B);
	    }
	  
	  	Attrs.push_back(PAS);
	  	func_func_PAL = AttributeSet::get(getGlobalContext(), Attrs);
	 }
	 //mainFunction->setAttributes(func_func_PAL);
	
	/* Push a new variable/block context */
	//pushBlock(bblock, false);
	root.codeGen(*this); /* emit bytecode for the toplevel block */
	//ReturnInst::Create(getGlobalContext(), bblock);
	//popBlock();
	
	/* Print the bytecode in a human-readable format 
	   to see if our program compiled properly
	 */
	//std::cout << "Code is generated.\n";
	//module->dump();
	verifyModule(*module, PrintMessageAction);
	PassManager pm;
	pm.add(createPrintModulePass(&outs()));
	pm.run(*module);
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() 
{
	std::cout << "Running code...\n";
	ExecutionEngine *ee = EngineBuilder(module).create();
	vector<GenericValue> noargs;
	mainFunction = module->getFunction("main");
	GenericValue v = ee->runFunction(mainFunction, noargs);
	std::cout << endl;
	//std::cout << "Code was run.\n";
	return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdentifier& ident) 
{
	Type* type;
	std::string str = std::string(ident.name);
	int count = 0;

	while(str.back() == '*')
	{
		count++;
		str.pop_back();
	}

	if (str.compare("int") == 0) 
	{
		type =  Type::getInt64Ty(getGlobalContext());
	}
	else if (str.compare("double") == 0) 
	{
		type = Type::getDoubleTy(getGlobalContext());
	}
	else if (str.compare("char") == 0)
	{
		type = Type::getInt8Ty(getGlobalContext());
	}
	else if (str.compare("void") == 0)
	{
		type = Type::getVoidTy(getGlobalContext());
	}

	while(count != 0)
	{
		type = PointerType::get(type, 0);
		count--;
	}

	return type;
}

/* -- Code Generation -- */

Value* NInteger::codeGen(CodeGenContext& context)
{
	std::cout << "Creating integer: " << value << endl;

	return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
}

Value* NDouble::codeGen(CodeGenContext& context)
{
	std::cout << "Creating double: " << value << endl;

	return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value);
}

Value* NChar::codeGen(CodeGenContext& context)
{
	std::cout << "Creating char: " << value << std::endl;

	return ConstantInt::get(Type::getInt8Ty(getGlobalContext()), value, true);
}

Value* NIdentifier::codeGen(CodeGenContext& context)
{
	std::cout << "Creating identifier reference: " << name << endl;

	Value* var;

	if (context.isTopLevel || context.locals().find(name) == context.locals().end()) 
	{
		if(context.globals().find(name) == context.globals().end())
		{
			std::cerr << "undeclared variable " << name << endl;
			return NULL;
		}
		else
		{
			var = context.globals()[name];
		}
	}
	else
	{
		var = context.locals()[name];
	}

	if(((AllocaInst*)var)->getAllocatedType()->isArrayTy()) //|| ((AllocaInst*)var)->getAllocatedType()->isPointerTy())
	{
		std::cout << "return array " << name << std::endl;

		ConstantInt* const_int = ConstantInt::get(getGlobalContext(), APInt(32, StringRef("0"), 10));

		std::vector<Value*> args;

	   	args.push_back(const_int);
	   	args.push_back(const_int);

		var = GetElementPtrInst::Create(var, args, "", context.currentBlock());
		return var;
	}
	else
	{
		std::cout << "return var " << name << std::endl;
		return new LoadInst(var, "", false, context.currentBlock());
	}
}

Value* NMethodCall::codeGen(CodeGenContext& context)
{
	Function *function = context.module->getFunction(id.name.c_str());

	if (function == NULL) 
	{
		std::cerr << "no such function " << id.name << endl;
	}

	std::vector<Value*> args;
	ExpressionList::const_iterator it;

	for (it = arguments.begin(); it != arguments.end(); it++) 
	{
		args.push_back((**it).codeGen(context));
	}

	CallInst *call = CallInst::Create(function, makeArrayRef(args), "", context.currentBlock());

	std::cout << "Creating method call: " << id.name << endl;

	return call;
}

Value* NBinaryOperator::codeGen(CodeGenContext& context)
{
	std::cout << "Creating binary operation " << op << endl;

	Instruction::BinaryOps instr;
	ICmpInst::Predicate pred;

	bool isFloating = false;

	Value* left = lhs.codeGen(context);
	Value* right = rhs.codeGen(context);

	left->getType()->dump();
	right->getType()->dump();

	if(left->getType()->isDoubleTy() || right->getType()->isDoubleTy())
	//if(left->getType() == Type::getDoubleTy(getGlobalContext()) || left->getType() == Type::getDoubleTy(getGlobalContext()))
	{
		left = cast(left, Type::getDoubleTy(getGlobalContext()), context);
		right = cast(right, Type::getDoubleTy(getGlobalContext()), context);

		isFloating = true;
	}
	else
	{
		left = cast(left, Type::getInt64Ty(getGlobalContext()), context);
		right = cast(right, Type::getInt64Ty(getGlobalContext()), context);
	}


	if(!isFloating)
	{
		switch (op)
		{
			case TPLUS  :	instr = Instruction::Add; goto math;
			case TMINUS : 	instr = Instruction::Sub; goto math;
			case TMUL   :	instr = Instruction::Mul; goto math;
			case TDIV   :	instr = Instruction::SDiv; goto math;

			case TCEQ   :   pred = ICmpInst::ICMP_EQ; goto logic;
			case TCNE   :   pred = ICmpInst::ICMP_NE; goto logic;
			case TCGT   :   pred = ICmpInst::ICMP_SGT; goto logic;
			case TCLT   :   pred = ICmpInst::ICMP_SLT; goto logic;
			case TCGE   :   pred = ICmpInst::ICMP_SGE; goto logic;   
			case TCLE   :   pred = ICmpInst::ICMP_SLE; goto logic;
		}
	}
	else
	{
		switch (op)
		{
			case TPLUS  :	instr = Instruction::FAdd; goto math;
			case TMINUS : 	instr = Instruction::FSub; goto math;
			case TMUL   :	instr = Instruction::FMul; goto math;
			case TDIV   :	instr = Instruction::FDiv; goto math;

			case TCEQ   :   pred = ICmpInst::FCMP_OEQ; goto logic;
			case TCNE   :   pred = ICmpInst::FCMP_ONE; goto logic;
			case TCGT   :   pred = ICmpInst::FCMP_OGT; goto logic;
			case TCLT   :   pred = ICmpInst::FCMP_OLT; goto logic;
			case TCGE   :   pred = ICmpInst::FCMP_OGE; goto logic;   
			case TCLE   :   pred = ICmpInst::FCMP_OLE; goto logic;
		}
	}

	return NULL;

math:
	return BinaryOperator::Create(instr, left, right, "", context.currentBlock());

logic:
	return new ICmpInst(*context.currentBlock(), pred, left, right, "");
}

Value* NAssignment::codeGen(CodeGenContext& context)
{
	std::cout << "Creating assignment for " << lhs.name << endl;

	Value* right;
	Value* left;

	if (context.isTopLevel || context.locals().find(lhs.name) == context.locals().end()) 
	{
		if (context.globals().find(lhs.name) == context.globals().end()) 
		{
			std::cerr << "undeclared variable " << lhs.name << endl;
			return NULL;
		}
		else
		{
			left = context.globals()[lhs.name];
		}
	}
	else 
	{
		left = context.locals()[lhs.name];
	}
	
	right = rhs.codeGen(context);

	if(left->getType() == Type::getInt64PtrTy(getGlobalContext()))
	{
		right = cast(right, Type::getInt64Ty(getGlobalContext()), context);
	}
	else if(left->getType() == Type::getDoublePtrTy(getGlobalContext()))
	{
		right = cast(right, Type::getDoubleTy(getGlobalContext()), context);
	}
	else if(left->getType() == Type::getInt8PtrTy(getGlobalContext()))
	{
		std::cout << "int8 cast\n";
		right = cast(right, Type::getInt8Ty(getGlobalContext()), context);
	}
	return new StoreInst(right, left, false, context.currentBlock());
}

Value* NBlock::codeGen(CodeGenContext& context)
{
	StatementList::const_iterator it;
	Value *last = NULL;

	for (it = statements.begin(); it != statements.end(); it++) 
	{
		std::cout << "Generating code for " << typeid(**it).name() << endl;
		last = (**it).codeGen(context);
	}

	std::cout << "Creating block" << endl;

	return last;
}

Value* NExpressionStatement::codeGen(CodeGenContext& context)
{
	std::cout << "Generating code for " << typeid(expression).name() << endl;

	return expression.codeGen(context);
}

Value* NReturnStatement::codeGen(CodeGenContext& context)
{
	std::cout << "Generating return code for " << typeid(expression).name() << endl;

	Value* returnValue = expression.codeGen(context);

	context.setCurrentReturnValue(returnValue);
	BranchInst::Create(context.getReturnBlock(), context.currentBlock());

	return returnValue;
}

Value* NVariableDeclaration::codeGen(CodeGenContext& context)
{
	std::cout << "Creating local variable declaration " << type.name << " " << id.name << std::endl;
	Value* new_var;

	new_var = new AllocaInst(typeOf(type), id.name.c_str(), context.currentBlock());
	context.locals()[id.name] = new_var;

	if (assignmentExpr != NULL) 
	{
		NAssignment assn(id, *assignmentExpr);
		assn.codeGen(context);
	}
	return new_var;
}

Value* NExternDeclaration::codeGen(CodeGenContext& context)
{
    vector<Type*> argTypes;
    VariableList::const_iterator it;

    for (it = arguments.begin(); it != arguments.end(); it++) 
    {
        argTypes.push_back(typeOf((**it).type));
    }

    FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
    Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);

    return function;
}

Value* NFunctionDeclaration::codeGen(CodeGenContext& context)
{
	vector<Type*> argTypes;
	VariableList::const_iterator it;

	for (it = arguments.begin(); it != arguments.end(); it++) 
	{
		argTypes.push_back(typeOf((**it).type));
	}
	FunctionType *ftype = FunctionType::get(typeOf(type), makeArrayRef(argTypes), false);
	Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);
	BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);
	BasicBlock *returnBlock = BasicBlock::Create(getGlobalContext(), "return", function, 0);

	function->setCallingConv(CallingConv::C);

	AttributeSet func_func_PAL;
	{
	 	SmallVector<AttributeSet, 4> Attrs;
	  	AttributeSet PAS;
	  	{
	    	AttrBuilder B;
	    	B.addAttribute(Attribute::NoUnwind);
	    	PAS = AttributeSet::get(getGlobalContext(), ~0U, B);
	    }
	  
	  	Attrs.push_back(PAS);
	  	func_func_PAL = AttributeSet::get(getGlobalContext(), Attrs);
	}
	function->setAttributes(func_func_PAL);

	context.setCurrentFunction(function);
	context.setReturnBlock(returnBlock);
	context.isTopLevel = false;

	context.pushBlock(bblock, false);

	Function::arg_iterator argsValues = function->arg_begin();
    Value* argumentValue;

	for (it = arguments.begin(); it != arguments.end(); it++) 
	{
		(**it).codeGen(context);
		
		argumentValue = argsValues++;
		argumentValue->setName((*it)->id.name.c_str());
		StoreInst *inst = new StoreInst(argumentValue, context.locals()[(*it)->id.name], false, bblock);
	}
	
	block.codeGen(context);
	ReturnInst::Create(getGlobalContext(), context.getCurrentReturnValue(), context.getReturnBlock());

	context.popBlock();
	context.isTopLevel = true;
	std::cout << "Creating function: " << id.name << endl;
	
	return function;
}

Value* NWhileStatement::codeGen(CodeGenContext& context)
{
	BasicBlock* labelStart = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);
	BasicBlock* labelEnd = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);
	BasicBlock* labelCheck = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);

	BranchInst::Create(labelCheck, context.currentBlock());

	context.pushBlock(labelCheck);
	BranchInst::Create(labelStart, labelEnd, expression.codeGen(context), context.currentBlock());
	context.popBlock();

	context.pushBlock(labelStart);
	block.codeGen(context);
	BranchInst::Create(labelCheck, context.currentBlock());
	context.popBlock();

	context.replaceBlock(labelEnd);

	return labelEnd;
}

Value* NForStatement::codeGen(CodeGenContext& context)
{
	initExpression.codeGen(context);

	BasicBlock* labelStart = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);
	BasicBlock* labelEnd = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);
	BasicBlock* labelCheck = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);

	BranchInst::Create(labelCheck, context.currentBlock());

	context.pushBlock(labelCheck);
	BranchInst::Create(labelStart, labelEnd, conditionExpression.codeGen(context), context.currentBlock());
	context.popBlock();

	context.pushBlock(labelStart);
	block.codeGen(context);
	loopExpression.codeGen(context);
	BranchInst::Create(labelCheck, context.currentBlock());
	context.popBlock();

	context.replaceBlock(labelEnd);

	return labelEnd;
}

Value* NConditionStatement::codeGen(CodeGenContext& context)
{
	BasicBlock* labelTrue = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);
	BasicBlock* labelFalse = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);
	BasicBlock* labelEnd = BasicBlock::Create(getGlobalContext(), "", context.getCurrentFunction(), 0);

	BranchInst::Create(labelTrue, labelFalse, expression.codeGen(context), context.currentBlock());
	context.pushBlock(labelTrue);
	trueBlock.codeGen(context);
	BranchInst::Create(labelEnd, context.currentBlock());
	context.popBlock();

	context.pushBlock(labelFalse);
	falseBlock.codeGen(context);
	BranchInst::Create(labelEnd, context.currentBlock());
	context.popBlock();

	context.replaceBlock(labelEnd);


	return labelEnd;
}

Value* NCastExpression::codeGen(CodeGenContext& context)
{
	Value* value = expression.codeGen(context);

	Type* cast_type = typeOf(id);

	std::cout << "Creating cast operator.\n";
	value = cast(value, cast_type, context);
	
	return value;
}

Value* NArrayDeclaration::codeGen(CodeGenContext& context)
{
	std::cout << "Creating array declaration " << type.name << " " << id.name<< " " << size << std::endl;

	ArrayType* array_type = ArrayType::get(typeOf(type), size);

	AllocaInst *alloc = new AllocaInst(array_type, id.name.c_str(), context.currentBlock());

	context.locals()[id.name] = alloc;
	if(init.size() != 0)
	{
		for(auto iter = init.begin(); iter != init.end(); ++iter)
		{
			NExpression* num = new NInteger(iter - init.begin());
			NAccessToArray a(id, *num, *iter);

			a.codeGen(context);

			delete num;
		}
	}

	return alloc;
}

Value* NAccessToArray::codeGen(CodeGenContext& context)
{
	std::cout << "Accessing to " << id.name << " array.\n";

	Instruction* instr;
	Value* num = cast(expression.codeGen(context), Type::getInt64Ty(getGlobalContext()), context);
	Value* array = id.codeGen(context);
	ConstantInt* const_int = ConstantInt::get(getGlobalContext(), APInt(32, StringRef("0"), 10));

	num = new TruncInst(num, Type::getInt32Ty(getGlobalContext()), "", context.currentBlock());

	std::vector<Value*> args;

   	args.push_back(const_int);
   	args.push_back(num);

   	std::cout << "step 1\n";
   	array->getType()->dump();
   	((AllocaInst*)array)->getAllocatedType()->dump();

   	if(((AllocaInst*)array)->getAllocatedType()->isArrayTy())
   	{
    	instr = GetElementPtrInst::Create(array, args, "", context.currentBlock());
    	std::cout << "array\n";
    }
    else 
    {
    	std::cout << "pointer\n";
		instr = GetElementPtrInst::Create(array, num, "", context.currentBlock());
	}

    std::cout << "step 2\n";
    Value* ret;

    if(assignment == NULL)
	{
    	ret = new LoadInst(instr, "", false, context.currentBlock());
	}
    else
    {
    	//Value* assign = cast(assignment->codeGen(context), Type::getInt8Ty(getGlobalContext()) ,context);
    	ret = new StoreInst(assignment->codeGen(context), instr, false, context.currentBlock());
    }
    ret->getType()->dump();
    std::cout << "step 3\n";
    return ret;
}

Value* NGlobalVariableDeclaration::codeGen(CodeGenContext& context)
{
	std::cout << "Creating global variable declaration " << type.name << " " << id.name << std::endl;
	Value* new_var;

	ConstantInt* init = (ConstantInt*)ConstantInt::get(typeOf(type), 0, true);

	new_var = new GlobalVariable(*context.module,typeOf(type), false, GlobalValue::CommonLinkage, 0, id.name.c_str()); 
	((GlobalVariable*)new_var)->setInitializer(init);
	context.globals()[id.name] = new_var;

	/*if (assignmentExpr != NULL) 
	{
		NAssignment assn(id, *assignmentExpr);
		assn.codeGen(context);
	}*/
	return new_var;
}
