%{
#include <string>
#include <iostream>
#include "node.h"
#include "parser.hpp"


#define SAVE_TOKEN  yylval.string = new std::string(yytext, yyleng)
#define TOKEN(t)    (yylval.token = t)

int line = 1;

%}

%option noyywrap

%%

"extern"						return TOKEN(TEXTERN);				       
"return"    			        return TOKEN(TRETURN);

"while"							return TOKEN(TWHILE);
"for"							return TOKEN(TFOR);

"if"							return TOKEN(TIF);
"else"							return TOKEN(TELSE);

"int"							SAVE_TOKEN; return TINT;
"double"						SAVE_TOKEN; return TDBL;
"char"							SAVE_TOKEN; return TCHAR;
"void"							SAVE_TOKEN; return TVOID;

["].*["]						{
									SAVE_TOKEN; 
									yylval.string->erase(yylval.string->begin());
									yylval.string->erase(yylval.string->end() - 1);		 
									return TSTR;
								}
								
[a-zA-Z_][a-zA-Z0-9_]*  		SAVE_TOKEN; return TIDENTIFIER;
[0-9]+					        SAVE_TOKEN; return TINTEGER;
[0-9]+\.[0-9]* 			    	SAVE_TOKEN; return TDOUBLE;
['].[']							{
									SAVE_TOKEN; 
									yylval.string->erase(yylval.string->begin());
									yylval.string->erase(yylval.string->end() - 1);		 
									return TCHARACTER;
								}

"="						        return TOKEN(TEQUAL);
"=="				          	return TOKEN(TCEQ);
"!="			          		return TOKEN(TCNE);
"<"				          		return TOKEN(TCLT);
"<="	          				return TOKEN(TCLE);
">"				          		return TOKEN(TCGT);
">="					        return TOKEN(TCGE);
"&&"							return TOKEN(TCAND);
"||"							return TOKEN(TCOR);

"("	          					return TOKEN(TLPAREN);
")"					          	return TOKEN(TRPAREN);
"["								return TOKEN(TLBRACKET);
"]"								return TOKEN(TRBRACKET);
"{"								return TOKEN(TLBRACE);
"}"								return TOKEN(TRBRACE);
"#"								return TOKEN(TSHARP);
"~"								return TOKEN(TTILDE);

"."         					return TOKEN(TDOT);
","				          		return TOKEN(TCOMMA);

"+"				          		return TOKEN(TPLUS);
"-"		          				return TOKEN(TMINUS);
"*"		          				return TOKEN(TMUL);
"/"				          		return TOKEN(TDIV);
":"								return TOKEN(TCOLON);
";"								return TOKEN(TSEMICOLON);

[ \t]*							;

"\n"							line++;

^"#include ".+					;

.                       		std::cout << "Unknown token! " << yytext << std::endl; yyterminate();

%%
