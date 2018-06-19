
#ifndef forvm_h
#define forvm_h

#include "mr.h"
#include "mrporting.h"
/*

#define BITMAPMAX  10
#define SPRITEMAX  5
#define TILEMAX    2

#define SOUNDMAX   10
#define TIMERMAX  10

#define MR_SPRITE_INDEX_MASK       (0x03FF) // mask of bits used for tile index
#define MR_SPRITE_TRANSPARENT         (0x0400)

#define MR_TILE_SHIFT         (11)



enum {
   K_0,               //按键 0
   K_1,               //按键 1
   K_2,               //按键 2
   K_3,               //按键 3
   K_4,               //按键 4
   K_5,               //按键 5
   K_6,               //按键 6
   K_7,               //按键 7
   K_8,               //按键 8
   K_9,               //按键 9
   K_STAR,            //按键 *
   K_POUND,           //按键 #
   K_UP,              //按键 上
   K_DOWN,            //按键 下
   K_LEFT,            //按键 左
   K_RIGHT,           //按键 右
   K_POWER,           //按键 挂机键
   K_SOFTLEFT,        //按键 左软键
   K_SOFTRIGHT,       //按键 右软键
   K_SEND,            //按键 接听键
   K_SELECT           //按键 确认/选择（若方向键中间有确认键，建议设为该键）
};

enum {
   K_PRESS,
   K_RELEASE,
   K_CLICK,
   K_CLICK_UP
};

*/

enum {
   BM_OR,         //SRC .OR. DST*   半透明效果
   BM_XOR,        //SRC .XOR. DST*
   BM_COPY,       //DST = SRC*      覆盖
   BM_NOT,        //DST = (!SRC)*
   BM_MERGENOT,   //DST .OR. (!SRC)
   BM_ANDNOT,     //DST .AND. (!SRC)
   BM_TRANSPARENT, //透明色不显示，图片的第一个象素（左上角的象素）是透明色
   BM_AND,
   BM_GRAY,
   BM_REVERSE
};

enum {
   MR_FILE_STATE_OPEN,
   MR_FILE_STATE_CLOSED,
   MR_FILE_STATE_NIL
};


#ifdef PC_MOD
#undef MR_FILE_HANDLE
#define MR_FILE_HANDLE FILE*
//#define MR_FILE_HANDLE int32
#else
#define MR_FILE_HANDLE int32
#endif

#ifndef BREW_MOD

typedef struct SaveF {
  MR_FILE_HANDLE f;
} SaveF;

typedef struct LoadF {
  MR_FILE_HANDLE f;
  char buff[MRP_L_BUFFERSIZE];
} LoadF;

#endif

#ifdef BREW_MOD

typedef struct SaveF {
  IFile *f;
} SaveF;

typedef struct LoadF {
  IFile *f;
#ifdef MR_BREW_USE_ZIP
  IUnzipAStream* pUnzip;
#endif
  char buff[MRP_L_BUFFERSIZE];
} LoadF;

#endif



extern int mr_wstrlen(char * txt);

extern void * _mr_readFile(const char* filename, int *filelen, int lookfor);


#if 0 /*老的内存管理形式，由厂商提供函数*/
/*内存申请*/
extern void* mr_malloc(uint32 len);
/*内存释放*/
extern void mr_free(void* p, uint32 len);
/*内存重新分配*/
extern void* mr_realloc(void* p, uint32 oldlen, uint32 len);
#endif

/*下面是当不能取得屏幕缓冲指针时使用的接口 (不完全)   */
//extern void mr_drawBitmap(uint16* bmp, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcolor);
//extern void mr_drawRect(int16 x, int16 y, uint16 w, uint16 h, uint32 color);
//extern int mr_check(uint16*p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
//extern void mr_effect(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb);

#endif

