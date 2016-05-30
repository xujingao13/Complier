LEX=flex
YACC=bison
CC=g++
OBJECT=c2js

$(OBJECT): lex.yy.o  c2js.tab.o
	$(CC) lex.yy.o c2js.tab.o -o $(OBJECT)

lex.yy.o: lex.yy.c  c2js.tab.h
	$(CC) -c lex.yy.c

yacc.tab.o: c2js.tab.c
	$(CC) -c c2js.tab.c

yacc.tab.c  c2js.tab.h: c2js.y
	$(YACC) -d c2js.y

lex.yy.c: c2js.l
	$(LEX) c2js.l

clean:
	rm -f c2js.tab.c
	rm -f c2js.tab.h
	rm -f c2js.tab.o
	rm -f lex.yy.c
	rm -f lex.yy.o
	rm -f $(OBJECT)
	rm -f out.js
