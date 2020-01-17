#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include <string.h> 
#include <pthread.h>
#include <semaphore.h>
#include <zmq.h>
#include <assert.h>
#define MAXSIZE 5
void readSugFlie(void);
void  Czmq(void);


sem_t sem;

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
                printf("===================队列已满\n");
                sem_wait(&sem);
        }
       strcpy(Q->ais_data[Q->rear],e);    //将元素e赋值给队尾 
       Q->rear=(Q->rear+1)%MAXSIZE;       //如果指向最后一位则转回到数组头 
       return 1;
}

/*若队列不为空,则删除Q中队头元素,用e值返回*/ 
void DeQueue(sqQueue *Q,char *e)
{
       if (Q->front==Q->rear)    /*队列为空判断*/
       {
               sem_post(&sem);
       }

        strcpy(e, Q->ais_data[Q->front]);
        Q->front=(Q->front+1)%MAXSIZE;

}





int main(int argc, char const *argv[])
{
    char *sug = "sug";
    char *browser = "browser";
    void *retval;
    int ret_thrd1,ret_thrd2;
    int tmp1, tmp2;
    pthread_t thread1, thread2;
    sem_init(&sem, 0, 0); //信号量初始化

    ret_thrd1 = pthread_create(&thread1, NULL, (void *)&readSugFlie,NULL);
    ret_thrd2 = pthread_create(&thread2, NULL,(void*)&Czmq,NULL);

    tmp1= pthread_join(thread1,&retval);
    tmp2= pthread_join(thread2,&retval);
 
    return 0;

}




void readSugFlie(void){
    sqQueue Q;
    InitQueue(&Q);


    FILE *fp; 
    char StrLine[1024];     //每行最大读取的字符数
    if((fp = fopen("sug","r")) == NULL) //判断文件是否存在及可读
    { 
        printf("error!"); 
    }
         while (1)
    { 
        if (!feof(fp))
        {
            fgets(StrLine,sizeof(StrLine),fp);  //读取一行
            printf("读取 %s",StrLine);
            EnQueue(&Q,StrLine);

        }
        else
        {
            fseek(fp,0L,SEEK_SET);
        }
    } ;
}


void Czmq(void){
    sqQueue Q;

    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);
    printf("Server Start .......\n");

    while (1) {

        char buffer[20];
        char value[1024];    
        zmq_recv(responder,buffer,strlen(buffer),0);
        printf("收到 %s,  长度%ld\n",buffer,strlen(buffer));
        if (strcmp(buffer,"hello ") == 0 ){
                DeQueue(&Q,value);
                printf("发送 %s \n" ,value);
                zmq_send(responder,value,strlen(value),0);     


        }
        else
        {
             zmq_send(responder,"NONE",4,0);
        }
    }

}