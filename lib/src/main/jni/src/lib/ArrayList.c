/** arraylist.c implement of arraylist.h
 ** 
 ** 单向可变长度线性链表 工具
 **
 ** 创建日期：2012/2/16
 ** 最后修改日期：2012/2/17
 **/
#if 1//def WIN32
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#else
#include "mrc_base.h"
#endif

#include "arraylist.h"



PARRAYLIST AL_create(void) {
	PARRAYLIST pal;

	pal = malloc(sizeof(ARRAYLIST));
	memset(pal, 0, sizeof(ARRAYLIST));
	pal->curIndex = -1;

	return pal;
}

void AL_add(PARRAYLIST pal, long int data) {
	PNODE p;

	if(!pal) return;

	p = malloc(sizeof(NODE));
	memset(p, 0, sizeof(NODE));

	p->data = data;

	if(pal->size == 0){
		pal->cur = pal->end = pal->head = p;
		//注意这个赋值
		pal->curIndex = 0;
	}else{
		pal->end->next = p;
		p->prev = pal->end;
		pal->end = p;		
	}

	pal->size++;
}

//添加并按名称排序
#if 1
//节点tmp，前插入节点pf
static void FL_insertBefore(PNODE dst, PNODE p) {
	dst->prev->next = p;
	p->prev = dst->prev;
	p->next = dst;
	dst->prev = p;
}

void AL_add_sort(PARRAYLIST pal, long int data, AL_Comparator c) {
	PNODE tmp, pf;

	if(!pal) return;

	//第一个节点
	if(!c || pal->size==0) 
	{
		AL_add(pal, data);
		return;	//只有一个文件不排序
	}

	pf = malloc(sizeof(NODE));
	memset(pf, 0, sizeof(NODE));
	pf->data = data;

	//排序插入
	tmp = pal->head;
	while(tmp)
	{
		if(tmp->next && c(data, tmp->data) < 0){
			FL_insertBefore(tmp->next, pf);

			//找到了插入位置，则直接跳出循环
			break;
		}


//NEXT:	//跳到下一个
		/**
		 * 当移动到最后一个节点还是不符合插入条件则他就是最后一个，
		 * 这里跳出循环tmp就是NULL了，该节点插在最后节点 listData.pEnd 后即可
		 */
		tmp = tmp->next;
	}

	//检测 tmp，如果是NULL则说明没插入，则插在最后
	if(tmp == NULL)
	{
		pal->end->next = pf;
		pf->prev = pal->end;
		pal->end = pf;	
	}

	pal->size++;
}
#endif

long int AL_remove(PARRAYLIST pal, int index){
	PNODE del, p;
	long int data;

	if(!pal || pal->size == 0) 
		return 0;

	if(index == 0){
		del = pal->head;
		pal->head = del->next;
		pal->head->prev = NULL;
	}else if(index == pal->size-1){
		del = pal->end;
		pal->end = del->prev;
		pal->end->next = NULL;	
	}else{
		int i;

		p = pal->head;
		for(i=0; i<index-1; i++) p = p->next;
		del = p->next;
		del->prev->next = del->next;
		del->next->prev = del->prev;
	}

	data = del->data;
	free(del);
	pal->size--;

	if(index == pal->curIndex){
		pal->curIndex = -1;
		pal->cur = NULL;
	}

	return data;
}

static
PNODE _AL_get(PARRAYLIST pal, int index){
	int i;
	PNODE p;

	if(!pal || index > pal->size-1) 
		return NULL;

	if(index == pal->curIndex) 
		return pal->cur;

	p = index<pal->curIndex? pal->head : pal->cur;
	i = index<pal->curIndex? 0 : pal->curIndex;
	for(; i<index; i++)
		p = p->next;

	pal->cur = p;
	pal->curIndex = index;

	return p;
}

long int AL_get(PARRAYLIST pal, int index){
	PNODE p = _AL_get(pal, index);

	return (p? p->data : 0);
}

static
PNODE _AL_get2(PARRAYLIST pal, int index)	//序号从1开始
{
	int i;
	PNODE tmp = pal->head;

	if(!pal || index > pal->size-1 || index < 0) 
		return 0;

	//pCur未被初始化，从开头找
	if(!pal->cur)
	{
		for(i=0; i<index; i++) {
			//if(!tmp) {
			//	mrc_printf("arraylist: NULL nodal at index %d", i);
			//	return 0;
			//}
			tmp = tmp->next;
		}
		pal->cur = tmp;
		pal->curIndex = i;
	}else	//从 pCur 位置开始搜寻
	{
		/*--20111022添加-- 判断要找的index距哪头最近
		 *开头、结尾、当前点
		 */
		int d0, d1;

		while(pal->curIndex != index)
		{
			if(pal->curIndex < index)	//前进
			{
				d0 = pal->size-1 - index;	//距尾节点
				d1 = index - pal->curIndex;

				if(d0 < d1){	//距尾节点更近，从尾节点找
					pal->curIndex = pal->size-1;
					pal->cur = pal->end;
					continue;
				}

				pal->curIndex++;
				pal->cur = pal->cur->next;
			}else
			{	//后退
				d0 = index;	//距开头
				d1 = pal->curIndex - index;	//距当前

				if(d0 < d1){	//距开头更近，从开头找
					pal->curIndex = 0;
					pal->cur = pal->head;
					continue;
				}

				pal->curIndex--;
				pal->cur = pal->cur->prev;
			}
		}

		tmp = pal->cur;
	}
	
	return tmp;
}

