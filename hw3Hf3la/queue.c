//
// Created by student on 3/4/24.
//

#include <stdio.h>
#include "queue.h"
#include <stdlib.h>

struct Request{
    int connfd;
    struct timeval arrival;
    struct timeval dispatch;
};

struct Node{
    p_Request webRequest;
    struct Node* next;
};

struct Queue{

    RequestNode head;
    RequestNode tail;
    int size;
};


//////////////////////////////// Request Function ////////////////////////////////////

p_Request createRequest(int connfdSet , struct timeval arrivalSet){
    p_Request newRequest = (p_Request) malloc(sizeof(*newRequest));
    newRequest->arrival = arrivalSet;
    newRequest->connfd = connfdSet ;
    newRequest->dispatch = arrivalSet;

    return newRequest;
}

void setDispatch(RequestNode mynode ,struct timeval dispatchTime){
    if( (mynode == NULL) || (mynode->webRequest == NULL) ) return;
        mynode->webRequest->dispatch = dispatchTime - mynode->webRequest->arrival;
}


/////////////////////////////////// Node Functions /////////////////////////////////////////


RequestNode createNode(p_Request newRequest){
    RequestNode newNode = (RequestNode) malloc(sizeof(*newNode));
    if (newNode == NULL){
        return NULL;
    }
    newNode->next = NULL;
    newNode->webRequest = newRequest;
    return newNode;
}

int destroyNode(RequestNode toDestroy){
    if(toDestroy == NULL) return 0;
    if(toDestroy->webRequest != NULL) {
        free(toDestroy->webRequest);
    }
    free(toDestroy);
    return 0;
}

int getConnfd(RequestNode myNode){
    if(myNode == NULL) return -1;
    return myNode->webRequest->connfd;
}

struct timeval getArrival(RequestNode mynode){
    if(mynode == NULL || mynode->webRequest == NULL) return 0;
    return mynode->webRequest->arrival;
}
struct timeval getDispatch(RequestNode mynode){
    if(mynode == NULL || mynode->webRequest == NULL) return 0;
    return mynode->webRequest->dispatch;
}
/////////////////////////////////// Queue Functions /////////////////////////////////////////

RequestQueue createQueue(){
    RequestQueue new_queue = (RequestQueue) malloc(sizeof(*new_queue));
    if (new_queue == NULL){
        return NULL;
    }
    new_queue->size = 0;
    new_queue->head = NULL;
    new_queue->tail = NULL;
    return new_queue;
}

int enqueue(RequestQueue myQueue, int connfdSet , struct timeval arrivalSet){

    p_Request newRequest = createRequest(connfdSet, arrivalSet);
    if(newRequest == NULL){
        return -1;
    }

    RequestNode newNode = createNode(newRequest);
    if(newNode == NULL){
        return -1;
    }

    if(myQueue->head == NULL && myQueue->tail == NULL){
        myQueue->head = newNode;
        myQueue->tail = newNode;
        myQueue->size = 1;
        return 0;
    }

    myQueue->tail->next = newNode;
    myQueue->tail = newNode;
    myQueue->size++;
    return 0;

}


int DestroyQueue(RequestQueue myQueue){
    if(myQueue == NULL){
        return -1;
    }

    RequestNode currToDestroy = myQueue->head;
    while(currToDestroy != NULL){
        RequestNode tmp = currToDestroy->next;
        destroyNode(currToDestroy);
        currToDestroy = tmp;
    }
    free(myQueue);
}

RequestNode dequeue(RequestQueue myQueue){

    if(myQueue == NULL){
        return NULL;
    }

    if(myQueue->head == NULL){
        return NULL;
    }

    if(myQueue->size == 1){
        RequestNode toReturn = myQueue->head;
        myQueue->head=NULL;
        myQueue->size --;
        myQueue->tail=NULL;
        return toReturn;
    }
    else {
        RequestNode toReturn = myQueue->head;
        RequestNode newHead = myQueue->head->next;
        myQueue->head = newHead;
        myQueue->size--;
        return toReturn;
    }
}

int getSizeQueue(RequestQueue myQueue){
    if(myQueue == NULL) return -1;
    return myQueue->size;
}

/*
void dequeueByConnfd(RequestQueue myQueue , int connfd){
    if(myQueue->head == NULL) return;
    if(myQueue->size == 1){
        if(myQueue->head->webRequest->connfd == connfd){
            destroyNode(myQueue->head);
            myQueue->head = NULL;
            myQueue->tail = NULL;
            myQueue->size--;
            return;
        }
        else{
            return;
        }
    }
    RequestNode tmp1;
    if(myQueue->head->webRequest->connfd == connfd){
        tmp1 = myQueue->head->next;
        destroyNode(myQueue->head);
        myQueue->head = tmp1;
        myQueue->size--;
        return;
    }

    RequestNode currPrev = myQueue->head;
    RequestNode curr = myQueue->head->next;

    while(curr != NULL){
        if(connfd == curr->webRequest->connfd){
            currPrev->next = curr->next;
            destroyNode(curr);
            return;
        }
        currPrev = currPrev->next;
        curr = curr->next;
    }
}
*/

void dequeueByConnfd(RequestQueue myQueue, int connfd) {
    if (myQueue->head == NULL) return; // Empty queue

    RequestNode prev = NULL;
    RequestNode curr = myQueue->head;

    while (curr != NULL) {
        if (curr->webRequest->connfd == connfd) {
            if (prev == NULL) {
                // Dequeueing the head
                myQueue->head = curr->next;
                if (myQueue->head == NULL) {
                    // If head becomes NULL, update tail as well
                    myQueue->tail = NULL;
                }
            } else {
                prev->next = curr->next;
                if (curr->next == NULL) {
                    // If dequeuing the last node, update tail
                    myQueue->tail = prev;
                }
            }
            destroyNode(curr);
            myQueue->size--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}
