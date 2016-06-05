/* c2js flex flie */
Digit 	[0-9]
Letter 	[a-zA-Z]
Hex 	[a-fA-F0-9]
FS      (f|F|l|L)
E           [Ee][+-]?{Digit}+

%{
#include <iostream>
#include <fstream>
using namespace std;
#define YYSTYPE string
#include "c2js.tab.h"

using namespace std;

extern "C"
{
    void yyerror(const char* s);
    extern int yylex(void);
}

void showStr();
%}

%%
"//"            { cout<<"//: 		"; showStr();  return(COMMENT);	}
"char"          { cout<<"CHAR: 		"; showStr();  return(CHAR);	}
"double"        { cout<<"DOUBLE: 	"; showStr();  return(DOUBLE);	}
"float"         { cout<<"FLOAT: 	"; showStr();  return(FLOAT); 	}
"for"           { cout<<"FOR: 		"; showStr();  return(FOR);		}
"while"         { cout<<"WHILE: 	"; showStr();  return(WHILE);	}
"if"            { cout<<"IF: 		"; showStr();  return(IF);		}
"else"          { cout<<"ELSE:      "; showStr();  return(ELSE);    }
"int"           { cout<<"INT: 		"; showStr();  return(INT);		}
"#define"           { cout<<"DEFINE: 		"; showStr();  return(DEFINE);		}
"bool"           { cout<<"BOOL: 		"; showStr();  return(BOOL);		}
"return"        { cout<<"RETURN: 	"; showStr();  return(RETURN);	}
"void"          { cout<<"VOID: 		"; showStr();  return(VOID);	}
"#include"      { cout<<"INCLUDE: 	"; showStr();  return(INCLUDE); }

[a-zA-Z_][a-zA-Z0-9_]*  { cout<<"IDENTIFIER: 	"; showStr();  return(IDENTIFIER); }

{Digit}*"."{Digit}+({E})?{FS}?  { cout<<"CONST:	"; showStr(); return(CONST); }
{Digit}+"."{Digit}*({E})?{FS}?  { cout<<"CONST:	"; showStr(); return(CONST); }
{Digit}+              			{ cout<<"CONST:	"; showStr(); return(CONST); }

\"(\\.|[^\\"])*\"        { cout<<"STRING_LITERAL: "; showStr(); return(STRING_LITERAL); }
\'(\\.|[^\\'])*\'        { cout<<"STRING_LITERAL: "; showStr(); return(STRING_LITERAL); }

"\\n"			{ /* ignore enter key*/ }
[ \n\t\v\f] 	  { }
"++"            { cout<<"++:	"; showStr(); return(INC_OP);	}
"--"            { cout<<"--:	"; showStr(); return(DEC_OP); 	}
"<="            { cout<<"<=:	"; showStr(); return(LE_OP); 	}
">="            { cout<<">=:	"; showStr(); return(GE_OP); 	}
"=="            { cout<<"==:	"; showStr(); return(EQ_OP); 	}
"!="            { cout<<"!=:	"; showStr(); return(NE_OP); 	}
"&&"            { cout<<"&&:	"; showStr(); return(AND_OP);	}
"||"            { cout<<"||:	"; showStr(); return(OR_OP); 	}
"-"             { cout<<"-:		"; showStr(); return('-'); 		}
"+"             { cout<<"+:		"; showStr(); return('+'); 		}
"*"             { cout<<"*:		"; showStr(); return('*'); 		}
"/"             { cout<<"/:		"; showStr(); return('/'); 		}
"%"             { cout<<"%:		"; showStr(); return('%'); 		}
"\\"            { cout<<"\\:	"; showStr(); return('\\'); 	}
"<"             { cout<<"<:		"; showStr(); return('<'); 		}
"."             { cout<<".:		"; showStr(); return('.'); 		}
">"             { cout<<">:		"; showStr(); return('>'); 		}
","             { cout<<",:		"; showStr(); return(','); 		}
":"             { cout<<"::		"; showStr(); return(':'); 		}
";"             { cout<<";:		"; showStr(); return(';'); 		}
"="             { cout<<"=:		"; showStr(); return('='); 		}
"("             { cout<<"(:		"; showStr(); return('('); 		}
")"             { cout<<"):		"; showStr(); return(')'); 		}
"{"     		    { cout<<"{:		"; showStr(); return('{'); 		}
"}"      		    { cout<<"}:		"; showStr(); return('}'); 		}
"["      		    { cout<<"[:		"; showStr(); return('['); 		}
"]"      		    { cout<<"]:		"; showStr(); return(']'); 		}
"\""			      { cout<<"\":	"; showStr(); return('\"');		}

.               printf("Unknown : %c\n",yytext[0]);

%%

void showStr()
{
    yylval = yytext;
    cout << yytext <<endl;
}

int yywrap()
{
    return 1;
}
