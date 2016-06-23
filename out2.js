
var istack = new Array(500);

var cstack = new Array(500);

var poly_exp = new Array(500);

var topi=0;

var topc=0;

function isint(c){
	if('0'<=c&&c<='9'){
		return 1;
	}
	return 0;
}

function getType(c){
	if(c=='+'||c=='-'){
		return 1;
	}
	if(c=='*'||c=='/'){
		return 2;
	}
	else {
		return 0;
	}
}

function getlength(str){
	var len=0;
	var i=0;
	for(i=0;str[i]!=undefined;i++){
		len++;
	}
	return len;
}

function popi(){
	var r;
	topi=topi-1;
	r=istack[topi];
	return r;
}

function popc(){
	var r;
	topc=topc-1;
	r=cstack[topc];
	return r;
}

function ctop(){
	var r;
	r=cstack[topc-1];
	return r;
}

function itop(){
	var r;
	r=istack[topi-1];
	return r;
}

function pushi(i){
	istack[topi]=i;
	topi=topi+1;
}

function pushc(c){
	cstack[topc]=c;
	topc=topc+1;
}

function empytc(){
	if(topc==0){
		return 1;
	}
	return 0;
}

function toBack(str){
	var stillint=0;
	var str_len=getlength(str);
	var num=0;
	var i=0;
	for(i=0;i<str_len;i++){
		var c=str[i];
		if(isint(c)){
			if(stillint){
				if((i+1)!=str_len){
					if(isint(str[i+1])){
						poly_exp[num]=c;
						num++;
					}
					else {
						stillint=0;
						poly_exp[num]=c;
						num++;
						poly_exp[num]='~';
						num++;
					}
				}
				else {
					poly_exp[num]=c;
					num++;
				}
			}
			else {
				poly_exp[num]=c;
				num++;
				stillint=1;
			}
		}
		else {
			if(stillint){
				poly_exp[num]='~';
				num++;
				stillint=0;
			}
			if(c=='('){
				pushc(c);
			}
			else {
				if(c==')'){
					while(ctop()!='('){
						poly_exp[num]=ctop();
						num++;
						popc();
					}
					popc();
				}
				else {
					if(empytc()){
						pushc(c);
					}
					else {
						if(getType(c)>getType(ctop())){
							pushc(c);
						}
						else {
							while((1-empytc())&&getType(c)<=getType(ctop())){
								poly_exp[num]=popc();
								num++;
							}
							pushc(c);
						}
					}
				}
			}
		}
	}
	while(empytc()==0){
		poly_exp[num]=ctop();
		num++;
		popc();
	}
}

function toResult(){
	var val=0;
	var i=0;
	var len=getlength(poly_exp);
	var result=0;
	for(i=0;i<len;i++){
		var c=poly_exp[i];
		if(isint(c)){
			val=val*10+(c-'0');
		}
		if(c=='~'){
			pushi(val);
			val=0;
		}
		if(isint(c)&&(1-isint(poly_exp[i+1]))&&poly_exp[i+1]!='~'){
			pushi(val);
			val=0;
		}
		if(c=='+'){
			var x=popi();
			var y=popi();
			pushi(x+y);
		}
		if(c=='-'){
			var x=popi();
			var y=popi();
			pushi(y-x);
		}
		if(c=='*'){
			var x=popi();
			var y=popi();
			pushi(y*x);
		}
		if(c=='/'){
			var x=popi();
			var y=popi();
			pushi(y/x);
		}
	}
	result=popi();
	return result;
}

function main(){
	var str="1-(3+2)*3+4/5";
	var result=0;
	toBack(str);
	result=toResult();
	console.log("%d\n",result);
	return 0;
}

