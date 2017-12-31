
extern "C"
{
#include "csapp.h"
}
void echo(int connfd);
void *thread(void *connfd1)
{
   
    printf("received client request\n");
    Sleep(40);
    echo(*(int*)connfd1);
    Close(*(int*)connfd1);
}
