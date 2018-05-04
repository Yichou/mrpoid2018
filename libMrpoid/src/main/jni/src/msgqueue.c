#include <msgqueue.h>

#include <time.h>
#include <lib/TimeUtils.h>


/**
 * 创建队列
 *
 * 头结点并不存储数据
 * 头结点->next 为第一个节点
 */
QUEUE new_queue()
{
	QUEUE q = (QUEUE)malloc(QUEUE_SIZE);

	q->head = NULL;
	q->end = NULL;
	q->blocked = 0;

	pthread_mutex_init(&(q->mutex), NULL);
	pthread_cond_init(&(q->not_empty), NULL);
	pthread_cond_init(&(q->timeout), NULL);

	return q;
}

/**
 * clear the queue
 */
void clqueue(QUEUE queue, ELEMENT_HANDLER handler)
{
	NODE p = queue->head;
	while(p) {
		NODE del = p;
		p = p->next;
		free(del);

		//how to deal with element? just free it!
		if(handler)
			handler(del->element);
	}
}

int is_empty(QUEUE queue)
{
	return (queue->head == NULL);
}

void delete_queue(QUEUE queue, ELEMENT_HANDLER handler)
{
	clqueue(queue, handler);

	pthread_mutex_lock(&(queue->mutex));

	pthread_cond_destroy(&(queue->not_empty));
	pthread_cond_destroy(&(queue->timeout));

	pthread_mutex_unlock(&(queue->mutex));
	pthread_mutex_destroy(&(queue->mutex));

	free(queue);
}

void enqueue(QUEUE queue, ELEMENT e, long when)
{
	int wakeup = 0;
	int timeout = 0;

	pthread_mutex_lock(&(queue->mutex));
	{
		NODE n0 = NEW_NODE;
		NODE end = queue->end;
		NODE head = queue->head;

		if(is_empty(queue)) //blocked by empty
			wakeup = 1;

		n0->element = e;
		n0->when = when;

		if(head == NULL) {//empty
			n0->next = NULL;
			queue->head = queue->end = n0;
		} else if(when < head->when) {//before the head
			n0->next = head;
			queue->head = n0;

			if(queue->blocked)//blocked by wait timeout
				timeout = 1;
		} else if(when >= end->when) {//after then end
			n0->next = NULL;
			end->next = n0;
			queue->end = n0;
		} else {//search place
			NODE p = queue->head;
			NODE p0 = p;

			while(p) {
				if(when < p->when)
					break;
				p0 = p;
				p = p->next;
			}

			n0->next = p0->next;
			p0->next = n0;
		}
	}
	pthread_mutex_unlock(&(queue->mutex));

	if (wakeup)
		pthread_cond_signal(&(queue->not_empty)); //唤醒第一个调用 pthread_cond_wait() 而进入睡眠的线程

	if(timeout)
		pthread_cond_signal(&(queue->timeout)); //weak up the timeout wait
}

ELEMENT dequeue(QUEUE queue)
{
	ELEMENT e;
	long now;
	NODE n0;

	pthread_mutex_lock(&(queue->mutex));
	{
		if(is_empty(queue)){ //阻塞等待消息可取
			/* pthread_cond_wait的执行过程如下:
    			1. 首先, 它unlock the mutex, then 挂起当前的线程.
    			2. 当被唤醒的时候, 它会lock the mutex.
    		*/
			pthread_cond_wait(&(queue->not_empty), &(queue->mutex));
		}

checktime:
		now = uptimems();
		n0 = queue->head;

		if(now < n0->when) {
			queue->blocked = 1;

			struct timespec t;
			pthread_cond_timedwait(&(queue->timeout), &(queue->mutex), ms2timespec(n0->when - now, &t));

			queue->blocked = 0;

			goto checktime; //re check time now and head node, since head node has been changed!
		}

		e = n0->element;
		queue->head = n0->next;
		if(!queue->head)
			queue->end = NULL;

		free(n0);
	}
	pthread_mutex_unlock(&(queue->mutex));

	return e;
}


