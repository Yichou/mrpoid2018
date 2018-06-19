/** arraylist.h declaration of arraylist.h
 ** 
 ** 单向可变长度线性链表 工具
 **
 ** 注：第一个节点的index是0
 **
 ** @Author: JianbinZhu
 ** 创建日期：2012/2/16
 ** 最后修改日期： [5/29/2012 JianbinZhu]
 **/

#ifndef _ARRAYLIST_H_
#define _ARRAYLIST_H_



#ifndef boolean

typedef unsigned char boolean;	//定义 布尔类型

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#endif


typedef struct _node {
	long int data;
	struct _node *prev;
	struct _node *next;
}NODE, *PNODE;


typedef struct _arrayList {
	PNODE head;
	PNODE end;
	PNODE cur;
	int curIndex;
	int size;
}ARRAYLIST, *PARRAYLIST;
//注：第一个节点的index是0


//---------------------------------------------------------
//删除列表节点后的回调函数，
//data即为该节点的数据，可以再回调函数里释放申请的内存
//避免手动遍历链表去释放
//---------------------------------------------------------
typedef void (*AL_removeNodeCallBack)(long int data);

//---------------------------------------------------------
//节点比较器 比较用来排序的两个参数
//返回：随第一个参数小于、等于或大于第二个参数而分别返回负整数、零或正整数。
//---------------------------------------------------------
typedef int (*AL_Comparator)(long int n1, long int n2);


//创建链表
PARRAYLIST AL_create(void);

//列表末尾追加数据，新建一个节点保存 数据data
//data可为数值或者结构体指针，结构体指针可以保存多个数据
void AL_add(PARRAYLIST pal, long int data);

void AL_add_sort(PARRAYLIST pal, long int data, AL_Comparator c);

//列表头部追加（类似栈）
void AL_push(PARRAYLIST pal, long int data);

//删除index处节点，返回该节点存储的数据
long int AL_remove(PARRAYLIST pal, int index);

//获取index节点存储的数据
//使用for循环获取
long int AL_get(PARRAYLIST pal, int index);

//优化了的获取节点算法
long int AL_get2(PARRAYLIST pal, int index);

//销毁链表，cb为链表销毁节点的过程中的回调函数，
//可以在cb中释放该节点的数据
void AL_destroy(PARRAYLIST pal, AL_removeNodeCallBack cb);

//如果每个节点数据都是分配的内存，AL_destroy 可以使用此回调函数释放数据
void AL_freeNode(long int data);

//获取链表大小
int AL_size(PARRAYLIST pal);

//检测链表是不是空
boolean AL_isEmpty(PARRAYLIST pal);

//移动节点位置
void AL_moveNodeTo(PARRAYLIST pal, int index, int to);

//移除所有元素
void AL_removeAll(PARRAYLIST pal, AL_removeNodeCallBack cb);

//链表排序
//参数：AL_Comparator：节点比较器
void AL_sort(PARRAYLIST pal, AL_Comparator c);


//-------- 宏操作定义 -------------------------------
#define _AL_GET(pal, index, type) \
	(type)AL_get2(pal, index)

#define _AL_ADD(pal, data) \
	AL_add(pal, (long int)data)

#define _AL_ADD_SORT(pal, data, c) \
	AL_add_sort(pal, (long int)data, c)

#define _AL_PUSH(pal, data) \
	AL_push(pal, (long int)data)

#endif