#include <iostream>
#include "node.h"
#include "codegen.h"

using namespace std;

extern FILE* yyin;
extern int yyparse();
extern NBlock* programBlock;

extern void createCoreFunctions(CodeGenContext& context);


int main(int argc, char **argv)
{
	if (argc > 1)
		yyin = fopen(argv[1], "r");
	else
		yyin = stdin;	

	if (yyparse())
		return 0;

	InitializeNativeTarget();
	CodeGenContext context;
	createCoreFunctions(context);

	cout << "========================================================" << endl;
	cout << "begin to generate LLVM IR" << endl;
	cout << "========================================================" << endl;
	context.generateCode(*programBlock);
	
	cout << endl << endl;
	cout << "========================================================" << endl;
	cout << "begin to run LLVM IR" << endl;
	cout << "========================================================" << endl;
	context.runCode();
	
	cout << endl << endl;
	cout << "========================================================" << endl;
	cout << "THE END" << endl;
	cout << "========================================================" << endl;
	context.writeToFile();
	
	return 0;
}

