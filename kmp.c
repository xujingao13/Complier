#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int next[1000];

void kmp_next(char base[])
{
    int i,j;
    i=0;
    j=-1;
    next[0]=-1;
    while(base[i]!='\0')
    {
        if(j==-1||(base[i]==base[j]))
        {
           i++;
           j++;
           if(base[i]==base[j])
           {
               next[i]=next[j];
           }
           else
           {
               next[i]=j;
           }
        }
        else
        {
            j=next[j];
        }
    }
}

int kmp(char b[],char base[], int pos)
{
     int i,j;
     i=pos-1;j=0;
     while(b[i]!='\0' && base[j]!='\0')
     {
        if(b[i]==base[j])
        {
            i++;j++;
        }
        else
        {
            j=next[j];
            if(j==-1)
            {
               i++;j++;
            }
        }
     }
     if(base[j]=='\0')
     {
         return i-j+1;
     }
     else
         return -1;
}


int main()
{
    char base[]="ab";
    char b[]="abbcacbacaabab";
    int pos=0;
    kmp_next(base);
    while(true){
		  pos = kmp(b,base,pos+1);
		  if(pos == -1)
			break;
		  printf("%d\n",pos);
	  }
}
