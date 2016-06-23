



var next = new Array(1000);

function kmp_next(base){
	var i,j;
	i=0;
	j=-1;
	next[0]=-1;
	while(base[i]!=undefined){
		if(j==-1||(base[i]==base[j])){
			i++;
			j++;
			if(base[i]==base[j]){
				next[i]=next[j];
			}
			else {
				next[i]=j;
			}
		}
		else {
			j=next[j];
		}
	}
}

function kmp(b,base,pos){
	var i,j;
	i=pos-1;
	j=0;
	while(b[i]!=undefined&&base[j]!=undefined){
		if(b[i]==base[j]){
			i++;
			j++;
		}
		else {
			j=next[j];
			if(j==-1){
				i++;
				j++;
			}
		}
	}
	if(base[j]==undefined){
		return i-j+1;
	}
	else 	return -1;
}

function main(){
	var base="ab";
	var b="abbcacbacaabab";
	var pos=0;
	kmp_next(base);
	while(true){
		pos=kmp(b,base,pos+1);
		if(pos==-1)		break;
		console.log("%d\n",pos);
	}
}

