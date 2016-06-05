#include <stdio.h>
#include <string.h>

//test comment
int isPlalindrome(char str[], int len)
{
    int i;
    for(i = 0; i < len / 2; i++)
    {
        if (str[i] != str[len - 1 - i])
        {
            return 0;
        }
    }
    return 1;
}


int main()
{
    char str[] = "abcdedca";
    int len = strlen(str);
    if (isPlalindrome(str, len) == 1)
    {
        printf("True\n");
    }
    else
    {
        printf("False\n");
    }
    return 0;
}
