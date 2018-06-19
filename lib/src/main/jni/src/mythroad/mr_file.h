#ifndef _MR_FILE_H
#define _MR_FILE_H

#include "mr_types.h"

extern uint8 *mr_gzInBuf;
extern uint8 *mr_gzOutBuf;
extern uint8 *LG_gzinptr;
extern uint8 *LG_gzoutcnt;

/*
从mrp中读取文件的所有内容到申请的内存中。
使用该函数得到的内存，需要使用mrc_freeFileData函数释放。

输入:
filename     mrp文件名
lookfor          指出函数的操作形式:
                    0:    读取mrp中文件的内容
                    1:    仅仅查找mrp中是否存在该文件，并不读取
                               文件内容

输出:
*filelen         当lookfor==0时返回文件内容长度
                   当lookfor==1时未知

返回:
   当lookfor==0时
      非NULL         指向读取到的内容的指针，该内存需要
                              调用者释放
      NULL         失败
   当lookfor==1时
      1         mrp中存在该文件
      NULL         mrp中不存在该文件
*/
void *_mr_readFile(const char *filename, int32 *filelen, int32 lookfor);

int32 mrc_GetMrpInfoOpen(const char *filename, int32 *memfile);
int32 mrc_GetMrpInfoClose(int32 memfile, int32 fd);
int32 mrc_GetMrpInfoRead(int32 memfile, int32 fd, int32 offset, void *Dst, uint32 Size);
int32 mrc_GetMrpInfoEx(int32 memfile, int32 fd, int32 type, void *Dst, uint32 Size);

#endif // _MR_FILE_H
