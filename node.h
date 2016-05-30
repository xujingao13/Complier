#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

class Node 
{
public:
	virtual ~Node() {}
	virtual llvm::Value* codeGen(CodeGenContext& context) = 0; //{ return NULL; }
};

class NExpression : public Node 
{
};

class NStatement : public Node 
{
};

class NInteger : public NExpression 
{
public:
	long long value;

	NInteger(long long value) : value(value) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NDouble : public NExpression 
{
public:
	double value;

	NDouble(double value) : value(value) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NChar : public NExpression
{
public:
	char value;

	NChar(char value) : value(value) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NIdentifier : public NExpression 
{
public:
	std::string name;

	NIdentifier(const std::string& name) : name(name) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NMethodCall : public NExpression 
{
public:
	const NIdentifier& id;
	ExpressionList arguments;

	NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
		id(id), arguments(arguments) { }
	NMethodCall(const NIdentifier& id) : id(id) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBinaryOperator : public NExpression
 {
public:
	int op;
	NExpression& lhs;
	NExpression& rhs;

	NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
		lhs(lhs), rhs(rhs), op(op) { }


	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAssignment : public NExpression 
{
public:
	NIdentifier& lhs;
	NExpression& rhs;

	NAssignment(NIdentifier& lhs, NExpression& rhs) : 
		lhs(lhs), rhs(rhs) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NBlock : public NExpression 
{
public:
	StatementList statements;

	NBlock() { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExpressionStatement : public NStatement 
{
public:
	NExpression& expression;

	NExpressionStatement(NExpression& expression) : 
		expression(expression) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NReturnStatement : public NStatement 
{
public:
	NExpression& expression;

	NReturnStatement(NExpression& expression) : 
		expression(expression) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement
{
public:
	const NIdentifier& type;
	NIdentifier& id;
	NExpression *assignmentExpr;

	NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
		type(type), id(id) { }
	NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
		type(type), id(id), assignmentExpr(assignmentExpr) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NExternDeclaration : public NStatement 
{
public:
    const NIdentifier& type;
    const NIdentifier& id;
    VariableList arguments;

    NExternDeclaration(const NIdentifier& type, const NIdentifier& id,
            const VariableList& arguments) :
        type(type), id(id), arguments(arguments) {}

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement 
{
public:
	const NIdentifier& type;
	const NIdentifier& id;
	VariableList arguments;
	NBlock& block;

	NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, 
			const VariableList& arguments, NBlock& block) :
		type(type), id(id), arguments(arguments), block(block) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NWhileStatement : public NStatement
{
public:
	NExpression& expression;
	NBlock& block;

	NWhileStatement(NExpression& expression, NBlock& block) :
		expression(expression), block(block) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NForStatement : public NStatement
{
public:
	NExpression& initExpression;
	NExpression& conditionExpression;
	NExpression& loopExpression;
	NBlock& block;

	NForStatement(NExpression& initExpr, NExpression& conditionExpr, NExpression& loopExpr, NBlock& block) :
		initExpression(initExpr), conditionExpression(conditionExpr), loopExpression(loopExpr), block(block) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NConditionStatement : public NStatement
{
public:
	NExpression& expression;
	NBlock& trueBlock;
	NBlock& falseBlock;

	NConditionStatement(NExpression& expression, NBlock& trueBlock, NBlock& falseBlock) :
		expression(expression), trueBlock(trueBlock), falseBlock(falseBlock) { }


	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NCastExpression : public NExpression
{
public:
	NExpression& expression;
	NIdentifier& id;

	NCastExpression(NIdentifier& id, NExpression& expression) :
		id(id), expression(expression) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NArrayDeclaration : public NStatement
{
public:
	NIdentifier& id;
	NIdentifier& type;
	ExpressionList init;
	long long size;

	NArrayDeclaration(NIdentifier& type, NIdentifier& id, long long size) :
		type(type), id(id), size(size) { }

	NArrayDeclaration(NIdentifier& type, NIdentifier& id, std::string& str) :
		type(type), id(id)
	{ 
		for(auto iter = str.begin(); iter != str.end(); ++iter)
		{
			init.push_back((NExpression*)(new NChar(*iter)));
		}

		init.push_back((NExpression*)(new NChar(0)));
		size = init.size() + 1;
	}

	NArrayDeclaration(NIdentifier& type, NIdentifier& id, ExpressionList& init) :
		type(type), id(id), init(init), size(init.size()) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NAccessToArray : public NExpression
{
public:
	NIdentifier& id;
	NExpression& expression;
	NExpression* assignment;

	NAccessToArray(NIdentifier& id, NExpression& expression, NExpression* assignment) : 
		id(id), expression(expression), assignment(assignment) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NGlobalVariableDeclaration : public NStatement
{
public:
	const NIdentifier& type;
	NIdentifier& id;
	NExpression *assignmentExpr;

	NGlobalVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
		type(type), id(id) { }
	NGlobalVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
		type(type), id(id), assignmentExpr(assignmentExpr) { }

	virtual llvm::Value* codeGen(CodeGenContext& context);
};

