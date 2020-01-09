#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zmq.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>
#define ERR_OK 0
#define ERR_NO 1





//指针定义名称 
typedef char* QElemType;

//status 定义类型
typedef int Status;


//定义结构体
typedef struct QNode //节点结构
{
    QElemType data;
    struct QNode *next;
}QNode,*QueuePtr;


typedef struct Link_queue//队列的链表结构
{
QueuePtr front,rear; //队列的头、尾指针
}LinkQueue;


//入队操作
Status Enqueue(LinkQueue* Q,QElemType e)
{
    QueuePtr newNode = (QueuePtr)malloc(sizeof(QNode)); //新建队列节点

    size_t len = strlen(e);

    newNode->data = (QElemType)malloc(len+2*sizeof(char));    //为节点数据域分配内存

    //数据域循环赋值
    int i=0;
    for(i=0;i<len;i++)
    {
    newNode->data[i] = e[i];
    }
    newNode->data[len] = '\0';


    newNode->next = NULL;

    Q->rear->next = newNode; //队列的尾指针指向的当前节点的下一个位置，指向s
    Q->rear = newNode;    //队列的尾指针向后移动至新节点
    return ERR_OK;

}

//出队操作,使用后需要释放e的内存
Status DeQueue(LinkQueue* Q,QElemType* e)
{
        QueuePtr p;

        p = Q->front->next; //要出队的是头结点的下一个节点
        //*e = p->data;    //将要出队的节点数据赋值给e
        size_t len = strlen(p->data);

        *e=(QElemType)malloc(len+2*sizeof(char));


        int i=0;
        for(i=0;i<len;i++)
        {
        (*e)[i] = p->data[i];
        }
        (*e)[len]='\0';

        Q->front->next = p->next;

        if(Q->rear == p) //尾指针指向p说明队列空了
        {
        Q->rear = Q->front;
        }

        free(p->data);
        free(p);
        return ERR_OK;
}


//入口
int main()
{     
    QueuePtr s = (QueuePtr)malloc(sizeof(QNode));
    s->data = 0;
    s->next = NULL;
    LinkQueue linkqueue;
    linkqueue.front = s;
    linkqueue.rear = s;

    FILE *fp; 
    char StrLine[1024] = {0};     //每行最大读取的字符数
    if((fp = fopen("sug","r")) == NULL) //判断文件是否存在及可读
    { 
        printf("error!"); 
        return -1; 
    } 

    //while (1){
        while (!feof(fp)) 
        { 
            fgets(StrLine,sizeof(StrLine),fp);  //读取一行
            Enqueue(&linkqueue,StrLine);
        } 
    //}
    // fclose(fp);

    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);
    int rc = zmq_bind (responder, "tcp://*:5555");
    assert (rc == 0);

    printf("server Start .....\n");

    while (1) {
        char buffer [1024];
        QElemType value;
        zmq_recv(responder, buffer, strlen(buffer), 0);
        DeQueue(&linkqueue,&value);
        printf("发送%s\n",value);
        zmq_send(responder,value,strlen(value),0);
     }
    return 0;


}



