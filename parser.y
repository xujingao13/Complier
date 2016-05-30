%{
    #include <cstdio>
 	#include <cstdlib>
 	#include <iostream>
 	#include "node.h"

 	NBlock *programBlock;
 	extern int line;
 	int yyerror(const char *str)
	{
   		printf("Error: %s at %d line\n", str, line);
	}
	extern int yylex();

%}


%union 
{
	Node *node;
	NBlock *block;
	NExpression *expr;
	NStatement *stmt;
	NIdentifier *ident;
	NVariableDeclaration *var_decl;
	NGlobalVariableDeclaration *global_var_decl;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<NExpression*> *exprvec;
	std::string *string;
	int token;
}

%token <string> TIDENTIFIER TINTEGER TDOUBLE TCHARACTER TINT TDBL TCHAR TVOID TSTR 
%token <token> TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL TCAND TCOR
%token <token> TLPAREN TRPAREN TCOMMA TDOT TCOLON TSEMICOLON TSHARP TTILDE TLBRACKET TRBRACKET TLBRACE TRBRACE
%token <token> TPLUS TMINUS TMUL TDIV
%token <token> TRETURN TWHILE TEXTERN TFOR
%token <token> TIF TELSE

%type <ident> ident type
%type <expr> numeric expr
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <block> program global_stmts local_stmts block 
%type <stmt> local_stmt global_stmt var_decl global_var_decl func_decl extern_decl condition cycle array_decl
%type <token> comparison

%left TPLUS TMINUS
%left TMUL TDIV

%error-verbose

%start program

%%

program : global_stmts 							{ programBlock = $1; }
		;

global_stmts : global_stmt						{ $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
			 | global_stmts global_stmt 		{ $$->statements.push_back($<stmt>2); }
			 ;

local_stmts : local_stmt 						{ $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
			| local_stmts local_stmt   			{ $$->statements.push_back($<stmt>2); }
			;

global_stmt : func_decl
			| global_var_decl TSEMICOLON
			| extern_decl TSEMICOLON
			| expr 								{ $$ = new NExpressionStatement(*$1); }
			;

local_stmt : var_decl TSEMICOLON
		   | array_decl TSEMICOLON
		   | cycle
		   | condition
		   | expr TSEMICOLON					{ $$ = new NExpressionStatement(*$1); }
		   | TRETURN expr TSEMICOLON			{ $$ = new NReturnStatement(*$2); }	
		   | TSEMICOLON							{}
		   ;

block : TLBRACE local_stmts TRBRACE   			{ $$ = $2; }
	  | local_stmt								{ $$ = new NBlock(); $$->statements.push_back($<stmt>1); }
	  ;

func_decl : type ident TLPAREN func_decl_args TRPAREN block
												{ $$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6); delete $4; }
		  ;

extern_decl : TEXTERN type ident TLPAREN func_decl_args TRPAREN
                								{ $$ = new NExternDeclaration(*$2, *$3, *$5); delete $5; }
            ;
								
global_var_decl : type ident 			 		{ $$ = new NGlobalVariableDeclaration(*$1, *$2); }
				;

var_decl : type ident 			 				{ $$ = new NVariableDeclaration(*$1, *$2); }
		 | type ident TEQUAL expr 				{ $$ = new NVariableDeclaration(*$1, *$2, $4); }
		 | type ident TEQUAL error 				{ $$ = new NVariableDeclaration(*$1, *$2); }
		 ;																 		

array_decl : type ident TLBRACKET TINTEGER TRBRACKET 				{ $$ = new NArrayDeclaration(*$1, *$2, atol($4->c_str())); }
		   | type ident TLBRACKET TRBRACKET TEQUAL TSTR			 	{ $$ = new NArrayDeclaration(*(new NIdentifier(*$1)), *$2, *$6); 
		   															  delete $1;}	
		   | type ident TLBRACKET TRBRACKET TEQUAL TLBRACE call_args TRBRACE 
			   														{ $$ = new NArrayDeclaration(*$1, *$2, *$7); }
		   ;

func_decl_args : /*blank*/  					{ $$ = new VariableList(); }
		 	   | var_decl 						{ $$ = new VariableList(); $$->push_back($<var_decl>1); }
		 	   | func_decl_args TCOMMA var_decl { $1->push_back($<var_decl>3); }
		 	   ;

ident : TIDENTIFIER 							{ $$ = new NIdentifier(*$1); delete $1; }
	  ;

numeric : TINTEGER 								{ $$ = new NInteger(atol($1->c_str())); delete $1; }
		| TDOUBLE 								{ $$ = new NDouble(atof($1->c_str())); delete $1; }
		| TMINUS TINTEGER						{ $$ = new NInteger(-atol($2->c_str())); delete $2; }
		| TMINUS TDOUBLE						{ $$ = new NDouble(-atof($2->c_str())); delete $2; }
		| TCHARACTER 							{ $$ = new NChar($1->front()); delete $1; }
		;
	
expr : ident TEQUAL expr 			 				{ $$ = new NAssignment(*$<ident>1, *$3); }
	 | ident TLPAREN call_args TRPAREN 			 	{ $$ = new NMethodCall(*$1, *$3); delete $3; }
	 | ident 										{ $<ident>$ = $1; }
	 | numeric
     | expr TMUL expr 								{ $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TDIV expr 								{ $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TPLUS expr 								{ $$ = new NBinaryOperator(*$1, $2, *$3); }
     | expr TPLUS error 							{ $$ = $1; }
     | expr TMINUS expr 							{ $$ = new NBinaryOperator(*$1, $2, *$3); }
 	 | expr comparison expr 						{ $$ = new NBinaryOperator(*$1, $2, *$3); }
     | TLPAREN expr TRPAREN 						{ $$ = $2; }
     | TLPAREN type TRPAREN TLPAREN expr TRPAREN	{ $$ = new NCastExpression(*$2, *$5); }
     | ident TLBRACKET expr	TRBRACKET				{ $$ = new NAccessToArray(*$1, *$3, NULL); }
     | ident TLBRACKET expr TRBRACKET TEQUAL expr	{ $$ = new NAccessToArray(*$1, *$3, $6); }  
	 ;
	
call_args : /*blank*/  							{ $$ = new ExpressionList(); }
		  | expr 								{ $$ = new ExpressionList(); $$->push_back($1); }
		  | call_args TCOMMA expr  				{ $1->push_back($3); }
		  ;

comparison : TCEQ 
		   | TCNE 
		   | TCLT 
		   | TCLE 
		   | TCGT 
		   | TCGE
		   ;

cycle : TWHILE TLPAREN expr TRPAREN block					{ $$ = new NWhileStatement(*$3, *$5); }
      | TFOR TLPAREN expr TSEMICOLON expr TSEMICOLON expr TRPAREN block 
		  													{ $$ = new NForStatement(*$3, *$5, *$7, *$9); }
      ; 

condition : TIF TLPAREN expr TRPAREN block TELSE block   	{ $$ = new NConditionStatement(*$3, *$5, *$7); }
		  | TIF TLPAREN expr TRPAREN block					{ $$ = new NConditionStatement(*$3, *$5, *(new NBlock())); }
		  ;

type : TINT  		{ $$ = new NIdentifier(*$1); delete $1; }
	 | TDBL  		{ $$ = new NIdentifier(*$1); delete $1; }
	 | TCHAR 		{ $$ = new NIdentifier(*$1); delete $1; }
	 | TVOID     	{ $$ = new NIdentifier(*$1); delete $1; }
	 | type TMUL 	{ $1->name += '*'; }
	 ;

%%
