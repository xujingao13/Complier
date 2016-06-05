#include <stdio.h>

int istack[500];
char cstack[500];
int topi;
int topc;

int popi(){
  int r;
  topi=topi-1;
  r=istack[topi];
  return r;
}

char popc(){
  char r;
  topc=topc-1;
  r=cstack[topc];
  return r;
}

void pushi(int i){
  istack[topi]=i;
  topi=topi+1;
}

void pushc(char c){
  cstack[topc]=c;
  topc=topc+1;
}

int main(){
  topi=0;
  topc=0;
  pushi(1);
  pushi(2);
  pushi(3);
  int i;
  i=popi();
  printf("%d\n", i);
  i=popi();
  printf("%d\n", i);
  i=popi();
  printf("%d\n", i);

  pushc('a');
  pushc('b');
  pushc('c');
  char c;
  c=popc();
  printf("%c\n", c);
  c=popc();
  printf("%c\n", c);
  c=popc();
  printf("%c\n", c);
  return 0;

}
