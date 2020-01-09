#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_LINE 1024


void ReadText(char *filename)
{
    char buf[MAX_LINE];  /*缓冲区*/
    FILE *fp;            /*文件指针*/
    int len;             /*行字符个数*/
    if((fp = fopen(filename,"r")) == NULL)
    {
    perror("fail to read");
    exit (1) ;
    }
    while(fgets(buf,MAX_LINE,fp) != NULL)
    {
    len = strlen(buf);
    buf[len-1] = '\0';  /*去掉换行符*/
    printf("%s %d \n",buf,len - 1);
    }
}

int main()
{
    ReadText("sug");
    return 0;
}