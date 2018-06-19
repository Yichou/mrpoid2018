/*-----------------------------------*/
// 内存管理

#include <string.h>
#include "mr_helper.h"
#include "mrporting.h"
#include "mythroad.h"
#include "mr_mem.h"




static char		*Origin_LG_mem_base;
static uint32   Origin_LG_mem_len;

char*	LG_mem_base; //VM 内存基址
int32	LG_mem_len;	 //VM 内存大小
char*	LG_mem_end;	 //VM 内存终址
int32	LG_mem_left; //VM 剩余内存
uint32	LG_mem_min;	 //VM 内存底值
uint32	LG_mem_top;	 //VM 内存峰值

mem_header	LG_mem_free;


#define SHOW_MEM_INFO() \
	LOGI("base=%#p, len=%d, left=%d, min=%d, top=%d, free.used=%d, free.left=%d", \
		LG_mem_base, LG_mem_len, LG_mem_left, LG_mem_min, LG_mem_top, LG_mem_free.used, LG_mem_free.left)

int32 mr_mem_init(void) {
	mem_header *head;
    int ret;

    ret = mr_mem_get(&Origin_LG_mem_base, &Origin_LG_mem_len);
    if (0 != ret)
        return -1;

    LG_mem_base = (char*)(((uint32)Origin_LG_mem_base + 3) & 0xfffffffc);
    LG_mem_len = (Origin_LG_mem_base + Origin_LG_mem_len - LG_mem_base) & 0xfffffffc;
    LG_mem_end = LG_mem_base + LG_mem_len;
    LG_mem_free.used = 0;
    LG_mem_free.left = 0;

    head = (mem_header *)LG_mem_base;
    head->used = LG_mem_len;
    head->left = LG_mem_len;
    LG_mem_left = LG_mem_len;
    LG_mem_min = LG_mem_len;
    LG_mem_top = 0;

    return 0;
}

void mr_mem_end(void) {
	mr_mem_free(Origin_LG_mem_base, Origin_LG_mem_len);
}

void *mr_malloc(uint32 len) {
	uint32 len8;
	mem_header *pNext = NULL;
	mem_header *pTop = NULL;
	mem_header *pCur = NULL;

//	LOGI("mr_malloc(%d)", len);

	len8 = (len + 7) & 0xfffffff8;
	if (len8 < LG_mem_left) 
	{
		if (len8) 
		{
			if (&LG_mem_base[LG_mem_free.used] <= LG_mem_end) 
			{
				pTop = &LG_mem_free;
				pCur = (mem_header *) &LG_mem_base[LG_mem_free.used];

				while (1) 
				{
					if ((uint32) pCur >= (uint32) LG_mem_end) {
						LOGE("mr_malloc(%d) no memory", len);
						mr_printf("mr_malloc no memory");
						break;
					}

					if (pCur->left == len8) {
						pTop->used = pCur->used;
						LG_mem_left -= len8;
						goto ok;
					} else if (pCur->left > len8) {
						pNext = (mem_header *) ((char *) pCur + len8);
						pNext->used = pCur->used;
						pNext->left = pCur->left - len8;
						pTop->used += len8;
						LG_mem_left -= len8;
						goto ok;
					}

					pTop = pCur;
					pCur = (mem_header *) &LG_mem_base[pCur->used];
				}
			} else {
				SHOW_MEM_INFO();
				LOGE("mr_malloc(%d) corrupted memory", len);
				mr_printf("mr_malloc corrupted memory");
			}
		} else {
			SHOW_MEM_INFO();
			LOGE("mr_malloc(%d) invalid memory request", len);
			mr_printf("mr_malloc invalid memory request");
		}
	} else {
		SHOW_MEM_INFO();
		LOGE("mr_malloc(%d) no memory", len);
		mr_printf("mr_malloc no memory");
	}

err:
	return NULL;

ok:
	if (LG_mem_left < LG_mem_min)
		LG_mem_min = LG_mem_left;
	if (LG_mem_top < pTop->used)
		LG_mem_top = pTop->used;

	//LOGI("mr_malloc(%d):%#p suc", len, pCur);

	return pCur;
}

void mr_free(void *p, uint32 len) {
	uint32 len8;
	mem_header *pCur = NULL;
	mem_header *pTop = NULL;

	len8 = (len + 7) & 0xfffffff8;
	if (len8 > 0 && NULL != p && (char*) p >= LG_mem_base
			&& (char*) p < LG_mem_end && (char *) p + len8 <= LG_mem_end
			&& (char *) p + len8 > LG_mem_base) {
		pTop = &LG_mem_free;
		for (pCur = (mem_header *) (LG_mem_base + LG_mem_free.used);
				(char*) pCur < LG_mem_end && (char*) pCur < (char*) p; pCur =
						(mem_header *) (LG_mem_base + pCur->used))
			pTop = pCur;
		if (p != pTop && p != pCur) {
			if (pTop == &LG_mem_free
					|| (mem_header *) ((char *) pTop + pTop->left) != p) {
				pTop->used = (char*) p - LG_mem_base;
				pTop = (mem_header *) p;
				pTop->used = (char *) pCur - LG_mem_base;
				pTop->left = len8;
			} else {
				pTop->left += len8;
			}

			if ((uint32) pCur < (uint32) LG_mem_end
					&& (mem_header *) ((char *) p + len8) == pCur) {
				pTop->used = pCur->used;
				pTop->left += pCur->left;
			}
			LG_mem_left += len8;
		} else {
			LOGW("mr_free:already free");
			mr_printf("mr_free:already free");
		}
	} else {
		LOGE("mr_free invalid");
		mr_printf("mr_free invalid");
		mr_printf("%#p,%d,base=%#p,end=%#p", p, len8, LG_mem_base, LG_mem_end);
	}

	//LOGI("mr_free(%#p,%d) suc", p, len);
}

void *mr_realloc(void *p, uint32 oldlen, uint32 newlen) {
	uint32 len;
	void *Dst = NULL;

	if (oldlen <= newlen)
		len = oldlen;
	else
		len = newlen;

	if (NULL != p) {
		if (newlen > 0) {
			Dst = (void *) mr_malloc(newlen);
			if (NULL != Dst) {
				memmove(Dst, p, len);
				mr_free(p, oldlen);
			}
		} else {
			mr_free(p, oldlen);
		}
	} else {
		Dst = (void *) mr_malloc(newlen);
	}

	//LOGI("mr_realloc(%#p,%d,%d) suc", p, oldlen, newlen);

	return Dst;
}
