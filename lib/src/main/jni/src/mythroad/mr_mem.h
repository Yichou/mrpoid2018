#ifndef _MR_MEM_H
#define _MR_MEM_H

#include "mr_helper.h"


typedef struct mem_header_st {
	uint32 used;
	uint32 left;
} mem_header;


extern char*	LG_mem_base; //VM 内存基址
extern int32	LG_mem_len;	 //VM 内存大小
extern char*	LG_mem_end;	 //VM 内存终址
extern int32	LG_mem_left; //VM 剩余内存
extern uint32	LG_mem_min;	 //VM 内存底值
extern uint32	LG_mem_top;	 //VM 内存峰值
extern mem_header	LG_mem_free;


int32 mr_mem_init(void);
void mr_mem_end(void);
void *mr_malloc(uint32 len);
void mr_free(void *p, uint32 len);
void *mr_realloc(void *p, uint32 oldlen, uint32 newlen);

#endif // _MR_MEM_H
