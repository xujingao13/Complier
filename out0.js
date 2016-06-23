

//test comment
function isPlalindrome(str,len){
	var i;
	for(i=0;i<len/2;i++){
		if(str[i]!=str[len-1-i]){
			return 0;
		}
	}
	return 1;
}

function main(){
	var str="abcdedca";
	var len=str.length;
	if(isPlalindrome(str,len)==1){
		console.log("True\n");
	}
	else {
		console.log("False\n");
	}
	return 0;
}

