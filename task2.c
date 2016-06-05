#include <stdio.h>

int istack[500];
char cstack[500];
char poly_exp[500];
int topi = 0;
int topc = 0;

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
    int i = 0;
    for(i = 0; str[i] != '\0'; i++){
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

int itop(){
    int r;
    r = istack[topi-1];
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

void toBack(char str[]){
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
                        poly_exp[num] = c;
                        num++;
                    }
                    else{
                        stillint = 0;
                        poly_exp[num] = c;
                        num++;
                        poly_exp[num] = '~';
                        num++;
                    }
                }
                else{
                    poly_exp[num] = c;
                    num++;
                }
            }
            else{
                poly_exp[num] = c;
                num++;
                stillint = 1;
            }
        }
        else{
            if(stillint){
                poly_exp[num] = '~';
                num++;
                stillint = 0;
            }
            if(c == '('){
                pushc(c);
            }else{
                if(c == ')'){
                    while(ctop() != '('){
                        poly_exp[num] = ctop();
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
                            while((1 - empytc()) && getType(c) <= getType(ctop())){
                                poly_exp[num] = popc();
                                num++;
                            }
                            pushc(c);
                        }
                    }
                }
            }
        }
    }
    while(empytc() == 0){
        poly_exp[num] = ctop();
        num++;
        popc();
    }
}

int toResult(){
    int val = 0;
    int i = 0;
    int len = getlength(poly_exp);
    int result = 0;
    for(i = 0; i < len; i++){
        char c = poly_exp[i];
        if(isint(c)){
            val = val*10+(c-'0');
        }
        if(c == '~'){
            pushi(val);
            val = 0;
        }
        if(isint(c) && (1-isint(poly_exp[i+1])) && poly_exp[i+1] != '~'){
            pushi(val);
            val = 0;
        }
        if(c == '+'){
            int x = popi();
            int y = popi();
            pushi(x+y);
        }
        if(c == '-'){
            int x = popi();
            int y = popi();
            pushi(y-x);
        }
        if(c == '*'){
            int x = popi();
            int y = popi();
            pushi(y*x);
        }
        if(c == '/'){
            int x = popi();
            int y = popi();
            pushi(y/x);
        }
    }
    result = popi();
    return result;
}
int main(){
    char str[] = "1*(3+2)*3";
    int result = 0;
    toBack(str);
    result = toResult();
    printf("%d\n", result);
    return 0;
}
