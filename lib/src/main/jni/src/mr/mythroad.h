#ifndef mythroad_h
#define mythroad_h

/*这里是做虚拟机配置的部分*/

/*启动虚拟机Log*/
#define MYTHROAD_DEBUG

/*使用打包应用*/
#define MR_GAME_PACK

/*支持PKZIP模式的压缩文件*/
#define MR_PKZIP_MAGIC


#define MR_AUTHORIZATION

/*退出VM 时是否释放VM 占用的资源
这里如果不释放VM 占用的资源，有可能会导致
文件、网络等资源得不到释放*/
#define MR_EXIT_RELEASE_ALL

#define MR_SECOND_BUF

/*MR_SCREEN_CACHE，使用虚拟机本地缓存*/
#ifdef WIN32
#define MR_SCREEN_CACHE
#else
#define MR_SCREEN_CACHE
#endif

///*使用m0文件*/
//#define MR_M0_FILE

/*默认的启动文件*/
#define MR_DEFAULT_PACK_NAME "*A"
#if 0
#ifdef MR_FS_ASYN
#define MR_DEFAULT_PACK_NAME "*A"
#else
#ifdef DSM_MENU_IN_ROM
#define MR_DEFAULT_PACK_NAME "*A"
#else
#define MR_DEFAULT_PACK_NAME "dsm_m0.mrp"   //"*A"
#endif
#endif
#endif

/*支持短信*/
#define MR_SM_SURPORT

/*支持Socket*/
#define MR_SOCKET_SUPPORT

/*配置结束*/


#define MR_TIME_START(a) {mr_timerStart(a);mr_timer_state = MR_TIMER_STATE_RUNNING;}
#define MR_TIME_STOP() {mr_timerStop();mr_timer_state = MR_TIMER_STATE_IDLE;}

#define DRAW_TEXT_EX_IS_UNICODE               1
#define DRAW_TEXT_EX_IS_AUTO_NEWLINE    2


enum {
   MR_SCREEN_FIRST_BUF,
   MR_SCREEN_SECOND_BUF
};

typedef struct {
   uint16            w;
   uint16            h;
   uint32            buflen;
   uint32            type;
   uint16*            p;
}mr_bitmapSt;

typedef struct {
   uint16*            p;
   uint16            w;
   uint16            h;
   uint16            x;
   uint16            y;
}mr_bitmapDrawSt;

typedef struct {
   uint16            x;
   uint16            y;
   uint16            w;
   uint16            h;
}mr_screenRectSt;

typedef struct {
   uint8            r;
   uint8            g;
   uint8            b;
}mr_colourSt;

typedef struct {
   uint16            h;
}mr_spriteSt;

typedef struct  {
   int16            x;
   int16            y;
   uint16            w;
   uint16            h;
   int16            x1;
   int16            y1;
   int16            x2;
   int16            y2;
   uint16            tilew;
   uint16            tileh;
}mr_tileSt;

typedef struct  {
   int16            t;
   int16            act;
}mr_cycleSt;


typedef struct {
   int16 A;  // A, B, C, and D are fixed point values with an 8-bit integer part
   int16 B;  // and an 8-bit fractional part.
   int16 C;
   int16 D;
   uint16 rop;
} mr_transMatrixSt;

typedef struct {
   void*            p;
   uint32            buflen;
   int32            type;
}mr_soundSt;


#ifdef MR_SCREEN_CACHE
/*下面的MAKERGB是MR平台内部的*/
#ifdef MR_SCREEN_CACHE_BITMAP
#define MAKERGB(r, g, b)     (uint16) ( ((uint32)(r>>3) << 10) + ((uint32)(g>>3) << 5) + ((uint32)(b>>3)) )
#else
#ifdef MR_ANYKA_MOD
#define MAKERGB(r, g, b)     (uint32) ( ((uint32)r<<16) | ((uint32)g << 8) | ((uint32)b) )
#else
#define MAKERGB(r, g, b)     (uint16) ( ((uint32)(r>>3) << 11) + ((uint32)(g>>2) << 5) + ((uint32)(b>>3)) )
#endif
#endif


