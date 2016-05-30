#include <stdio.h>
#include <string.h>

int main() {
	int n;
	int k;
	char text[] = "abcdfdcba";
	char noans[] = "False";
	char yesans[] = "True";
	int i;
	int len = strlen(text);
	int lenh = len / 2;
	for (i = 0; i < lenh; i = i+1) {
		k = len - i - 1;
		if (text[i] != text[k]) {
			printf(noans);
			return 0;
		}
	}
	printf(yesans);
	return 0;
}
