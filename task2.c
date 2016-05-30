#include <stdio.h>
#include <string.h>
#include "externc.h"
extern void printi(int val);

int main() {
	int n;
	int k;
	char input1[] = "abcdefgabdef";
	char input2[] = "ab";
	char noans[] = "False";
	char yesans[] = "True";
	int len1 = strlen(input1);
	int len2 = strlen(input2);
	int i;
	int j;
	int next[100];
	for (i = 0; i < 100; i = i + 1)
		next[i] = 0;
	for (i = len1; i >= 0; i = i - 1) {
		j = i+1;
		input1[j] = input1[i];
	}
	for (i = len2; i >= 0; i = i - 1) {
		j = i+1;
		input2[j] = input2[i];
	}
	i = 1;
	j = 0;
	while (i <= len2) {
		k = 0;
		if (j == 0) {
			k = k + 1;
		}
		if (input2[i] == input2[j]) {
			k = k + 1;
		}
		if (k > 0) {
			j = j + 1;
			i = i + 1;
			if (input2[i] == input2[j]) {
				next[i] = next[j];
			}
			else {
				next[i] = j;
			}
		}
		else {
			j = next[j];
		}
	}
	i = 1;
	j = 1;
	int r = -1;
	while (1==1) {
		k = 0;
		if (j == 0) {
			k = k + 1;
		}
		if (input1[i] == input2[j]) {
			k = k + 1;
		}
		if (k > 0) {
			j = j + 1;
			i = i + 1;
		}
		else {
			j = next[j];
		}
		if (j > len2) {
			r= i-j;
			printi(r);
			j = 1;
		}
		if (i > len1) {
			if (r == -1){
				printf(noans);
			}
			return 0;
		}
	}
	return 0;
}
