//
// Created by student on 3/4/24.
//

#ifndef OS_HW3_QUEUE_H
#define OS_HW3_QUEUE_H


typedef struct Queue* RequestQueue;
typedef struct Node* RequestNode;
typedef struct Request* p_Request;

RequestQueue createQueue();
int enqueue(RequestQueue myQueue, int connfdSet , struct timeval arrivalSet);
RequestNode createNode(p_Request newRequest);
p_Request createRequest(int connfdSet , struct timeval arrivalSet);
RequestNode dequeue(RequestQueue myQueue);
int destroyNode(RequestNode toDestroy);
int DestroyQueue(RequestQueue myQueue);
int getSizeQueue(RequestQueue myQueue);
//RequestNode dequeue(RequestQueue myQueue);
void dequeueByConnfd(RequestQueue myQueue , int connfd);
int getConnfd(RequestNode myNode);
void setDispatch(RequestNode mynode ,struct timeval dispatchTime);






#endif //OS_HW3_QUEUE_H