long int AL_get2(PARRAYLIST pal, int index){
	PNODE p = _AL_get2(pal, index);

	return (p? p->data : 0);
}

void AL_removeAll(PARRAYLIST pal, AL_removeNodeCallBack cb){
	PNODE p, p2;

	if(!pal) return;

	p = pal->head;
	while(p){
		p2 = p->next;
		//...
		if(cb) cb(p->data);
		free(p);
		p = p2;
	}
	
	memset(pal, 0, sizeof(ARRAYLIST));
	pal->curIndex = -1;
}

void AL_destroy(PARRAYLIST pal, AL_removeNodeCallBack cb){
	if(!pal) return;
	
	AL_removeAll(pal, cb);
	free(pal);
}

int AL_size(PARRAYLIST pal){
	if(!pal) return -1;
	return pal->size;
}

boolean AL_isEmpty(PARRAYLIST pal){
	if(!pal) return true;
	return (0 == pal->size);
}

void AL_push(PARRAYLIST pal, long int data){
	PNODE p;

	if(!pal) return;

	p = malloc(sizeof(NODE));
	memset(p, 0, sizeof(NODE));

	p->data = data;

	if(pal->size == 0){
		pal->cur = pal->end = pal->head = p;
		//注意这个赋值
		pal->curIndex = 0;
	}else{
		p->next = pal->head;
		pal->head->prev = p;
		pal->head = p;
	}

	pal->size++;
}

void AL_freeNode(long int data){
	if(data) free((void*)data);
}

void AL_moveNodeTo(PARRAYLIST pal, int index, int to){
	PNODE p, p1;

	if(!pal || index<0 || index>pal->size-1 || to<0 || to>pal->size-1 || index==to) 
		return;

	p = _AL_get2(pal, index);
	p1 = _AL_get2(pal, to);

	if(!p || !p1) return;

	if(p->next) 
		p->next->prev = p->prev;
	else //尾节点
		pal->end = p->prev;

	if(p->prev) 
		p->prev->next = p->next;
	else //头节点
		pal->head = p->next;

	//在p1前插入
	if(!p1->prev)
		pal->head = p;

	p->next = p1;
	p->prev = p1->prev;
	p1->prev = p;

	pal->cur = pal->head;
	pal->curIndex = 0;
}

void AL_sort(PARRAYLIST pal, AL_Comparator c)
{
	int ret;
	PNODE p, p1, p2, p3, p4;
	NODE h, t;

	if(!pal || !c || pal->size<=0)  //出错的情况
		return;

	h.next = pal->head;	// h 保存头结点

	p = pal->end->prev; //p 指向倒数第二个

	p = p->next = &t;	//链表的尾节点可能被交换到链表中间去，这里使用了一个固定的节点t
	while(p != h.next)	//此时P为最后一个自加节点
	{
		p3 = &h;		//自加头节点
		p1 = p3->next;	//待排序头节点
		p2 = p1->next;  //待排序第二个节点

		while(p2 != p)  //p指向的是已就序的节点
		{
			ret = c(p1->data, p2->data); //得出比较结果

			if(ret > 0)	//避免在双层循环内进行特殊情况的判断，因为这里的时间复杂度是O(n^2)
			{
				p1->next = p2->next;	//交换顺序
				p2->next = p1;
				p3->next = p2;

				p3 = p2;		//由于加了表头和表尾，整个交换过程更简洁
				p2 = p1->next;
			}else { //这里是 <= 情况
				p3 = p1;
				p1 = p2;
				p2 = p2->next;	//p2始终是排序的三个结点的最后一个 p3->p1->p2
			}
		}

		p = p1;	//p1此时是最后一个节点
	}

	while(p->next != &t){
		p4 = p;
		p = p->next;  //虽然这里复杂度是O(n)，但是远比在双层循环内的特殊处理要高效的多
		p->prev = p4;
	}
	p->next = NULL; //尾节点
	h.next->prev = NULL; //头节点

	pal->head = h.next;
	pal->cur = h.next;
	pal->curIndex = 0;
	pal->end = p;
}
