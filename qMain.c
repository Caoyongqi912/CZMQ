#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include <string.h> 
#include <zmq.h>
#include <assert.h>
#include <ctype.h>
#include <semaphore.h>
#include <pthread.h>

#define MAXSIZE 5
sem_t sem;

typedef char *QElemType;

 /*循环队列对的顺序存储结构*/
typedef struct
{
        char ais_data[MAXSIZE][250];  //队列中的二维数组元素 
        int front;    //头指针
        int rear;    //尾指针
}sqQueue;
 
/*初始化一个空队列*/
int InitQueue(sqQueue *Q)
{
        Q->front=0;        //结构体指针 
        Q->rear=0;
       return 1;
}
 
/* 将队列清空 */
int ClearQueue(sqQueue *Q)
{
        Q -> front = 0;
        Q -> rear = 0;
        return 1;
} 
 
/*返回队列当前长度*/
int QueueLength(const sqQueue *Q)
{
        return ((Q->rear - Q->front+ MAXSIZE)%MAXSIZE);
       /* 返回队列中元素的个数 */
}
 
/*循环队列入队操作  e为插入的元素*/
int EnQueue(sqQueue *Q,char* e)
{	
        if((Q->rear+1)%MAXSIZE==Q->front)  /*队列已满*/
        {
                                printf("================满================\n");

                sem_wait(&sem);
        }
       strcpy(Q->ais_data[Q->rear],e);    //将元素e赋值给队尾 
       Q->rear=(Q->rear+1)%MAXSIZE;       //如果指向最后一位则转回到数组头 
       return 1;
}
 
/*若队列不为空,则删除Q中队头元素,用e值返回*/ 
int DeQueue(sqQueue *Q,char *e)
{
       if (Q->front==Q->rear)    /*队列为空判断*/
       {
               sem_post(&sem);

       }
        strcpy(e, Q->ais_data[Q->front]);
        Q->front=(Q->front+1)%MAXSIZE;
        return 1;

}

void main(void){
    sqQueue Q;
    InitQueue(&Q);
    sem_init(&sem, 0, 0); //信号量初始化


    FILE *fp; 
    char StrLine[1024];     //每行最大读取的字符数
    if((fp = fopen("sug","r")) == NULL) //判断文件是否存在及可读
    { 
        printf("error!"); 
    } 

    while(1){
                    
        if (!feof(fp)) { 
            fgets(StrLine,sizeof(StrLine),fp);  //读取一行
            EnQueue(&Q,StrLine); //插入队列
     } else
    {
        fseek(fp,0L,SEEK_SET);
    }
    };


    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);

    printf("server Start .....\n");


    while (1) {

        char buffer[5];
        char value[100];
        int a ;
        memset(&value,0,strlen(value));    
        zmq_recv(responder,buffer,strlen(buffer),0);
        if (strcmp(buffer,"hello") == 0 ){
                DeQueue(&Q,value);
                zmq_send(responder,value,strlen(value),0);     
                a = QueueLength(&Q);
                printf("队列长度为%d\n",a);

        }
        else
        {
             zmq_send(responder,"NONE",4,0);
        }
    }
}
