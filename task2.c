#include <stdio.h>

int istack[500];
char cstack[500];
int topi;
int topc;

int isint(char c){
    if('0' <= c && c <= '9'){
        return 1;
    }
    return 0;
}
int getType(char c){
    if(c == '+' || c == '-'){
        return 1;
    }
    if(c == '*' || c == '/'){
        return 2;
    }
    else{
        return 0;
    }
}

int getlength(char str[]){
    int len = 0;
    for(int i = 0; str[i] != '\0'; i++){
        len++;
    }
    return len;
}
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

char ctop(){
    char r;
    r = cstack[topc-1];
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

int empytc(){
    if(topc == 0){
        return 1;
    }
    return 0;
}

void toBack(char str[], char exp[]){
    int stillint = 0;
    int str_len = getlength(str);
    int num = 0;
    int i = 0;
    for(i = 0; i < str_len; i++){
        char c = str[i];
        if(isint(c)){
            if(stillint){
                if((i+1) != str_len){
                    if(isint(str[i+1])){
                        exp[num] = c;
                        num++;
                    }
                    else{
                        stillint = 0;
                        exp[num] = c;
                        num++;
                        exp[num] = '~';
                        num++;
                    }
                }
                else{
                    exp[num] = c;
                    num++;
                }
            }
            else{
                exp[num] = c;
                num++;
                stillint = 1;
            }
        }
        else{
            if(stillint){
                exp[num] = '~';
                num++;
                stillint = 0;
            }
            if(c == '('){
                pushc(c);
            }else{
                if(c == ')'){
                    while(ctop() != '('){
                        exp[num] = ctop();
                        num++;
                        popc();
                    }
                    popc();
                }else{
                    if(empytc()){
                        pushc(c);
                    }else{
                        if(getType(c) > getType(ctop())){
                            pushc(c);
                        }else{
                            while(!empytc() && getType(c) <= getType(ctop())){
                                exp[num] = ctop();
                                num++;
                            }
                            pushc(c);
                        }
                    }
                }
            }
        }
    }
    while(!empytc()){
        exp[num] = ctop();
        num++;
        popc();
    }
}

int toResult(char exp[]){
    return 0;
}
int main(){
    char str[] = "1+2*3";
    char exp[100] = {'\0'};
    toBack(str, exp);
    printf("%d\n", getlength(exp));
    printf("%s\n", exp);
    return 0;
}
