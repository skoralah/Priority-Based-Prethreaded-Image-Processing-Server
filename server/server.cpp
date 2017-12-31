extern "C"
{
#include "csapp.h"
}

#define QUEUESIZE 6
#define DELAY 3000

pthread_t workerthread1;
pthread_t workerthread2;
pthread_t workerthread3;

pthread_barrier_t mybarrier;
void echo(int connfd);
void *thread(void *);

//define queue structure
typedef struct {
	int buf[QUEUESIZE];
	long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notempty;
} queue;

//declare queue functins
queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, int in);
void queueDel (queue *q, int *out);
void millisleep(int milliseconds);

void *worker(void *);

int main(int argc, char **argv) 
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    int bytesReceived = 0;
    char recvBuff[256];
    pthread_t t1;
    if (argc != 2) 
    {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
    }
    port = atoi(argv[1]);
    queue *fifo;
    fifo = queueInit ();
	if (fifo ==  NULL) {
		fprintf (stderr, "main: Queue Init failed.\n");
		exit (1);
	}
	struct sched_param my_param;
	pthread_attr_t workerthread1_attr;
	pthread_attr_t workerthread2_attr;
	pthread_attr_t workerthread3_attr;
	int i, min_priority=1, policy;
	
	/* MAIN-THREAD WITH HIGH PRIORITY */
	my_param.sched_priority = sched_get_priority_min(SCHED_FIFO);
	min_priority=my_param.sched_priority;
	my_param.sched_priority=my_param.sched_priority+1;

	pthread_setschedparam(pthread_self(), SCHED_RR, &my_param);
	pthread_getschedparam (pthread_self(), &policy, &my_param);
	printf("Manager thread priority=%d\n",my_param.sched_priority);


	/* SCHEDULING POLICY AND PRIORITY FOR OTHER THREADS */
	pthread_attr_init(&workerthread1_attr);
        pthread_attr_setinheritsched(&workerthread1_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&workerthread1_attr, SCHED_FIFO);

	pthread_attr_init(&workerthread2_attr);
        pthread_attr_setinheritsched(&workerthread2_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&workerthread2_attr, SCHED_FIFO);

	pthread_attr_init(&workerthread3_attr);
        pthread_attr_setinheritsched(&workerthread3_attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&workerthread3_attr, SCHED_FIFO);
	
	
	

	my_param.sched_priority = min_priority;
	pthread_attr_setschedparam(&workerthread1_attr, &my_param);
	my_param.sched_priority = min_priority;
	pthread_attr_setschedparam(&workerthread2_attr, &my_param);
	my_param.sched_priority = min_priority;
	pthread_attr_setschedparam(&workerthread3_attr, &my_param);
	
    
	Pthread_create (&workerthread1, &workerthread1_attr, worker, fifo);
	Pthread_create (&workerthread2, &workerthread2_attr, worker, fifo);
	Pthread_create (&workerthread3, &workerthread3_attr, worker, fifo);
	
	
	listenfd = Open_listenfd(port);
	clientlen = sizeof(clientaddr);
	while (1) 
	{
		
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		printf("New Client Arrived \n");
		pthread_mutex_lock (fifo->mut);
		queueAdd (fifo, connfd);
		printf("Manager added client [%d]\n",connfd);
		while (fifo->full) {
			printf ("Buffer is full.\n");
			pthread_cond_wait (fifo->notFull, fifo->mut);
		}
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notempty);
    }
}
/* $end echoserverimain */

void *worker(void *fileDescriptor){
	queue *fifo;
	fifo=(queue *)fileDescriptor;
	int fd,d;
	pthread_t thread_id = pthread_self();
	struct sched_param param;
	int priority, policy, ret;
	ret = pthread_getschedparam (thread_id, &policy, &param);
	 priority = param.sched_priority;	
	printf("Worker thread priority=%d \n", priority);
	while(1){
		pthread_mutex_lock (fifo->mut);
		while (fifo->empty) {
			printf ("Worker queue is empty.\n");
			pthread_cond_wait (fifo->notempty, fifo->mut);
		}
		queueDel (fifo, &fd);
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notFull);
		printf ("Worker received client [%d]\n", fd);
		for(d=0;d<DELAY;d++){
			millisleep(10);
		}
		echo(fd);
		printf("worker served client [%d] \n",fd-3);
		Close(fd);
		millisleep(200);
	}

}


//queue function definitions
queue *queueInit (void)
{
	queue *q;

	q = (queue *)malloc (sizeof (queue));
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->head = 0;
	q->tail = 0;
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	q->notempty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notempty, NULL);
	
	return (q);
}

void queueDelete (queue *q)
{
	pthread_mutex_destroy (q->mut);
	free (q->mut);	
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	pthread_cond_destroy (q->notempty);
	free (q->notempty);
	free (q);
}

void queueAdd (queue *q, int in)
{
	q->buf[q->tail] = in;
	q->tail++;
	if (q->tail == QUEUESIZE)
		q->tail = 0;
	if (q->tail == q->head)
		q->full = 1;
		q->empty = 0;

	return;
}

void queueDel (queue *q, int *out)
{
	*out = q->buf[q->head];

	q->head++;
	if (q->head == QUEUESIZE)
		q->head = 0;
	if (q->head == q->tail)
		q->empty = 1;
	q->full = 0;

	return;
}

void millisleep(int milliseconds)
{
      usleep(milliseconds * 1000);
}

