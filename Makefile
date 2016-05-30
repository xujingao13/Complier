all: parser

OBJS = parser.o  \
       codegen.o \
       main.o    \
       tokens.o  \
       corefn.o  \
	   externc.o  \

LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cppflags` -std=c++11
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ldl -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

clean:
	$(RM) -rf parser.cpp parser.hpp parser tokens.cpp $(OBJS) out.ll prog.s prog.o prog
	$(RM) -rf task1.o task1 task2.o task2 task3.o task3

parser.cpp: parser.y
	bison -d -o $@ $^
	
parser.hpp: parser.cpp

tokens.cpp: tokens.l parser.hpp
	flex -o $@ $^

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<


parser: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

exec: 
	llc -o prog.s out.ll
	as -o prog.o prog.s
	gcc -o prog prog.o native.o

task1: parser task1.c
	cat task1.c | ./parser

task2: parser task2.c
	cat task2.c | ./parser