#ifdef MR_SCREEN_CACHE_BITMAP
#define MR_SCREEN_CACHE_POINT(x, y)    (mr_screenBuf + (MR_SCREEN_H-y) * MR_SCREEN_MAX_W + x)
#else
#ifdef MR_ANYKA_MOD
#define MR_SCREEN_CACHE_POINT(x, y)    ((uint8*)mr_screenBuf + (y * MR_SCREEN_MAX_W + x)*3)
#define MR_BITMAP_POINT(p, x, y, w)    ((uint8*)p + (y * w + x)*3)
#define MR_BITMAP_POINT_COLOUR(p)  ( ((uint32)*((uint8*)p)<<16) | ((uint32)*((uint8*)p+1)<<8) | *((uint8*)p+2) )

#else
#define MR_SCREEN_CACHE_POINT(x, y)    (mr_screenBuf + y * MR_SCREEN_MAX_W + x)
#endif
#endif


#else
/*下面的MAKERGB是For TI平台的*/
#define MAKERGB(r, g, b)     (uint32) ( ((uint32)(r) << 16) + ((uint32)(g) << 8) + ((uint32)(b)) )
#endif

#ifndef FALSE
  #define FALSE 0
#endif

#ifndef TRUE
  #define TRUE 1
#endif



#define realLGmemSize(x) (((x)+7)&(0xfffffff8))

typedef struct                         
{
    uint32 next; 
    uint32 len;
} LG_mem_free_t;



#define MR_BMP_FILE_HEADER_LEN 54
#define MR_SET_U16(p, v) {*(uint8*)p++=(v)&0xff;*(uint8*)p++=(v)>>8;}
#define MR_SET_U32(p, v) {*(uint8*)p++=(v)&0xff;*(uint8*)p++=((v)&0xff00)>>8;\
                                             *(uint8*)p++=((v)&0xff0000)>>16;*(uint8*)p++=((v)&0xff000000)>>24;}

/* 54 byte */
/*
typedef struct {
	uint16	bmType;
	uint32	bmSize;
	uint16	bmReserved1;
	uint16	bmReserved2;
	uint32	bmOffset;
   
	uint32	Size;
	uint32	Width;
	uint32	Height;
	uint16	Planes;
	uint16	BitCount;
	uint32	Compression;
	uint32	SizeImage;
	uint32	XPelsPerMeter;
	uint32	YPelsPerMeter;
	uint32	ClrUsed;
	uint32	ClrImportant;
}mr_bitmap_file_header;
*/


#define MR_FLAGS_BI     1
#define MR_FLAGS_AI     2
#define MR_FLAGS_RI     4
#define MR_FLAGS_EI     8



extern int32 _mr_smsGetBytes(int32 pos, char* p, int32 len);
extern void _mr_showErrorInfo(const char *errstr);
extern int _mr_GetSysInfo(mrp_State* L);
extern int _mr_GetDatetime(mrp_State* L);
extern int mr_Gb2312toUnicode(mrp_State* L);
extern int32 _mr_getHost(mrp_State* L, char* host);

extern int _mr_pcall(int nargs, int nresults);

extern const char *_mr_memfind (const char *s1, size_t l1, const char *s2, size_t l2);
extern int32 _mr_u2c(char * input, int32 inlen, char* output, int32 outlen);




//extern int32 mr_read_asyn_cb(int32 result, uint32  cb_param);
extern mrp_State    *vm_state;

extern int32                mr_timer_run_without_pause;


#ifdef MR_PKZIP_MAGIC
extern int32 mr_zipType;
#endif


typedef int32 (*MR_LOAD_C_FUNCTION)(int32 code);
typedef int32 (*MR_C_FUNCTION)(void* P, int32 code, uint8* input, int32 input_len, uint8** output, int32* output_len);

typedef int32 (*MR_EVENT_FUNCTION)(int16 type, int32 param1, int32 param2);
typedef int32 (*MR_TIMER_FUNCTION)(void);
typedef int32 (*MR_STOP_FUNCTION)(void);
typedef int32 (*MR_PAUSEAPP_FUNCTION)(void);
typedef int32 (*MR_RESUMEAPP_FUNCTION)(void);

typedef void (*mrc_timerCB)(int32 data);


#ifdef MR_C_TEST
extern int32 mr_c_function_load(int32 code);
#endif


#endif
