#include "segel.h"
#include "request.h"
#include "queue.h"


RequestQueue WaitReqQueue = NULL;
RequestQueue BusyReqQueue = NULL;

pthread_cond_t notEmpty;
pthread_cond_t fullQueue;
pthread_mutex_t lock;

void* service(void* arg);


// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
void getargs(int *port, int argc, char *argv[], int* threads_num , int* queue_size)
{
    if (argc < 5) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *threads_num = atoi(argv[2]);
    *queue_size = atoi(argv[3]);

}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    int threads_num = 0;
    int queue_size = 0;

    getargs(&port, argc, argv,&threads_num , &queue_size);

    // 
    // HW3: Create some threads...
    //

    pthread_cond_init(&notEmpty , NULL);
    pthread_cond_init(&fullQueue , NULL);
    pthread_mutex_init(&lock , NULL);


    // Create the requests queue
    WaitReqQueue = createQueue();
    BusyReqQueue = createQueue();

    // Create the workers pool
    int* workersId = (int*) malloc(threads_num);
    for(int i=0 ; i<threads_num ; i++) workersId[i] = i ;
    pthread_t* workers = (pthread_t*) malloc(threads_num*sizeof(*workers));
    for(int i=0 ; i<threads_num; i++){
        pthread_create(&workers[i] , NULL , service , (void *) &workersId[i]);
    }

    listenfd = Open_listenfd(port);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        // adding new request to the queue
        struct timeval arrive;
        gettimeofday(&arrive);
        pthread_mutex_lock(&lock);

        if(strcmp(argv[4],"dt") == 0){
            if(getSizeQueue(WaitReqQueue) + getSizeQueue(BusyReqQueue) >= queue_size) {
                Close(connfd);
                pthread_mutex_unlock(&lock);
                continue;
            }
        }

        else if(strcmp(argv[4], "block") == 0) {
            while (getSizeQueue(WaitReqQueue) + getSizeQueue(BusyReqQueue) >= queue_size) {
                pthread_cond_wait(&fullQueue, &lock);
            }
        }
        else if(strcmp(argv[4],"dh") == 0){
            if(getSizeQueue(WaitReqQueue) + getSizeQueue(BusyReqQueue) >= queue_size) {
                RequestNode toClose = dequeue(WaitReqQueue);
                if(toClose == NULL){
                    Close(connfd);
                    pthread_mutex_unlock(&lock);
                    continue;
                }
                Close(getConnfd(toClose));
            }
        }
        enqueue(WaitReqQueue , connfd , arrive);
        pthread_cond_signal(&notEmpty);
        pthread_mutex_unlock(&lock);

        //
        // HW3: In general, don't handle the request in the main thread.
        // Save the relevant info in a buffer and have one of the worker threads
        // do the work.
        //

        //Close(connfd);
    }
}


void* service(void* arg){

        threads_stats myThread;
        myThread->id = *arg;
        myThread->dynm_req = 0;
        myThread->stat_req = 0;
        myThread->total_req = 0;

        /////////// Removing from waiting queue and inserting it to busy queue
        while(1) {
            pthread_mutex_lock(&lock);
            while (getSizeQueue(WaitReqQueue) == 0) {
                pthread_cond_wait(&notEmpty, &lock);
            }
            RequestNode waitToBusy = dequeue(WaitReqQueue);
            struct timeval dispatch;
            gettimeofday(&dispatch);
            setDispatch(waitToBusy ,dispatch);
            int connfd = getConnfd(waitToBusy);
            enqueue(BusyReqQueue, connfd, waitToBusy);
            pthread_mutex_unlock(&lock);

            ///////////// Handling the request
            requestHandle(connfd , myThread);

            ////// Removing from busy queue
            pthread_mutex_lock(&lock);
            dequeueByConnfd(BusyReqQueue, connfd);
            pthread_cond_signal(&fullQueue);
            Close(connfd);
            pthread_mutex_unlock(&lock);
        }
}








 
