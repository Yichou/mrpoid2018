/*
 * msgqueue.h
 *
 *  Created on: 2013-6-13
 *      Author: Yichou
 */

#ifndef MSGQUEUE_H_
#define MSGQUEUE_H_

#include <pthread.h>

//#ifdef __cplusplus
//extern "C" {
//#endif


typedef struct Node * 	NODE;
typedef struct Queue * 	QUEUE;
typedef void * 			ELEMENT;

typedef void (*ELEMENT_HANDLER)(ELEMENT *e);

struct Queue {
	int blocked;
	NODE head;
	NODE end;

	pthread_cond_t  not_empty; 	//条件 空
	pthread_cond_t  timeout; 	//条件
	pthread_mutex_t mutex; 		//互斥锁
};

struct Node {
	long when;

	ELEMENT element;
	NODE next;
};


#define QUEUE_SIZE sizeof(struct Queue)
#define NODE_SIZE sizeof(struct Node)
#define NEW_NODE (NODE)malloc(NODE_SIZE);


QUEUE new_queue();
void delete_queue(QUEUE queue, ELEMENT_HANDLER handler);
void enqueue(QUEUE queue, ELEMENT e, long when);
ELEMENT dequeue(QUEUE queue);
void clqueue(QUEUE queue, ELEMENT_HANDLER handler);

//#ifdef __cplusplus
//}
//#endif

#endif /* MSGQUEUE_H_ */
