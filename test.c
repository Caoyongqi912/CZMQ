

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int sharedi = 0;
void increse_num(void);

int main(){
    int ret;
    pthread_t thrd1, thrd2, thrd3;

    ret = pthread_create(&thrd1, NULL, (void *)ReadFile("sug"), NULL);
    //ret = pthread_create(&thrd2, NULL, (void *)increse_num, NULL);
    // ret = pthread_create(&thrd3, NULL, (void *)increse_num, NULL);

    pthread_join(thrd1, NULL);
    // pthread_join(thrd2, NULL);
    // pthread_join(thrd3, NULL);

    printf("sharedi = %d\n", sharedi);

    return 0;

}

void increse_num(void) {
    long i,tmp;
    for(i=0; i<=100000; i++) {
        tmp = sharedi;
        tmp = tmp + 1;
        sharedi = tmp;
    }
}




//读取文件
void ReadFile(char* filePath)
{
    char data[100];
    FILE *fp=fopen(filePath,"r");
    while(!feof(fp))
    {
        fscanf(fp,"%s",&data);
        printf("%s",data);
        
    }
    fclose(fp);
    // return true;
}