/*===========================================================================

FILE: mythroad.c
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#ifdef MR_TI_MOD
#undef _INLINE 
#include <string.h>
#endif

#if (defined(MR_ANYKA_MOD)||defined(MR_SPREADTRUM_MOD)||defined(MR_BREW_MOD))
#else
//#if (!defined(MR_ANYKA_MOD))
#include <ctype.h>
#endif


//#include "tomr.h"         // 
#include "mr.h"
#include "mr_lib.h"
#include "mr_auxlib.h"
#include "mr_gb2312.h"
//#include "mr_store.h"
//#include "mr_maketo.h"
#include "mr_forvm.h"
#include"mr_gzip.h"
#include "mythroad.h"
#include "mrcomm.h"
#include "mr_socket_target.h"

#include "mrporting.h"
#include "md5.h"
#include "mr_encode.h"

#include "mr_tcp_target.h"
#include "mr_graphics.h"

//#ifdef MR_M0_FILE
//#include "mr_m0file.h"
//#else
/*
static const unsigned char mr_m0_file1[]=
{0x00
};
*/

/* PKZIP header definitions */
#define LOCSIG 0x04034b50L      /* four-byte lead-in (lsb first) */
#define LOCFLG 6                /* offset of bit flag */
#define  CRPFLG 1               /*  bit for encrypted entry */
#define  EXTFLG 8               /*  bit for extended local header */
#define LOCHOW 8                /* offset of compression method */
#define LOCTIM 10               /* file mod time (for decryption) */
#define LOCCRC 14               /* offset of crc */
#define LOCSIZ 18               /* offset of compressed size */
#define LOCLEN 22               /* offset of uncompressed length */
#define LOCFIL 26               /* offset of file name field length */
#define LOCEXT 28               /* offset of extra field length */
#define LOCHDR 30               /* size of local header, including sig */
#define EXTHDR 16               /* size of extended local header, inc sig */


const unsigned char *mr_m0_files[] =
{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};
//#endif


typedef struct  _mini_mr_c_event_st                       
{
	int32 code;
	int32 param0;
	int32 param1;
	int32 param2;
	int32 param3;
} mini_mr_c_event_st;



mini_mr_c_event_st c_event_st;



#ifdef COMPATIBILITY01
//#define MR_DRAW_TXT_AUTO_UNICODE   //向下兼容，过渡期后取消
#endif


#define MRDBGPRINTF mr_printf


mrp_State    *vm_state;


static uint16*      mr_screenBuf;
#ifdef MR_SCREEN_CACHE_BITMAP
static uint8*      mr_screenBMP;
#endif
#ifdef MR_TRACE
mr_bitmapSt  mr_bitmap[BITMAPMAX+1];
mr_tileSt    mr_tile[TILEMAX];
int16*       mr_map[TILEMAX];
mr_soundSt  mr_sound[SOUNDMAX];
#else
static mr_bitmapSt  mr_bitmap[BITMAPMAX+1];
static mr_tileSt    mr_tile[TILEMAX];
static int16*       mr_map[TILEMAX];
static mr_soundSt  mr_sound[SOUNDMAX];
#endif
static mr_spriteSt  mr_sprite[SPRITEMAX];
//static mr_cycleSt mr_cycle;
int32        mr_state = MR_STATE_IDLE;
static int32        bi = 0;
static char  pack_filename[MR_MAX_FILENAME_SIZE];
//static char  pack_filename_old[MR_MAX_FILENAME_SIZE];
static char  start_filename[MR_MAX_FILENAME_SIZE];

static char  start_fileparameter[MR_MAX_FILENAME_SIZE];

static char  old_pack_filename[MR_MAX_FILENAME_SIZE];
//static char  pack_filename_old[MR_MAX_FILENAME_SIZE];
static char  old_start_filename[MR_MAX_FILENAME_SIZE];

static char  mr_entry[MR_MAX_FILENAME_SIZE];

static int32        mr_screen_w;
static int32        mr_screen_h;
static int32        mr_screen_bit;
static void*       mr_timer_p;
static int32                mr_timer_state = MR_TIMER_STATE_IDLE;
int32                mr_timer_run_without_pause = FALSE;

static char*      mr_exception_str = NULL;


extern int32 mr_cacheSync(void* addr,int32 len);


#ifdef MR_CFG_USE_A_DISK
static char  temp_current_path[MR_MAX_FILENAME_SIZE];
#endif

//******************************************
//将应用加载到内存中运行
static char* mr_ram_file=NULL;
static int mr_ram_file_len;
//******************************************



//*******************************
int8 mr_soundOn = 0;
int8 mr_shakeOn = 0;
//*******************************
uint8 *mr_gzInBuf;
uint8 *mr_gzOutBuf;
unsigned LG_gzinptr;            /* index of next byte to be processed in inbuf */
unsigned LG_gzoutcnt;           /* bytes in output buffer */

#ifdef MR_PKZIP_MAGIC
int32 mr_zipType;
#endif


//*******************************


//*****************************
#ifdef MYTHROAD_DEBUG
static uint32 LG_mem_min;
static uint32 LG_mem_top;
#endif

static LG_mem_free_t LG_mem_free;        
static char *LG_mem_base;
static uint32 LG_mem_len;
static char *Origin_LG_mem_base;
static uint32 Origin_LG_mem_len;
static char *LG_mem_end;   
static uint32 LG_mem_left;
//*****************************



//************************************短信
#define   MR_MAX_NUM_LEN         32         //手机号码最大长度
#define   MR_MAX_TRACE_BUF         100         //TRACE 大小
#define   MR_CMD_NUM 10                        //最大命令号码个数
#define   MR_SECTION_LEN 120                        //一节的长度
#define   MR_MAX_SM_LEN 160                        //短消息的最大长度
#define MR_SMS_CFG_BUF_LEN (MR_SECTION_LEN*36)

#define CFG_USE_UNICODE_OFFSET 5//统一长号码flag偏移
#define CFG_SM_FLAG_OFFSET 32//短信更新flag偏移

#define CFG_USE_SM_UPDATE_OFFSET 4//是否使用短信更新
#define CFG_USE_URL_UPDATE_OFFSET 6//是否使用SMS更新的URL


static const char* dsm_cfg_data_file_name = "dsm.cfg";
#define   DSM_CFG_FILE_NAME  dsm_cfg_data_file_name                     //短信文件名称

static uint8 mr_sms_cfg_buf[MR_SMS_CFG_BUF_LEN];
static int32 mr_sms_cfg_need_save = FALSE;

static uint8 mr_sms_return_flag;
static int32 mr_sms_return_val;


//************************************短信


MR_LOAD_C_FUNCTION mr_load_c_function;
MR_C_FUNCTION mr_c_function;
void *mr_c_function_P;
int32 mr_c_function_P_len;

static int32* mr_c_function_fix_p;

MR_EVENT_FUNCTION mr_event_function = NULL;
MR_TIMER_FUNCTION mr_timer_function = NULL;
MR_STOP_FUNCTION mr_stop_function = NULL;
MR_PAUSEAPP_FUNCTION mr_pauseApp_function = NULL;
MR_RESUMEAPP_FUNCTION mr_resumeApp_function = NULL;

static mrc_timerCB mr_exit_cb = NULL;
static int32 mr_exit_cb_data;

typedef struct
{
	int32   id;
	int32   ver;
	char*  sidName;
	int32   ram;
}mrc_appInfoSt_st;

mrc_appInfoSt_st mrc_appInfo_st;


//*********************性能测试
//#define NU_Retrieve_Clock TMT_Retrieve_Clock

//unsigned long  TMT_Retrieve_Clock(void);


//*********************性能测试

//************read file form mrp plat
#ifdef MR_PLAT_READFILE
int8 mr_flagReadFileForPlat = FALSE;
#endif


//************read file form mrp plat
void mr_md5_init(md5_state_t *pms);
void mr_md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
void mr_md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
//int32 _mr_decode(unsigned char *in, unsigned int len, unsigned char *out);
//int32 _mr_encode(unsigned char *in, unsigned int len, unsigned char *out);

static int32 _mr_smsSetBytes(int32 pos, char* p, int32 len);
static int32 _mr_smsAddNum(int32 index, char* pNum);
static int32 _mr_load_sms_cfg(void);
static int32 _mr_save_sms_cfg(MR_FILE_HANDLE f);
static int32 _mr_newSIMInd(int16  type, uint8* old_IMSI);


static int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h);
static int _mr_isMr(char*  input);

#ifdef MR_ANYKA_MOD
static void _DrawPoint(int16 x, int16 y, uint32 nativecolor);
static void _DrawBitmap(uint16* p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint32 transcoler, int16 sx, int16 sy, int16 mw);
static void _DrawBitmapEx(mr_bitmapDrawSt* srcbmp, mr_bitmapDrawSt* dstbmp, uint16 w, uint16 h, mr_transMatrixSt* pTrans, uint32 transcoler);
static void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
static int32 _DrawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
int _BitmapCheck(uint16*p, int16 x, int16 y, uint16 w, uint16 h, uint32 transcoler, uint32 color_check);
#else
static void _DrawPoint(int16 x, int16 y, uint16 nativecolor);
static void _DrawBitmap(uint16* p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw);
static void _DrawBitmapEx(mr_bitmapDrawSt* srcbmp, mr_bitmapDrawSt* dstbmp, uint16 w, uint16 h, mr_transMatrixSt* pTrans, uint16 transcoler);
static void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b);
static int32 _DrawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font);
int _BitmapCheck(uint16*p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check);
#endif


void mr_platDrawChar(uint16 ch, int32 x, int32 y, int32 color);

void * _mr_readFile(const char* filename, int *filelen, int lookfor);
int mr_wstrlen(char * txt);
int32 mr_registerAPP(uint8 *p, int32 len, int32 index);

void* mr_malloc(uint32 len);
void mr_free(void* p, uint32 len);
void* mr_realloc(void* p, uint32 oldlen, uint32 len);
int32 _mr_c_function_new(MR_C_FUNCTION f, int32 len);
static int _mr_EffSetCon(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb);
static int _mr_TestCom(mrp_State* L, int input0, int input1);
static int32 _DrawTextEx(char* pcText, int16 x, int16 y, 
						 mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font);

static int _mr_TestCom1(mrp_State* L, int input0, char* input1, int32 len);
static int _mr_TestComC(int input0, char* input1, int32 len, int32 code);

int32 mr_stop_ex(int16 freemem);

static int32 _mr_div(int32 a, int32 b);
static int32 _mr_mod(int32 a, int32 b);

static int32 _mr_getMetaMemLimit(void);

static const void* _mr_c_internal_table[] = {
	(void*)mr_m0_files,
	(void*)&vm_state,
	(void*)&mr_state,
	(void*)&bi,

	(void*)&mr_timer_p,
	(void*)&mr_timer_state,
	(void*)&mr_timer_run_without_pause,

	(void*)&mr_gzInBuf,
	(void*)&mr_gzOutBuf,
	(void*)&LG_gzinptr,
	(void*)&LG_gzoutcnt,

	(void*)&mr_sms_cfg_need_save,
	(void*)_mr_smsSetBytes,
	(void*)_mr_smsAddNum,
	(void*)_mr_newSIMInd,

	(void*)_mr_isMr,

#if 0

	(void*)mrp_gettop,  //1937
	(void*)mrp_settop,
	(void*)mrp_pushvalue,
	(void*)mrp_remove,
	(void*)mrp_insert,
	(void*)mrp_replace,

	(void*)mrp_isnumber,
	(void*)mrp_isstring,
	(void*)mrp_iscfunction,
	(void*)mrp_isuserdata,
	(void*)mrp_type,
	(void*)mrp_typename,
	(void*)mrp_shorttypename,


	(void*)mrp_equal,
	(void*)mrp_rawequal,
	(void*)mrp_lessthan,

	(void*)mrp_tonumber,
	(void*)mrp_toboolean,
	(void*)mrp_tostring,
	(void*)mrp_strlen,
	(void*)mrp_tostring_t,
	(void*)mrp_strlen_t,
	(void*)mrp_tocfunction,
	(void*)mrp_touserdata,
	(void*)mrp_tothread,
	(void*)mrp_topointer,

	(void*)mrp_pushnil,
	(void*)mrp_pushnumber,
	(void*)mrp_pushlstring,
	(void*)mrp_pushstring,
	(void*)mrp_pushvfstring,
	(void*)mrp_pushfstring,
	(void*)mrp_pushboolean,
	(void*)mrp_pushcclosure,


	(void*)mrp_gettable,
	(void*)mrp_rawget,
	(void*)mrp_rawgeti,
	(void*)mrp_newtable,
	(void*)mrp_getmetatable,


	(void*)mrp_settable,
	(void*)mrp_rawset,
	(void*)mrp_rawseti,


	(void*)mrp_call,
	(void*)mrp_pcall,
	(void*)mrp_load,

	(void*)mrp_getgcthreshold,
	(void*)mrp_setgcthreshold,


	(void*)mrp_error,

	(void*)mrp_checkstack,
	(void*)mrp_newuserdata,
	(void*)mrp_getfenv,
	(void*)mrp_setfenv,
	(void*)mrp_setmetatable,
	(void*)mrp_cpcall,
	(void*)mrp_next,
	(void*)mrp_concat,
	(void*)mrp_pushlightuserdata,
	(void*)mrp_getgccount,
	(void*)mrp_dump,
	(void*)mrp_yield,
	(void*)mrp_resume,

#endif
	NULL
};

static void* _mr_c_port_table[] = {
	NULL,NULL,NULL,NULL
};


#if defined(MR_BREW_MOD) || defined(MR_MSTAR_MOD)
int mr_strlen(const char * s)
{
	if (s){
		return strlen(s);
	}else{
		return 0;
	}
}
#endif

extern int mr_sprintf(char *buf, const char *fmt, ...);

#ifdef  SDK_MOD
void* sdk_mr_c_function_table;

const void* _mr_c_function_table[] = {
#else
static const void* _mr_c_function_table[] = {
#endif
	(void*)mr_malloc,
	(void*)mr_free,
	(void*)mr_realloc,   // 3

	(void*)mr_memcpy,
	(void*)mr_memmove,
	(void*)mr_strcpy,
	(void*)mr_strncpy,
	(void*)mr_strcat,
	(void*)mr_strncat,
	(void*)mr_memcmp,
	(void*)mr_strcmp,
	(void*)mr_strncmp,
	(void*)mr_strcoll,
	(void*)mr_memchr,
	(void*)mr_memset,
#if (!defined(MR_BREW_MOD) && !defined(MR_MSTAR_MOD))
	(void*)mr_strlen,
#else
	(void*)mr_strlen,
#endif
	(void*)mr_strstr,
	(void*)mr_sprintf,
	(void*)mr_atoi,
	(void*)mr_strtoul,       // 20
	(void*)mr_rand,

	(void*)NULL,
	(void*)mr_stop_ex,    //V1939
	(void*)_mr_c_internal_table,

	(void*)_mr_c_port_table,
	(void*)_mr_c_function_new,  //26
	(void*)mr_printf,
	(void*)mr_mem_get,
	(void*)mr_mem_free ,
	(void*)mr_drawBitmap,
	(void*)mr_getCharBitmap,
	(void*)mr_timerStart,
	(void*)mr_timerStop,
	(void*)mr_getTime,
	(void*)mr_getDatetime,
	(void*)mr_getUserInfo,
	(void*)mr_sleep,           //37

	(void*)mr_plat,
	(void*)mr_platEx,          //39

	(void*)mr_ferrno,
	(void*)mr_open,
	(void*)mr_close,
	(void*)mr_info,
	(void*)mr_write,
	(void*)mr_read,
	(void*)mr_seek,
	(void*)mr_getLen,
	(void*)mr_remove,
	(void*)mr_rename,
	(void*)mr_mkDir,
	(void*)mr_rmDir,
	(void*)mr_findStart,
	(void*)mr_findGetNext,
	(void*)mr_findStop,              //54

	(void*)mr_exit,
	(void*)mr_startShake,
	(void*)mr_stopShake,
	(void*)mr_playSound,
	(void*)mr_stopSound ,         //59

	(void*)mr_sendSms,
	(void*)mr_call,
	(void*)mr_getNetworkID,
	(void*)mr_connectWAP,

	(void*)mr_menuCreate,
	(void*)mr_menuSetItem,
	(void*)mr_menuShow,
	(void*)NULL,//mr_menuSetFocus,
	(void*)mr_menuRelease,
	(void*)mr_menuRefresh,
	(void*)mr_dialogCreate,
	(void*)mr_dialogRelease,
	(void*)mr_dialogRefresh,
	(void*)mr_textCreate,
	(void*)mr_textRelease,
	(void*)mr_textRefresh,
	(void*)mr_editCreate,
	(void*)mr_editRelease,
	(void*)mr_editGetText,
	(void*)mr_winCreate,
	(void*)mr_winRelease,

	(void*)mr_getScreenInfo,

	(void*)mr_initNetwork,
	(void*)mr_closeNetwork,
	(void*)mr_getHostByName,
	(void*)mr_socket,
	(void*)mr_connect,
	(void*)mr_closeSocket,
	(void*)mr_recv,
	(void*)mr_recvfrom,
	(void*)mr_send,
	(void*)mr_sendto,


	(void*)&mr_screenBuf,
	(void*)&mr_screen_w,
	(void*)&mr_screen_h,
	(void*)&mr_screen_bit,
	(void*)mr_bitmap,
	(void*)mr_tile,
	(void*)mr_map,
	(void*)mr_sound,
	(void*)mr_sprite,

	(void*)pack_filename,
	(void*)start_filename,
	(void*)old_pack_filename,
	(void*)old_start_filename,

	(void*)&mr_ram_file,
	(void*)&mr_ram_file_len,

	(void*)&mr_soundOn,
	(void*)&mr_shakeOn,

	(void*)&LG_mem_base,
	(void*)&LG_mem_len,
	(void*)&LG_mem_end,
	(void*)&LG_mem_left,

	(void*)&mr_sms_cfg_buf,
	(void*)mr_md5_init,
	(void*)mr_md5_append,
	(void*)mr_md5_finish,
	(void*)_mr_load_sms_cfg,
	(void*)_mr_save_sms_cfg,
	(void*)_DispUpEx,

	(void*)_DrawPoint,
	(void*)_DrawBitmap,
	(void*)_DrawBitmapEx,
	(void*)DrawRect,
	(void*)_DrawText,
	(void*)_BitmapCheck,
	(void*)_mr_readFile,
	(void*)mr_wstrlen,
	(void*)mr_registerAPP,
	(void*)_DrawTextEx,  //1936
	(void*)_mr_EffSetCon,
	(void*)_mr_TestCom,
	(void*)_mr_TestCom1,   //1938
	(void*)c2u,            //1939
	(void*)_mr_div,            //1941
	(void*)_mr_mod,          

	(void*)&LG_mem_min,
	(void*)&LG_mem_top,
	(void*)mr_updcrc,       //1943
	(void*)start_fileparameter, //1945
	(void*)&mr_sms_return_flag,//1949
	(void*)&mr_sms_return_val,
	(void*)mr_unzip,   //1950
	(void*)&mr_exit_cb,//1951
	(void*)&mr_exit_cb_data,//1951
	(void*)mr_entry,//1952
	(void*)mr_platDrawChar,//2004
	(void*)&LG_mem_free,//1967,2009

	(void*)mr_transbitmapDraw,
	(void*)mr_drawRegion,	   
	NULL

};

static int32 _mr_div(int32 a, int32 b)
{
	return a/b;
}

static int32 _mr_mod(int32 a, int32 b)
{
	return a%b;
}


static int32 _mr_mem_init_ex(int32 ram)
{
#if 1
	if (mr_mem_get(&Origin_LG_mem_base, &Origin_LG_mem_len) != MR_SUCCESS)
#else
	if (mr_mem_get_ex(ram, &Origin_LG_mem_base, &Origin_LG_mem_len) != MR_SUCCESS)
#endif
	{
		return MR_FAILED;
	}
	LG_mem_base = (char*)((uint32)(Origin_LG_mem_base + 3) & (~3));
	LG_mem_len = (Origin_LG_mem_len - (LG_mem_base - Origin_LG_mem_base)) & (~3);
	LG_mem_end = LG_mem_base + LG_mem_len;
	LG_mem_free.next = 0;
	LG_mem_free.len = 0;
	((LG_mem_free_t *) LG_mem_base)->next = LG_mem_len;
	((LG_mem_free_t *) LG_mem_base)->len = LG_mem_len;
	LG_mem_left = LG_mem_len;
#ifdef MYTHROAD_DEBUG
	LG_mem_min = LG_mem_len;
	LG_mem_top = 0;
#endif
	//memset(LG_mem_base+sizeof(LG_mem_free_t),0,
	//                           mem_len-sizeof(LG_mem_free_t));
	return MR_SUCCESS;
}

void* mr_malloc(uint32 len)
{
#if 0
	//#ifdef SDK_MOD
	int32 *t;
	t = malloc(len + 4);
	*t = len;
	return (void*)(t+1);
#else

	LG_mem_free_t *previous, *nextfree, *l;

	len = (uint32) realLGmemSize(len);
	if (len >= LG_mem_left)
	{
		MRDBGPRINTF("mr_malloc no memory");
		return 0;
	}

	if (!len)
	{
		MRDBGPRINTF("mr_malloc invalid memory request");
		return 0;
	}
	if (LG_mem_base + LG_mem_free.next > LG_mem_end)
	{
		MRDBGPRINTF("mr_malloc corrupted memory");
		return 0;
	}

	previous = &LG_mem_free;                        
	nextfree = (LG_mem_free_t *) (LG_mem_base + previous->next); 
	while ((char *) nextfree < LG_mem_end)
	{
		if (nextfree->len == len)             
		{
			previous->next = nextfree->next;         
			LG_mem_left -= len;
#ifdef MYTHROAD_DEBUG
			if (LG_mem_left < LG_mem_min)
				LG_mem_min = LG_mem_left;
			if (LG_mem_top < previous->next)
				LG_mem_top = previous->next;
			//memset(nextfree,0,len); //ouli
#endif
			return (void *) nextfree;
		}
		if (nextfree->len > len)              
		{
			l = (LG_mem_free_t *) ((char *) nextfree + len); 
			l->next = nextfree->next;         
			l->len = (uint32) (nextfree->len - len); 
			previous->next += len;            
			LG_mem_left -= len;
#ifdef MYTHROAD_DEBUG
			if (LG_mem_left < LG_mem_min)
				LG_mem_min = LG_mem_left;
			if (LG_mem_top < previous->next)
				LG_mem_top = previous->next;
			//memset(nextfree,0,len);
#endif
			return (void *) nextfree;            
		}
		previous = nextfree;
		nextfree = (LG_mem_free_t *) (LG_mem_base + nextfree->next);
	}
	MRDBGPRINTF("mr_malloc no memory");

	return 0;
#endif
}

void mr_free(void* p, uint32 len)
{
#if 0
	//#ifdef SDK_MOD
	int32 *t = (int32 *)p;
	t = t - 1;
	if(*t == len){
		free(t);
	}else{
		assert(0);
	}
#else

	LG_mem_free_t *free, *n;

	len = (uint32) realLGmemSize(len);
#ifdef MYTHROAD_DEBUG
	if (!len || !p || (char*)p < LG_mem_base || (char*)p >= LG_mem_end
		|| (char*)p + len > LG_mem_end
		|| (char*)p + len <= LG_mem_base)
	{
		MRDBGPRINTF("mr_free invalid");
		MRDBGPRINTF("p=%d,l=%d,base=%d,LG_mem_end=%d", (int32)p, len, 
			(int32)LG_mem_base, (int32)LG_mem_end);
		return;                     
	}
#endif


	free = &LG_mem_free;                      
	n = (LG_mem_free_t *) (LG_mem_base + free->next);
	while (((char*) n < LG_mem_end) && ((void*) n < p))
	{                                  
		free = n;
		n = (LG_mem_free_t *) (LG_mem_base + n->next);
	}
#ifdef MYTHROAD_DEBUG
	if (p == (void *) free || p == (void *) n)
	{
		MRDBGPRINTF("mr_free:already free");
		return;                   
	}
	//memset(p,0,len);  //ouli
#endif
	if ((free != &LG_mem_free) && ((char *) free + free->len == p))
	{                               
		free->len += len;               
	}
	else
	{
		free->next = (uint32) ((char* )p - LG_mem_base); 
		free = (LG_mem_free_t *) p;         
		free->next = (uint32) ((char *) n - LG_mem_base);
		free->len = len;
	}
	if (((char *) n < LG_mem_end) &&( (char* )p + len == (char *) n))
	{                                  
		free->next = n->next;             
		free->len += n->len;             
	}
	LG_mem_left += len;

#endif
}

void* mr_realloc(void* p, uint32 oldlen, uint32 len)
{
	unsigned long minsize = (oldlen>len)? len:oldlen;
	void *newblock;
	if (p == NULL)
	{
		return mr_malloc(len);
	}

	if (len == 0)
	{
		mr_free(p,oldlen);
		return NULL;
	}
	newblock = mr_malloc(len);
	if (newblock == NULL)
	{
		return newblock;
	}

	MEMMOVE(newblock, p, minsize);
	mr_free(p,oldlen);
	return newblock;

}

//#ifdef MR_PLAT_DRAWTEXT

/*
typedef struct
{
int32 ch;
int32 x;
int32 y;
int32 color;
}mr_drawCharSt;
*/

#ifdef MR_PLAT_DRAWTEXT
void mr_platDrawChar(uint16 ch, int32 x, int32 y, int32 color)
{
	mr_platDrawCharReal((uint16)ch, x, y, (uint16)color);
}
#else
void mr_platDrawChar(uint16 ch, int32 x, int32 y, int32 color)
{

}
#endif



static void _DrawPoint(int16 x, int16 y, uint16 nativecolor)
{
	if (x < 0 || y < 0 || x >= MR_SCREEN_W || y >= MR_SCREEN_H)
		return;
	//*(mr_screenBuf + y * MR_SCREEN_MAX_W + x) = nativecolor;
	*MR_SCREEN_CACHE_POINT(x,y) = nativecolor;
}

static void _DrawBitmap(uint16* p, int16 x, int16 y, uint16 w, uint16 h, uint16 rop, uint16 transcoler, int16 sx, int16 sy, int16 mw)
{
	uint16 *dstp,*srcp;
	int MaxY = MIN(MR_SCREEN_H, y+h);
	int MaxX = MIN(MR_SCREEN_W, x+w);
	int MinY = MAX(0, y);
	int MinX = MAX(0, x);
	uint16 dx,dy;

	if( rop > MR_SPRITE_TRANSPARENT) 
	{
		uint16 BitmapRop = rop & MR_SPRITE_INDEX_MASK;
		uint16 BitmapMode = (rop >> MR_TILE_SHIFT)&0x3;
		uint16 BitmapFlip = (rop >> MR_TILE_SHIFT)&0x4;
		switch (BitmapRop) 
		{
		case BM_TRANSPARENT:
			for (dy=MinY; dy < MaxY; dy++)
			{
				//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
				dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
				srcp = p + (dy - y) * w + (MinX - x);
				for (dx = MinX; dx < MaxX; dx++)
				{
					if (*srcp != transcoler)
						*dstp = *srcp;
					dstp++;
					srcp++;
				}
			}
			break;
		case BM_COPY:
			switch (BitmapMode)
			{
			case MR_ROTATE_0:
				if (MaxX > MinX){
					for (dy=MinY; dy < MaxY; dy++)
					{
						//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
						dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
						srcp = BitmapFlip? p + (h - 1 - (dy - y)) * w + (MinX - x):p + (dy - y) * w + (MinX - x);
						MEMCPY(dstp, srcp, (MaxX - MinX)<<1);
						/*
						for (dx = MinX; dx < MaxX; dx++)
						{
						*dstp = *srcp;
						dstp++;
						srcp++;
						}
						*/
					}
				}
				break;
			case MR_ROTATE_90:
				for (dy=MinY; dy < MaxY; dy++)
				{
					//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
					dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
					srcp = BitmapFlip? p + (h - 1 - (MinX - x)) * w + (w - 1 - (dy - y)):p + (MinX - x) * w + (w - 1 - (dy - y));
					for (dx = MinX; dx < MaxX; dx++)
					{
						*dstp = *srcp;
						dstp++;
						srcp = BitmapFlip? srcp - w:srcp + w;
					}
				}
				break;
			case MR_ROTATE_180:
				for (dy=MinY; dy < MaxY; dy++)
				{
					//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
					dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
					srcp = BitmapFlip? p + (dy - y) * w + (w - 1 - (MinX - x)):p + (h - 1 - (dy - y)) * w + (w - 1 - (MinX - x));
					for (dx = MinX; dx < MaxX; dx++)
					{
						*dstp = *srcp;
						dstp++;
						srcp--;
					}
				}
				break;
			case MR_ROTATE_270:
				for (dy=MinY; dy < MaxY; dy++)
				{
					//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
					dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
					srcp = BitmapFlip? p + (MinX - x) * w + (dy - y):p + (h - 1 - (MinX - x)) * w + (dy - y);
					for (dx = MinX; dx < MaxX; dx++)
					{
						*dstp = *srcp;
						dstp++;
						srcp = BitmapFlip? srcp + w:srcp - w;
					}
				}
				break;
			}
		}
	}else
	{
		switch (rop) 
		{
		case BM_TRANSPARENT:
			for (dy=MinY; dy < MaxY; dy++)
			{
				//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
				dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
				srcp = p + (dy - y + sy) * mw + (MinX - x + sx);
				for (dx = MinX; dx < MaxX; dx++)
				{
					if (*srcp != transcoler)
						*dstp = *srcp;
					dstp++;
					srcp++;
				}
			}
			break;
		case BM_COPY:
			if (MaxX > MinX){
				for (dy=MinY; dy < MaxY; dy++)
				{
					//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
					dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
					srcp = p + (dy - y + sy) * mw + (MinX - x + sx);
					MEMCPY(dstp, srcp, (MaxX - MinX)<<1);
					/*
					for (dx = MinX; dx < MaxX; dx++)
					{
					*dstp = *srcp;
					dstp++;
					srcp++;
					}
					*/
				}
			}
			break;
		case BM_GRAY:
		case BM_OR:
		case BM_XOR:
		case BM_NOT:
		case BM_MERGENOT:
		case BM_ANDNOT:
		case BM_AND:
		case BM_REVERSE:
			for (dy=MinY; dy < MaxY; dy++)
			{
				//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
				dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
				srcp = p + (dy - y + sy) * mw + (MinX - x + sx);
				for (dx = MinX; dx < MaxX; dx++)
				{
					switch (rop) {
					case BM_GRAY:
						if (*srcp != transcoler){
							uint32 color_old = *srcp;
							uint32 r,g,b;
#ifdef MR_SCREEN_CACHE_BITMAP
							r = ((color_old&0x7c00) >> 10);
							g = ((color_old&0x3e0) >> 5) ;
							b = ((color_old&0x1f));
#else
							r = ((color_old&0xf800) >> 11);
							g = ((color_old&0x7e0)  >> 6);
							b = ((color_old&0x1f));
#endif
							r= (r*60+g*118+b*22)/25;
							*dstp = MAKERGB(r, r, r);
						}
						break;
					case BM_REVERSE:
						if (*srcp != transcoler){
							*dstp = ~*srcp;
						}
						break;
					case BM_OR:
						*dstp = (*srcp) | (*dstp);
						break;
					case BM_XOR:
						*dstp = (*srcp) ^ (*dstp);
						break;
					case BM_NOT:
						*dstp = ~ (*srcp);
						break;
					case BM_MERGENOT:
						*dstp = (~ *srcp) | (*dstp);
						break;
					case BM_ANDNOT:
						*dstp = (~ *srcp) & (*dstp);
						break;
					case BM_AND:
						*dstp = (*srcp) & (*dstp);
						break;
					}
					dstp++;
					srcp++;
				}
			}
			break;
		}
	}

}


//static void _DrawBitmapEx(uint16* p, int16 x, int16 y, uint16 w, uint16 h, mr_transMatrixSt* pTrans, uint16 transcoler)
static void _DrawBitmapEx(mr_bitmapDrawSt* srcbmp, mr_bitmapDrawSt* dstbmp, uint16 w, uint16 h, mr_transMatrixSt* pTrans, uint16 transcoler)
{
	/*
	int16 A = pTrans->A;
	int16 B = pTrans->B;
	int16 C = pTrans->C;
	int16 D = pTrans->D;
	uint16 rop = pTrans->rop;
	uint16 *dstp,*srcp;
	int16 CenterX = x + w/2;
	int16 CenterY = y + h/2;
	int16 dx,dy;
	int32 I = A * D - B * C;
	int16 MaxY = (ABS(C) * w/2 + ABS(D) * h/2)/256;
	int16 MinY = 0-MaxY;

	MaxY = MIN(MaxY, MR_SCREEN_H - CenterY);
	MinY = MAX(MinY, 0 - CenterY);

	for(dy=MinY;dy<MaxY;dy++)
	{
	int16 MaxX = MIN(D==0? 999:(D>0? (w * I / 2 /256 + B * dy )/D:(B * dy - w * I / 2 /256 )/D), 
	C==0? 999:(C>0? (A * dy + h * I / 2/256)/C:(A * dy - h * I / 2/256)/C));
	int16 MinX = MAX(D==0? -999:(D>0? (B * dy - w * I / 2 /256 )/D:(w * I / 2 /256 + B * dy )/D), 
	C==0? -999:(C>0? (A * dy - h * I / 2/256)/C:(A * dy + h * I / 2/256)/C));
	MaxX = MIN(MaxX, MR_SCREEN_W - CenterX);
	MinX = MAX(MinX, 0 - CenterX);
	dstp = mr_screenBuf + (dy + CenterY) * MR_SCREEN_MAX_W + (MinX + CenterX);
	for(dx=MinX;dx<MaxX;dx++)
	{
	srcp = p + ( (A * dy - C * dx )*256/I + h/2 ) * w + (D * dx - B * dy)*256/I + w/2;
	if (!((rop == BM_TRANSPARENT) && (*srcp == transcoler)))
	*dstp = *srcp;
	dstp++;
	}
	}
	*/

	/*
	int16 MaxX = MIN(D==0? 999:(D>0? (((w * I)>>9) + B * dy )/D:(B * dy - ((w * I)>>9) )/D), 
	C==0? 999:(C>0? (A * dy + ((h * I)>>9))/C:(A * dy - ((h * I) >>9))/C));
	int16 MinX = MAX(D==0? -999:(D>0? (B * dy - ((w * I)>>9) )/D:(((w * I) >>9) + B * dy )/D), 
	C==0? -999:(C>0? (A * dy - ((h * I)>>9))/C:(A * dy + ((h * I)>>9))/C));
	*/
	int32 A = pTrans->A;
	int32 B = pTrans->B;
	int32 C = pTrans->C;
	int32 D = pTrans->D;
	//uint16 rop = pTrans->rop;
	uint16 *dstp,*srcp;
	int16 CenterX = dstbmp->x + w/2;
	int16 CenterY = dstbmp->y + h/2;
	int32 dx,dy;
	int32 I = A * D - B * C;
	int16 MaxY = (ABS(C) * w + ABS(D) * h)>>9;
	int16 MinY = 0-MaxY;

	MaxY = MIN(MaxY, dstbmp->h - CenterY);
	MinY = MAX(MinY, 0 - CenterY);

	for(dy=MinY;dy<MaxY;dy++)
	{
		int16 MaxX = (int16)MIN(D==0? 999:(MAX((((w * I)>>9) + B * dy )/D,(B * dy - ((w * I)>>9) )/D)), 
			C==0? 999:(MAX((A * dy + ((h * I)>>9))/C,(A * dy - ((h * I) >>9))/C)));
		int16 MinX = (int16)MAX(D==0? -999:(MIN((B * dy - ((w * I)>>9) )/D,(((w * I) >>9) + B * dy )/D)), 
			C==0? -999:(MIN((A * dy - ((h * I)>>9))/C,(A * dy + ((h * I)>>9))/C)));
		MaxX = MIN(MaxX, dstbmp->w - CenterX);
		MinX = MAX(MinX, 0 - CenterX);
		dstp = dstbmp->p + (dy + CenterY) * dstbmp->w + (MinX + CenterX);
		switch(pTrans->rop){
		case BM_TRANSPARENT:
			for(dx=MinX;dx<MaxX;dx++)
			{
				int32 offsety = ((A * dy - C * dx )<<8)/I + h/2;
				int32 offsetx = ((D * dx - B * dy )<<8)/I + w/2;
				if(((offsety < h) && (offsety >= 0))&&((offsetx < w) && (offsetx >= 0))){
					srcp = srcbmp->p + (offsety+srcbmp->y)*srcbmp->w+(offsetx+srcbmp->x);
					//if (!((rop == BM_TRANSPARENT) && (*srcp == transcoler)))
					if (*srcp != transcoler)
						*dstp = *srcp;
				}
				dstp++;
			}
			break;
		case BM_COPY:
			for(dx=MinX;dx<MaxX;dx++)
			{
				int32 offsety = ((A * dy - C * dx )<<8)/I + h/2;
				int32 offsetx = ((D * dx - B * dy )<<8)/I + w/2;
				if(((offsety < h) && (offsety >= 0))&&((offsetx < w) && (offsetx >= 0))){
					srcp = srcbmp->p + (offsety+srcbmp->y)*srcbmp->w+(offsetx+srcbmp->x);
					//if (!((rop == BM_TRANSPARENT) && (*srcp == transcoler)))
					*dstp = *srcp;
				}
				dstp++;
			}
			break;
		}
	}

}

static void DrawRect(int16 x, int16 y, int16 w, int16 h, uint8 r, uint8 g, uint8 b)
{
	//   mr_drawRect(x,y,w,h,MAKERGB(r, g, b)); 
	uint16 *dstp,*srcp;
	int MaxY = MIN(MR_SCREEN_H, y+h);
	int MaxX = MIN(MR_SCREEN_W, x+w);
	int MinY = MAX(0, y);
	int MinX = MAX(0, x);
	uint16 dx,dy;
	uint16 nativecolor;

	/*
	nativecolor = (r/8)<<11;
	nativecolor |=(g/4)<<5;
	nativecolor |=(b/8);          
	*/
	nativecolor = MAKERGB(r, g, b);

	if ((MaxY > MinY) && (MaxX > MinX)){
#if 0
		// for align speed test
		srcp = MR_MALLOC((MaxX - MinX)<<1+8);
		dstp = srcp;
		for (dx = MinX; dx < MaxX; dx++)
		{
			*dstp= nativecolor;
			dstp++;
		}
		for (dy=MinY; dy < MaxY; dy++)
		{
			dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
			memcpy(dstp, srcp+1, (MaxX - MinX)<<1);
			/*
			for (dx = MinX; dx < MaxX; dx++)
			{
			*dstp = nativecolor;
			dstp++;
			}
			*/
		}
		MR_FREE(srcp, (MaxX - MinX)<<1+8);
#endif
#if 0
		// for align test, shut down
		dstp = mr_screenBuf + MinY * MR_SCREEN_MAX_W + MinX;
		srcp = dstp;
		for (dx = MinX; dx < MaxX; dx++)
		{
			*dstp = nativecolor;
			dstp++;
		}
		for (dy=MinY+1; dy < MaxY; dy++)
		{
			dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
			//memcpy(dstp, srcp, (MaxX - MinX)<<1);
			for (dx = MinX; dx < MaxX; dx++)
			{
				*dstp = nativecolor;
				dstp++;
			}
		}

#else
		//dstp = mr_screenBuf + MinY * MR_SCREEN_MAX_W + MinX;
		dstp = MR_SCREEN_CACHE_POINT(MinX, MinY);
		srcp = dstp;
		for (dx = MinX; dx < MaxX; dx++)
		{
			*dstp = nativecolor;
			dstp++;
		}

		if (((uint32)srcp & 0x00000003) != 0){
			//srcp = ((srcp+1) & 0xfffffffc);
			srcp++;
			for (dy=MinY+1; dy < MaxY; dy++)
			{
				//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
				dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
				*dstp = nativecolor;
				//dstp = ((dstp+1) & 0xfffffffc);
				dstp++;
				MEMCPY(dstp, srcp, (MaxX - MinX - 1)<<1);
				/*
				for (dx = MinX; dx < MaxX; dx++)
				{
				*dstp = nativecolor;
				dstp++;
				}
				*/
			}
		}else{
			for (dy=MinY+1; dy < MaxY; dy++)
			{
				//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
				dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
				MEMCPY(dstp, srcp, (MaxX - MinX)<<1);
				/*
				for (dx = MinX; dx < MaxX; dx++)
				{
				*dstp = nativecolor;
				dstp++;
				}
				*/
			}
		}
#endif
	}
	return;
}



static int32 _DrawText(char* pcText, int16 x, int16 y, uint8 r, uint8 g, uint8 b, int is_unicode, uint16 font)
{
	int TextSize;
	//#endif
	uint16 *tempBuf;
	//int tempret=0;

#ifdef MYTHROAD_DEBUG
	if (!pcText)
	{
		//mrp_pushfstring(vm_state, "DrawText x=%d: txt is nil!",x);
		//mrp_error(vm_state);
		MRDBGPRINTF("DrawText x=%d: txt is nil!",x);
		return 0;
	}
#endif

	if (!is_unicode){
		tempBuf = c2u((const char*)pcText, NULL, &TextSize); 
		if (!tempBuf)
		{
			//mrp_pushfstring(vm_state, "DrawText x=%d:c2u err!",x);
			//mrp_error(vm_state);
			MRDBGPRINTF("DrawText x=%d:c2u err!",x);
			return 0;
		}
	}else{
		tempBuf = (uint16 *)pcText;
	}

#ifdef MR_SCREEN_CACHE
	{
		uint16 ch;
		int width, height;
		const char *current_bitmap;
		uint8  *p=(uint8*)tempBuf;
		int32 X1,Y1;
		uint16 a_,b_;
		uint16 chx=x,chy=y,color=MAKERGB(r, g, b);
		ch = (uint16) ((*p<<8)+*(p+1));
		while(ch)
		{
			current_bitmap = mr_getCharBitmap(ch, font, &width, &height);
			if(current_bitmap)
			{
				//MRDBGPRINTF("mr_platDrawChar 21!");
#ifndef MR_PLAT_DRAWTEXT
#ifndef MR_VIA_MOD
#ifndef MR_FONT_LIB_REDUNDANCY_BIT
				int32 font_data_size = ((width * height) + 7) >> 3;
				int32 X2=0,Y2;
				X1 = chx;
				Y1 = chy;
				while(font_data_size--)
				{
					uint8 pattern = *current_bitmap++;

					if(!pattern){
						int32 nTemp;

						X2+=8;
						nTemp = X2/width;

						if(nTemp)
						{
							Y1+=nTemp;

							height-=nTemp;

							if(!height) 
								break;
						}

						X2%=width;
						X1=chx+X2;
					}else{
						for(Y2 = 0; Y2 < 8; Y2++)               
						{                   
#ifdef MTK_MOD
							if(pattern&1)     
#else
							if(pattern&0x80)     
#endif
								_DrawPoint(X1, Y1, color); 
							++X2;      
							if(X2 == width )  
							{                
								X1=chx;          
								height--;         
								++Y1;             

								if(height == 0)     
									break ;           
								X2=0;              
							}                  
							else              
							{                 
								++X1;            
							}                 
#ifdef MTK_MOD
							pattern >>=1;            
#else
							pattern <<=1;            
#endif
						}     
					}
				}
#else

#ifndef MR_FONT_LIB_REDUNDANCY_BIT24
				for (Y1 = 0; Y1 < height; Y1++)
					for (X1 = 0; X1 < width; X1++)
					{
						a_= (X1&(0x07));
						b_= Y1*((width+7)>>3)+((X1&0xF8)>>3);
						if(((uint16)(current_bitmap[b_]))&(0x80>>a_))
							_DrawPoint((int16)(chx+X1),(int16)(chy+Y1), color);
					};
#else
				for (Y1 = 0; Y1 < height; Y1++)
					for (X1 = 0; X1 < width; X1++)
					{
						a_= (X1&(0x07));
						b_= Y1*3+((X1&0xF8)>>3);
						if(((uint16)(current_bitmap[b_]))&(0x80>>a_))
							_DrawPoint((int16)(chx+X1),(int16)(chy+Y1), color);
					};
#endif

#endif
#else //defined MR_VIA_MOD
				for (X1 = 0; X1 < width; X1++)
					for (Y1 = 0; Y1 < height; Y1++)
					{
						a_= (Y1&(0x07));
						b_= X1+((Y1&0xF8)>>3)*width;
						if(((uint16)(current_bitmap[b_]))&(0x01<<a_))
							_DrawPoint((int16)(chx+X1),(int16)(chy+Y1), color);
					};
#endif
#else // MR_PLAT_DRAWTEXT
				//MRDBGPRINTF("mr_platDrawChar 1!");
				mr_platDrawChar(ch, chx, chy, MAKERGB(r, g, b)); 
#endif
				chx = chx + width;
			};
			p+=2;
			ch = (uint16) ((*p<<8)+*(p+1));
		};
	}
#else
	mr_drawText((char *)tempBuf, x, y, MAKERGB(r, g, b)); 
#endif
	//#ifdef MR_DRAW_TXT_AUTO_UNICODE
	if (!is_unicode){
		MR_FREE((void *)tempBuf, TextSize);
	}
	//#endif
	return 0;
}

static int32 _DrawTextEx(char* pcText, int16 x, int16 y, mr_screenRectSt rect, mr_colourSt colorst, int flag, uint16 font)
{
	int TextSize,endchar_index;
	uint16 *tempBuf;
	//int tempret=0;
	uint16 ch;
	endchar_index = 0;

	if (!pcText)
	{
		MRDBGPRINTF("DrawTextEx x=%d: txt is nil!",x);
		return 0;
	}

	if (!(flag & DRAW_TEXT_EX_IS_UNICODE)){
		tempBuf = c2u((const char*)pcText, NULL, &TextSize); 
		if (!tempBuf)
		{
			MRDBGPRINTF("DrawTextEx x=%d:c2u err!",x);
			return 0;
		}
	}else{
		tempBuf = (uint16 *)pcText;
	}

	{
		int width, height, mh;
		const char *current_bitmap;
		uint8  *p=(uint8*)tempBuf;
		int32 X1,Y1;
		uint16 a_,b_;
		uint16 chx=x,chy=y,color=MAKERGB(colorst.r, colorst.g, colorst.b);
		ch = (uint16) ((*p<<8)+*(p+1));
		mh = 0;
		while(ch)
		{
			if ((ch == 0x0a) || (ch == 0x0d)){
				current_bitmap = mr_getCharBitmap(0x20, font, &width, &height);
			}else{
				current_bitmap = mr_getCharBitmap(ch, font, &width, &height);
			}
			if(current_bitmap)
			{
				int32 font_data_size = ((width * height) + 7) >> 3;
				int32 X2=0,Y2;
				if(flag & DRAW_TEXT_EX_IS_AUTO_NEWLINE)
				{
					if(((chx + width) > (x + rect.w)) || (ch == 0x0a)){
						if ((chy + mh) < (y + rect.h) ){
							endchar_index = p - (uint8*)tempBuf;
						}
						X1 = chx = x;
						Y1 = chy = chy + mh + 2;
						mh = 0;
						if(Y1 > (y + rect.h)){
							break;
						}
					}else{
						X1 = chx;
						Y1 = chy;
					}
					mh = (mh > height)? mh:height;
				}else{
					if((chx > (x + rect.w)) || (ch == 0x0a)){
						break;
					}
					if((chx + width) > (x + rect.w)){
						endchar_index = p - (uint8*)tempBuf;
					}
					X1 = chx;
					Y1 = chy;
				}

				if ((ch == 0x0a) || (ch == 0x0d)){
					p+=2;
					ch = (uint16) ((*p<<8)+*(p+1));
					continue;
				}
#ifndef MR_PLAT_DRAWTEXT
#ifndef MR_FONT_LIB_REDUNDANCY_BIT
#ifndef MR_VIA_MOD
				while(font_data_size--)
				{
					uint8 pattern = *current_bitmap++;

					if(!pattern){
						int32 nTemp;

						X2+=8;
						nTemp = X2/width;

						if(nTemp)
						{
							Y1+=nTemp;

							height-=nTemp;

							if(!height) 
								break;
						}

						X2%=width;
						X1=chx+X2;
					}else{
						for(Y2 = 0; Y2 < 8; Y2++)               
						{                   
#ifdef MTK_MOD
							if(pattern&1)     
#else
							if(pattern&0x80)     
#endif
								if(X1 < (x + rect.w) && Y1 < (y + rect.h))
									_DrawPoint(X1, Y1, color); 
							++X2;      
							if(X2 == width )  
							{                
								X1=chx;          
								height--;         
								++Y1;             

								if(height == 0)     
									break ;           
								X2=0;              
							}                  
							else              
							{                 
								++X1;            
							}                 
#ifdef MTK_MOD
							pattern >>=1;            
#else
							pattern <<=1;            
#endif
						}     
					}
				}
#else //defined MR_VIA_MOD
				for (X1 = 0; X1 < width; X1++)
					for (Y1 = 0; Y1 < height; Y1++)
					{
						a_= (Y1&(0x07));
						b_= X1+((Y1&0xF8)>>3)*width;
						if(((uint16)(current_bitmap[b_]))&(0x01<<a_))
							_DrawPoint((int16)(chx+X1),(int16)(chy+Y1), color);
					};
#endif


#else  //#ifndef MR_FONT_LIB_REDUNDANCY_BIT
				/*
				if(flag & DRAW_TEXT_EX_IS_AUTO_NEWLINE)
				{
				if(((chx + width) > (x + rect.w)) || (ch == 0x0a)){
				if ((chy + mh) < (y + rect.h) ){
				endchar_index = p - (uint8*)tempBuf;
				}
				chx = x;
				chy = chy + mh + 2;
				mh = 0;
				if(chy > (y + rect.h)){
				break;
				}
				}
				mh = (mh > height)? mh:height;
				}else{
				if((chx > (x + rect.w)) || (ch == 0x0a)){
				break;
				}
				if((chx + width) > (x + rect.w)){
				endchar_index = p - (uint8*)tempBuf;
				}
				}

				if ((ch == 0x0a) || (ch == 0x0d)){
				p+=2;
				ch = (uint16) ((*p<<8)+*(p+1));
				continue;
				}
				*/

#ifndef MR_FONT_LIB_REDUNDANCY_BIT24
				for (Y1 = 0; Y1 < height; Y1++)
					for (X1 = 0; X1 < width; X1++)
					{
						a_= (X1&(0x07));
						b_= Y1*((width+7)>>3)+((X1&0xF8)>>3);
						if(((uint16)(current_bitmap[b_]))&(0x80>>a_))
							if((chx+X1) < (x + rect.w) && (chy+Y1) < (y + rect.h))
								_DrawPoint((int16)(chx+X1),(int16)(chy+Y1), color);
					};
#else  //MR_FONT_LIB_REDUNDANCY_BIT24
				for (Y1 = 0; Y1 < height; Y1++)
					for (X1 = 0; X1 < width; X1++)
					{
						a_= (X1&(0x07));
						b_= Y1*3+((X1&0xF8)>>3);
						if(((uint16)(current_bitmap[b_]))&(0x80>>a_))
							if((chx+X1) < (x + rect.w) && (chy+Y1) < (y + rect.h))
								_DrawPoint((int16)(chx+X1),(int16)(chy+Y1), color);
					};
#endif

#endif  //MR_FONT_LIB_REDUNDANCY_BIT

#else //MR_PLAT_DRAWTEXT
				mr_platDrawChar(ch, chx, chy, color); 
#endif



				chx = chx + width;
			};
			p+=2;
			ch = (uint16) ((*p<<8)+*(p+1));
		};
		if(!ch){
			if(flag & DRAW_TEXT_EX_IS_AUTO_NEWLINE){
				if((chy + mh) < (y + rect.h)){
					endchar_index = mr_wstrlen((char*)tempBuf);
				}
			}else{
				if(!((chx > (x + rect.w)) || (ch == 0x0a))){
					endchar_index = mr_wstrlen((char*)tempBuf);
				}
			}
		}
	}

	if (!(flag & DRAW_TEXT_EX_IS_UNICODE)){
		MR_FREE((void *)tempBuf, TextSize);
	}
	return endchar_index;
}



int _BitmapCheck(uint16*p, int16 x, int16 y, uint16 w, uint16 h, uint16 transcoler, uint16 color_check)
{

	uint16 *dstp,*srcp;
	int16 MaxY = MIN(MR_SCREEN_H, y+h);
	int16 MaxX = MIN(MR_SCREEN_W, x+w);
	int16 MinY = MAX(0, y);
	int16 MinX = MAX(0, x);
	uint16 dx,dy;
	int nResult = 0;

	for (dy=MinY; dy < MaxY; dy++)
	{
		//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
		dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
		srcp = p + (dy - y) * w + (MinX - x);
		for (dx = MinX; dx < MaxX; dx++)
		{
			if (*srcp != transcoler)
			{
				if (*dstp != color_check)
				{
					nResult++;
				}
			}
			dstp++;
			srcp++;
		}
	}

	return nResult;
}

#if 0
static int MRF_BmGetScr(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	uint16 *srcp,*dstp;
	uint16 dx,dy;
	if(i>=BITMAPMAX){
		mrp_pushfstring(L, "BmGetScr:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	if(mr_bitmap[i].p)
	{
		MR_FREE(mr_bitmap[i].p, mr_bitmap[i].buflen);
		mr_bitmap[i].p = NULL;
	}

	mr_bitmap[i].p = MR_MALLOC(MR_SCREEN_W*MR_SCREEN_H*MR_SCREEN_DEEP);
	if(!mr_bitmap[i].p)
	{
		mrp_pushfstring(L, "BmGetScr %d :No memory!", i);
		mrp_error(L);
		return 0;
	}

	mr_bitmap[i].w = (int16)MR_SCREEN_W;
	mr_bitmap[i].h = (int16)MR_SCREEN_H;
	mr_bitmap[i].buflen = MR_SCREEN_W*MR_SCREEN_H*MR_SCREEN_DEEP;
	dstp = mr_bitmap[i].p;
	for (dy=0; dy < MR_SCREEN_H; dy++)
	{
		//srcp = mr_screenBuf + dy * MR_SCREEN_MAX_W;
		srcp = MR_SCREEN_CACHE_POINT(0, dy);
		for (dx=0; dx < MR_SCREEN_W; dx++)
		{
			*dstp = *srcp;
			dstp++;
			srcp++;
		}
	}
	return 0;
}
#endif

//effect
static int _mr_EffSetCon(int16 x, int16 y, int16 w, int16 h, int16 perr, int16 perg, int16 perb)
{
	uint16 *dstp;
	uint32 color_old,coloer_new;
	int MaxY = MIN(MR_SCREEN_H, y+h);
	int MaxX = MIN(MR_SCREEN_W, x+w);
	int MinY = MAX(0, y);
	int MinX = MAX(0, x);
	uint16 dx,dy;

	for (dy=MinY; dy < MaxY; dy++)
	{
		//dstp = mr_screenBuf + dy * MR_SCREEN_MAX_W + MinX;
		dstp = MR_SCREEN_CACHE_POINT(MinX, dy);
		for (dx = MinX; dx < MaxX; dx++)
		{
			color_old = *dstp;
#ifdef MR_SCREEN_CACHE_BITMAP
			coloer_new = (((color_old&0x7c00) * perr ) >>8) & 0x7c00;
			coloer_new |= (((color_old&0x3e0) * perg ) >>8) & 0x3e0;
			coloer_new |= (((color_old&0x1f) * perb ) >>8) & 0x1f;
#else
			coloer_new = (((color_old&0xf800) * perr ) >>8) & 0xf800;
			coloer_new |= (((color_old&0x7e0) * perg ) >>8) & 0x7e0;
			coloer_new |= (((color_old&0x1f) * perb ) >>8) & 0x1f;
#endif
			*dstp=(uint16)coloer_new;
			dstp++;
		}
	}
	return 0;
}
//effect

#if 0
static int MRF_SpriteCheck(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	uint16 spriteindex = ((uint16)  to_mr_tonumber(L,2,0));
	int16 x = ((int16)  to_mr_tonumber(L,3,0));
	int16 y = ((int16)  to_mr_tonumber(L,4,0));
	uint32 color_check = ((uint32)  to_mr_tonumber(L,5,0));
	uint32 color;
	uint16 r,g,b;
#ifdef MYTHROAD_DEBUG
	if(i>=SPRITEMAX){
		mrp_pushfstring(L, "SpriteCheck:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	if (!mr_bitmap[i].p)
	{
		mrp_pushfstring(L, "SpriteCheck:Sprite %d is nil!",i);
		mrp_error(L);
		return 0;
	}
#endif
	r = (uint16)((color_check&0xff0000)>>16);
	g = (uint16)((color_check&0xff00)>>8);
	b = (uint16)(color_check&0xff);

	/*
	color = (r/8)<<11;
	color |=(g/4)<<5;
	color |=(b/8);     
	*/
	color = MAKERGB(r, g, b);
	//   return mr_check(mr_bitmap[i].p + spriteindex*mr_bitmap[i].w*mr_sprite[i].h,
	//      x, y, mr_bitmap[i].w, mr_sprite[i].h, *(mr_bitmap[i].p), color);
	{
		int to_mr_ret = (int)  _BitmapCheck(mr_bitmap[i].p + 
			spriteindex*mr_bitmap[i].w*mr_sprite[i].h,
			(uint16)x, (uint16)y, (uint16)mr_bitmap[i].w, (uint16)mr_sprite[i].h, 
			(uint16)*(mr_bitmap[i].p), (uint16)color);
		to_mr_pushnumber(L,(mrp_Number)to_mr_ret);
	}

	return 1;
}
#endif



void _mr_showErrorInfo(const char *errstr)
{
	int32 i;
	int32 len = STRLEN(errstr);
	char buf[16];

	MRDBGPRINTF(errstr);
	len = (len < (12*8))? len:12*8;
	DrawRect(0,0,(int16)MR_SCREEN_W,(int16)MR_SCREEN_H,(uint8)255,(uint8)255,(uint8)255);
	for(i=0;i<len;i=i+12)
	{
		MEMSET(buf, 0, sizeof(buf));
		MEMCPY(buf, errstr+i, ((len - i)>12)? 12:(len - i));
		_DrawText(buf, (int16)0, (int16)((i/12)*18), 0, 0, 0, (int)FALSE, MR_FONT_MEDIUM);
		//_DispUpEx(0,0,(uint16)MR_SCREEN_W,(uint16)MR_SCREEN_H);
	}

	mr_drawBitmap(mr_screenBuf, 0,0,(uint16)MR_SCREEN_W,(uint16)MR_SCREEN_H);
	//MRF_DrawText(errstr, 2, 2, 0, 0,0);
}

static void _mr_readFileShowInfo(const char* filename, int32 code)
{
	MRDBGPRINTF( "read file  \"%s\" err, code=%d", filename, code);
}

void * _mr_readFile(const char* filename, int *filelen, int lookfor)
{
	//int ret;
	int method;
	uint32 reallen,found=0;
	int32 oldlen,nTmp;
	uint32 len;
	void* filebuf;
	MR_FILE_HANDLE f;
	char TempName[MR_MAX_FILENAME_SIZE];
	char* mr_m0_file;
	int is_rom_file = FALSE;


//	if(strstr(filename, ".ext") != NULL)
//		mr_printf("_mr_readFile:%s", filename);


	if ((pack_filename[0] == '*')||(pack_filename[0] == '$'))/*m0 file or ram file?*/
	{/*read file from m0*/
		uint32 pos = 0;
		uint32 m0file_len;

		if (pack_filename[0] == '*'){/*m0 file?*/
			mr_m0_file = (char*)mr_m0_files[pack_filename[1]-0x41]; //这里定义文件名为*A即是第一个m0文件
			//*B是第二个.........
		}else{
			mr_m0_file = mr_ram_file;
		}

		if (mr_m0_file == NULL){

			if ((char*)mr_m0_files[pack_filename[1]-0x41]==NULL) MRDBGPRINTF("mr_m0_files[%d] nil!",pack_filename[1]-0x41);
			if (mr_ram_file == NULL) MRDBGPRINTF("mr_ram_file nil!");
			//MRDBGPRINTF( "_mr_readFile:mr_m0_file nil at \"%s\"!",filename);
			_mr_readFileShowInfo(filename, 1001);
			return 0;
		}
		pos = pos + 4;
		MEMCPY(&len, &mr_m0_file[pos], 4);
#ifdef MR_BIG_ENDIAN
		len = ntohl(len);
#endif
		pos = pos + 4;

		if((pack_filename[0] == '$')){
			m0file_len = mr_ram_file_len;

#ifdef MR_AUTHORIZATION
			if(bi & MR_FLAGS_AI){
				if(_mr_isMr(&mr_m0_file[52]) != MR_SUCCESS){
					_mr_readFileShowInfo("unauthorized", 3);
					return 0;
				}
			}else{
			}
#endif
		}else{
			MEMCPY(&m0file_len, &mr_m0_file[pos], 4);

#ifdef MR_BIG_ENDIAN
			m0file_len = ntohl(m0file_len);
#endif
		}

		//MRDBGPRINTF("readFile 11 len = %d", len);
		//MRDBGPRINTF("readFile 21 len = %d", m0file_len);
		pos = pos + len;
		while(!found)
		{
			if (((pos+4) >= m0file_len)||(len<1)||(len>=MR_MAX_FILE_SIZE))
			{
				//MRDBGPRINTF( "_mr_readFile:err 4 at \"%s\"!",filename);
				_mr_readFileShowInfo(filename, 1004);
				return 0;
			}
			MEMCPY(&len, &mr_m0_file[pos], 4);
			//MRDBGPRINTF("readFile 3 len = %d", len);

#ifdef MR_BIG_ENDIAN
			len = ntohl(len);
#endif

			//MRDBGPRINTF("readFile 3 len = %d", len);
			pos = pos + 4;
			if (((len + pos) >= m0file_len)||(len<1)||(len>=MR_MAX_FILENAME_SIZE))
			{
				//MRDBGPRINTF( "_mr_readFile:err 2 at \"%s\"!",filename);
				_mr_readFileShowInfo(filename, 1002);
				return 0;
			}
			MEMSET(TempName, 0, sizeof(TempName));
			MEMCPY(TempName, &mr_m0_file[pos], len);
			//MRDBGPRINTF(TempName);
			pos = pos + len;
			if (STRCMP(filename, TempName)==0)
			{
				if(lookfor == 1)
				{
					return (void *)1;
				}
				found = 1;
				MEMCPY(&len, &mr_m0_file[pos], 4);

#ifdef MR_BIG_ENDIAN
				len = ntohl(len);
#endif

				pos = pos + 4;
				if (((len + pos) > m0file_len)||(len<1)||(len>=MR_MAX_FILE_SIZE))
				{
					//MRDBGPRINTF( "_mr_readFile:err 4 at \"%s\"!",filename);
					_mr_readFileShowInfo(filename, 1003);
					return 0;
				}
			}else
			{
				MEMCPY(&len, &mr_m0_file[pos], 4);

#ifdef MR_BIG_ENDIAN
				len = ntohl(len);
#endif

				//MRDBGPRINTF("l = %d,p = %d", len, pos);
				pos = pos + 4 + len;
			}/*if (STRCMP(filename, TempName)==0)*/
		}

		*filelen = len;
		if (*filelen <= 0 )
		{
			//MRDBGPRINTF("_mr_readFile  \"%s\" len err!", filename);
			_mr_readFileShowInfo(filename, 1005);
			return 0;
		}

		if(lookfor == 2)
		{
			return (void *)&mr_m0_file[pos];
		}
		filebuf = &mr_m0_file[pos];
		is_rom_file = TRUE;

	}else   /*read file from efs , EFS 中的文件*/
	{

		f = mr_open(pack_filename, MR_FILE_RDONLY );
		if (f == 0)
		{
			//MRDBGPRINTF( "file  \"%s\" can not be opened!", filename);
			_mr_readFileShowInfo(filename, 2002);
			return 0;
		}

		// 从这里开始是新版的mrp处理
		{
			uint32 headbuf[4];
			//uint32 infohead_len, mrp_tag;
			//headbuf = (uint32 *)MR_MALLOC(16);
			MEMSET(headbuf, 0, sizeof(headbuf));
			nTmp = mr_read(f, &headbuf, sizeof(headbuf));
#ifdef MR_BIG_ENDIAN
			headbuf[0] = ntohl(headbuf[0]);
			headbuf[1] = ntohl(headbuf[1]);
			headbuf[2] = ntohl(headbuf[2]);
			headbuf[3] = ntohl(headbuf[3]);
#endif
			if( (nTmp != 16)||(headbuf[0] != 1196446285))
			{
				mr_close(f);
				_mr_readFileShowInfo(filename, 3001);
				return 0;
			}
			if(headbuf[1] <= 232){
				mr_close(f);
				_mr_readFileShowInfo(filename, 3051);
				return 0;
			}
			{                             //新版mrp 
				uint32 indexlen = headbuf[1] + 8 - headbuf[3];
				uint8* indexbuf = MR_MALLOC(indexlen);
				uint32 pos = 0;
				uint32 file_pos,file_len;
				if(!indexbuf){
					mr_close(f);
					_mr_readFileShowInfo(filename, 3003);
					return 0;
				}
				nTmp = mr_seek(f, headbuf[3] - 16, MR_SEEK_CUR);
				if (nTmp < 0)
				{
					mr_close(f);
					MR_FREE(indexbuf, indexlen);
					_mr_readFileShowInfo(filename, 3002);
					return 0;
				}

				nTmp = mr_read(f, indexbuf, indexlen);

				if ((nTmp != (int32)indexlen))
				{
					mr_close(f);
					MR_FREE(indexbuf, indexlen);
					_mr_readFileShowInfo(filename, 3003);
					return 0;
				}


				//MRDBGPRINTF("str1=%s",filename);
				while(!found)
				{
					MEMCPY(&len, &indexbuf[pos], 4);
#ifdef MR_BIG_ENDIAN
					len = ntohl(len);
#endif
					pos = pos + 4;
					if (((len + pos) > indexlen)||(len<1)||(len>=MR_MAX_FILENAME_SIZE))
					{
						mr_close(f);
						MR_FREE(indexbuf, indexlen);
						_mr_readFileShowInfo(filename, 3004);
						return 0;
					}
					MEMSET(TempName, 0, sizeof(TempName));
					MEMCPY(TempName, &indexbuf[pos], len);
					pos = pos + len;

#if 0
					MRDBGPRINTF("pos=%d,len=%d",pos,len);
					MRDBGPRINTF("str2=%s",TempName);
					MRDBGPRINTF("strcmp=%d",STRCMP(filename, TempName));
					mr_sleep(50);
#endif

					if (STRCMP(filename, TempName)==0)
					{
						if(lookfor == 1)
						{
							mr_close(f);
							MR_FREE(indexbuf, indexlen);
							return (void *)1;
						}
						found = 1;
						MEMCPY(&file_pos, &indexbuf[pos], 4);
						pos = pos + 4;
						MEMCPY(&file_len, &indexbuf[pos], 4);
						pos = pos + 4;
#ifdef MR_BIG_ENDIAN
						file_pos = ntohl(file_pos);
						file_len = ntohl(file_len);
#endif
						if ((file_pos + file_len) > headbuf[2])
						{
							mr_close(f);
							MR_FREE(indexbuf, indexlen);
							_mr_readFileShowInfo(filename, 3005);
							return 0;
						}
					}else{
						pos = pos + 12;
						if (pos >= indexlen)
						{
							mr_close(f);
							MR_FREE(indexbuf, indexlen);
							_mr_readFileShowInfo(filename, 3006);
							return 0;
						}
					}/*if (STRCMP(filename, TempName)==0)*/
				}

				MR_FREE(indexbuf, indexlen);

				*filelen = file_len;

				//MRDBGPRINTF("Debug:_mr_readFile:old filelen = %d",file_len);
				filebuf = MR_MALLOC((uint32)*filelen);
				if(filebuf == NULL)
				{
					mr_close(f);
					_mr_readFileShowInfo(filename, 3007);
					return 0;
				}

				nTmp = mr_seek(f, file_pos, MR_SEEK_SET);
				if (nTmp < 0)
				{
					MR_FREE(filebuf, *filelen);
					mr_close(f);
					_mr_readFileShowInfo(filename, 3008);
					return 0;
				}



				oldlen = 0;
#ifdef MR_SPREADTRUM_MOD
				if ((*filelen < 0)){
					MRDBGPRINTF("filelen=%d",*filelen);
					MR_FREE(filebuf, file_len);
					mr_close(f);
					_mr_readFileShowInfo(filename, 3010);
					return 0;
				}
#endif
				//MRDBGPRINTF("oldlen=%d",oldlen);
				while(oldlen < *filelen){
					//MRDBGPRINTF("oldlen=%d",oldlen);
					nTmp = mr_read(f, (char*)filebuf+oldlen, *filelen-oldlen);
					//MRDBGPRINTF("Debug:_mr_readFile:readlen = %d,oldlen=%d",nTmp,oldlen);
					//MRDBGPRINTF("oldlen=%d",oldlen);
#ifdef MR_SPREADTRUM_MOD
					if ((nTmp <= 0) || (oldlen > 1024*1024))
#else
					if (nTmp <= 0)
#endif
					{
						//MRDBGPRINTF("oldlen=%d",oldlen);
						MR_FREE(filebuf, *filelen);
						mr_close(f);
						_mr_readFileShowInfo(filename, 3009);
						return 0;
					}
					oldlen = oldlen + nTmp;
				}

				mr_close(f);


			}
		}
		// 新版的mrp处理
	}/*efs file*/




	mr_gzInBuf = filebuf;
	LG_gzoutcnt = 0;
	LG_gzinptr = 0;

	method = mr_get_method(*filelen);
	if (method < 0) 
	{
		return filebuf;             
	}

	reallen  = (uint32)(((uch*)filebuf)[*filelen-4]);
	reallen |= (uint32)(((uch*)filebuf)[*filelen-3]) << 8;
	reallen |= (uint32)(((uch*)filebuf)[*filelen-2]) << 16;
	reallen |= (uint32)(((uch*)filebuf)[*filelen-1]) << 24;

	//MRDBGPRINTF("Debug:_mr_readFile:filelen = %d",reallen);
	//MRDBGPRINTF("Debug:_mr_readFile:mem left = %d",LG_mem_left);

	//MRDBGPRINTF("1base=%d,end=%d",  (int32)LG_mem_base, (int32)LG_mem_end);   
	//MRDBGPRINTF("is_rom_file = %d",is_rom_file);
	mr_gzOutBuf = MR_MALLOC(reallen);
	//MRDBGPRINTF("mr_gzOutBuf = %d",mr_gzOutBuf);
	oldlen = *filelen;
	*filelen = reallen;
	//MRDBGPRINTF("2base=%d,end=%d",  (int32)LG_mem_base, (int32)LG_mem_end);   
	if(mr_gzOutBuf == NULL)
	{
		if(!is_rom_file)
			MR_FREE(mr_gzInBuf, oldlen);
		//MRDBGPRINTF("_mr_readFile  \"%s\" Not memory unzip!", filename);
		return 0;
	}

	//MRDBGPRINTF("3base=%d,end=%d",  (int32)LG_mem_base, (int32)LG_mem_end);   
	if (mr_unzip() != 0) {
		if(!is_rom_file)
			MR_FREE(mr_gzInBuf, oldlen);
		MR_FREE(mr_gzOutBuf, reallen);
		MRDBGPRINTF("_mr_readFile: \"%s\" Unzip err!", filename);
		return 0;
	}

	//MRDBGPRINTF("4base=%d,end=%d",  (int32)LG_mem_base, (int32)LG_mem_end);   
	//MRDBGPRINTF("is_rom_file = %d",is_rom_file);
	if(!is_rom_file)
		MR_FREE(mr_gzInBuf, oldlen);

	//MRDBGPRINTF("is_rom_file = %d",is_rom_file);
	//MRDBGPRINTF("5base=%d,end=%d",  (int32)LG_mem_base, (int32)LG_mem_end);   
	return mr_gzOutBuf;
}



#ifdef MR_PLAT_READFILE
extern MR_FILE_HANDLE mr_openForPlat(const char* filename,  uint32 mode);
extern int32 mr_closeForPlat(MR_FILE_HANDLE f);
extern int32 mr_readForPlat(MR_FILE_HANDLE f,void *p,uint32 l);
extern int32 mr_seekForPlat(MR_FILE_HANDLE f, int32 pos, int method);
extern void* mr_mallocForPlat(uint32 len);
extern void mr_freeForPlat(void* p, uint32 len);


void * _mr_readFileForPlat(const char* mrpname, const char* filename, int *filelen, int lookfor)
{
	int ret;
	int method;
	uint32 reallen,found=0;
	int32 oldlen,nTmp;
	uint32 len;
	void* filebuf;
	MR_FILE_HANDLE f;
	char TempName[MR_MAX_FILENAME_SIZE];
	char* mr_m0_file;
	int is_rom_file = FALSE;

	f = mr_openForPlat(mrpname, MR_FILE_RDONLY );
	if (f == 0)
	{
		//MRDBGPRINTF( "file  \"%s\" can not be opened!", filename);
		_mr_readFileShowInfo(filename, 2002);
		return 0;
	}

	// 从这里开始是新版的mrp处理
	{
		uint32 headbuf[4];
		MEMSET(headbuf, 0, sizeof(headbuf));
		nTmp = mr_readForPlat(f, &headbuf, sizeof(headbuf));
#ifdef MR_BIG_ENDIAN
		headbuf[0] = ntohl(headbuf[0]);
		headbuf[1] = ntohl(headbuf[1]);
		headbuf[2] = ntohl(headbuf[2]);
		headbuf[3] = ntohl(headbuf[3]);
#endif
		if( (nTmp != 16)||(headbuf[0] != 1196446285))
		{
			mr_closeForPlat(f);
			_mr_readFileShowInfo(filename, 3001);
			return 0;
		}
		if(headbuf[1] > 232){                             //新版mrp 
			uint32 indexlen = headbuf[1] + 8 - headbuf[3];
			uint8* indexbuf = mr_mallocForPlat(indexlen);
			uint32 pos = 0;
			uint32 file_pos,file_len;
			if(!indexbuf){
				mr_closeForPlat(f);
				_mr_readFileShowInfo(filename, 3003);
				return 0;
			}
			nTmp = mr_seekForPlat(f, headbuf[3] - 16, MR_SEEK_CUR);
			if (nTmp < 0)
			{
				mr_closeForPlat(f);
				mr_freeForPlat(indexbuf, indexlen);
				_mr_readFileShowInfo(filename, 3002);
				return 0;
			}

			nTmp = mr_readForPlat(f, indexbuf, indexlen);

			if ((nTmp != (int32)indexlen))
			{
				mr_closeForPlat(f);
				mr_freeForPlat(indexbuf, indexlen);
				_mr_readFileShowInfo(filename, 3003);
				return 0;
			}


			//MRDBGPRINTF("str1=%s",filename);
			while(!found)
			{
				MEMCPY(&len, &indexbuf[pos], 4);
#ifdef MR_BIG_ENDIAN
				len = ntohl(len);
#endif
				pos = pos + 4;
				if (((len + pos) > indexlen)||(len<1)||(len>=MR_MAX_FILENAME_SIZE))
				{
					mr_closeForPlat(f);
					mr_freeForPlat(indexbuf, indexlen);
					_mr_readFileShowInfo(filename, 3004);
					return 0;
				}
				MEMSET(TempName, 0, sizeof(TempName));
				MEMCPY(TempName, &indexbuf[pos], len);
				pos = pos + len;
				if (STRCMP(filename, TempName)==0)
				{
					if(lookfor == 1)
					{
						mr_closeForPlat(f);
						mr_freeForPlat(indexbuf, indexlen);
						return (void *)1;
					}
					found = 1;
					MEMCPY(&file_pos, &indexbuf[pos], 4);
					pos = pos + 4;
					MEMCPY(&file_len, &indexbuf[pos], 4);
					pos = pos + 4;
#ifdef MR_BIG_ENDIAN
					file_pos = ntohl(file_pos);
					file_len = ntohl(file_len);
#endif
					if ((file_pos + file_len) > headbuf[2])
					{
						mr_closeForPlat(f);
						mr_freeForPlat(indexbuf, indexlen);
						_mr_readFileShowInfo(filename, 3005);
						return 0;
					}
				}else{
					pos = pos + 12;
					if (pos >= indexlen)
					{
						mr_closeForPlat(f);
						mr_freeForPlat(indexbuf, indexlen);
						_mr_readFileShowInfo(filename, 3006);
						return 0;
					}
				}/*if (STRCMP(filename, TempName)==0)*/
			}

			mr_freeForPlat(indexbuf, indexlen);

			*filelen = file_len;

			//MRDBGPRINTF("Debug:_mr_readFile:old filelen = %d",file_len);
			filebuf = mr_mallocForPlat((uint32)*filelen);
			if(filebuf == NULL)
			{
				mr_closeForPlat(f);
				_mr_readFileShowInfo(filename, 3007);
				return 0;
			}

			nTmp = mr_seekForPlat(f, file_pos, MR_SEEK_SET);
			if (nTmp < 0)
			{
				mr_freeForPlat(filebuf, *filelen);
				mr_closeForPlat(f);
				_mr_readFileShowInfo(filename, 3008);
				return 0;
			}



			oldlen = 0;
#ifdef MR_SPREADTRUM_MOD
			if ((*filelen < 0)){
				MRDBGPRINTF("filelen=%d",*filelen);
				mr_freeForPlat(filebuf, file_len);
				mr_closeForPlat(f);
				_mr_readFileShowInfo(filename, 3010);
				return 0;
			}
#endif
			//MRDBGPRINTF("oldlen=%d",oldlen);
			while(oldlen < *filelen){
				//MRDBGPRINTF("oldlen=%d",oldlen);
				nTmp = mr_readForPlat(f, (char*)filebuf+oldlen, *filelen-oldlen);
				//MRDBGPRINTF("Debug:_mr_readFile:readlen = %d,oldlen=%d",nTmp,oldlen);
				//MRDBGPRINTF("oldlen=%d",oldlen);
#ifdef MR_SPREADTRUM_MOD
				if ((nTmp <= 0) || (oldlen > 1024*1024))
#else
				if (nTmp <= 0)
#endif
				{
					//MRDBGPRINTF("oldlen=%d",oldlen);
					mr_freeForPlat(filebuf, *filelen);
					mr_closeForPlat(f);
					_mr_readFileShowInfo(filename, 3009);
					return 0;
				}
				oldlen = oldlen + nTmp;
			}

			mr_closeForPlat(f);


		}
	}
	// 新版的mrp处理

	mr_gzInBuf = filebuf;
	LG_gzoutcnt = 0;
	LG_gzinptr = 0;

	method = mr_get_method(*filelen);
	if (method < 0) 
	{
		return filebuf;             
	}

	reallen  = (uint32)(((uch*)filebuf)[*filelen-4]);
	reallen |= (uint32)(((uch*)filebuf)[*filelen-3]) << 8;
	reallen |= (uint32)(((uch*)filebuf)[*filelen-2]) << 16;
	reallen |= (uint32)(((uch*)filebuf)[*filelen-1]) << 24;

	mr_gzOutBuf = mr_mallocForPlat(reallen);
	oldlen = *filelen;
	*filelen = reallen;
	if(mr_gzOutBuf == NULL)
	{
		mr_freeForPlat(mr_gzInBuf, oldlen);
		return 0;
	}

	if (mr_unzip() != 0) {
		mr_freeForPlat(mr_gzInBuf, oldlen);
		mr_freeForPlat(mr_gzOutBuf, reallen);
		MRDBGPRINTF("_mr_readFile: \"%s\" Unzip err!", filename);
		return 0;
	}

	mr_freeForPlat(mr_gzInBuf, oldlen);

	return mr_gzOutBuf;
}


void * _mrc_readFile(const char* mrpname, const char* filename, int *filelen, int lookfor)
{
	void * ret;
	mr_flagReadFileForPlat = TRUE;
	//MEMSET(pack_filename,0,sizeof(pack_filename));
	//STRCPY(pack_filename,mrpname);
	ret = _mr_readFileForPlat(mrpname, filename, filelen, lookfor);
	mr_flagReadFileForPlat = FALSE;
	return ret;
}
#endif

//display
static int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h)
{
	if (!(mr_state == MR_STATE_RUN))
	{
		return 0;
	}
#ifdef MR_SCREEN_CACHE
	//mr_drawBitmap(mr_screenBuf,0,0,MR_SCREEN_W,MR_SCREEN_H);
#ifdef MR_SCREEN_CACHE_BITMAP
	//mr_drawBitmap((uint16*)mr_screenBMP,x, y, w, h);
	mr_drawBitmap((uint16*)mr_screenBMP,0, 0, (uint16)MR_SCREEN_W,(uint16)MR_SCREEN_H);
#else
	//mr_drawBitmap(mr_screenBuf,0, 0, (uint16)MR_SCREEN_W,(uint16)MR_SCREEN_H);
	mr_drawBitmap(mr_screenBuf,x, y, (uint16)w,(uint16)h);
#endif
#else
	mr_bufToScreen(x, y, w, h);
#endif
	return 0;
}

#if 0
static int MRF_DispUpEx(mrp_State* L)
{
	int16 x = ((int16)  mrp_tonumber(L,1));
	int16 y = ((int16)  mrp_tonumber(L,2));
	uint16 w = ((uint16)  mrp_tonumber(L,3));
	uint16 h = ((uint16)  mrp_tonumber(L,4));

	return _DispUpEx(x, y, w, h);
}

static int MRF_DispUp(mrp_State* L)
{
	int16 x = ((int16)  mrp_tonumber(L,1));
	int16 y = ((int16)  mrp_tonumber(L,2));
	uint16 w = ((uint16)  mrp_tonumber(L,3));
	uint16 h = ((uint16)  mrp_tonumber(L,4));
	uint16 i = ((uint16)  mr_L_optlong(L,5, BITMAPMAX));

	mr_drawBitmap(mr_bitmap[i].p+y*mr_bitmap[i].h+x,x, y, (uint16)w,(uint16)h);

	return 0;
}

//display

//timer
static int MRF_TimerStart(mrp_State* L)
{
	int n = ((int)  to_mr_tonumber(L,1,0));
	uint16 thistime = ((uint16)  to_mr_tonumber(L,2,0));
	char* pcFunction = ((char*)  to_mr_tostring(L,3,0));
	if (!((mr_state == MR_STATE_RUN) || ((mr_timer_run_without_pause) && (mr_state == MR_STATE_PAUSE))))
	{
		return 0;
	}
	mr_timer_p = (void*)pcFunction;
	MR_TIME_START(thistime);
	//mr_timer_state = MR_TIMER_STATE_RUNNING;
	return 0;
}

static int MRF_TimerStop(mrp_State* L)
{
	int n = ((int)  to_mr_tonumber(L,1,0));
	MR_TIME_STOP();
	//mr_timer_state = MR_TIMER_STATE_IDLE;
	return 0;
}
//timer

//draw
static int MRF_DrawText(mrp_State* L)
{
	char* pcText = ((char*)  to_mr_tostring(L,1,0));
	int16 x = ((int16)  to_mr_tonumber(L,2,0));
	int16 y = ((int16)  to_mr_tonumber(L,3,0));
	uint8 r = ((uint8)  to_mr_tonumber(L,4,0));
	uint8 g = ((uint8)  to_mr_tonumber(L,5,0));
	uint8 b = ((uint8)  to_mr_tonumber(L,6,0));
	int is_unicode = to_mr_toboolean(L, 7, FALSE);
	uint16 font = (uint16)mr_L_optlong(L, 8, MR_FONT_MEDIUM);
	return _DrawText(pcText, x, y, r, g, b, is_unicode, font);
	/*
	char* pcText = ((char*)  to_mr_tostring(L,1,0));
	int16 x = ((int16)  to_mr_tonumber(L,2,0));
	int16 y = ((int16)  to_mr_tonumber(L,3,0));
	uint8 r = ((uint8)  to_mr_tonumber(L,4,0));
	uint8 g = ((uint8)  to_mr_tonumber(L,5,0));
	uint8 b = ((uint8)  to_mr_tonumber(L,6,0));
	int is_unicode = to_mr_toboolean(L, 7, FALSE);
	//#ifdef MR_DRAW_TXT_AUTO_UNICODE
	int TextSize;
	//#endif
	uint16 *tempBuf;
	int tempret=0;

	#ifdef MYTHROAD_DEBUG
	if (!pcText)
	{
	mrp_pushfstring(L, "DrawText x=%d: txt is nil!",x);
	mrp_error(L);
	return 0;
	}
	#endif

	//#ifdef MR_DRAW_TXT_AUTO_UNICODE
	if (!is_unicode){
	//tempBuf = c2u((const char*)pcText, &tempret, &TextSize); 
	tempBuf = c2u((const char*)pcText, NULL, &TextSize); 
	if (!tempBuf)
	{
	mrp_pushfstring(L, "DrawText x=%d:c2u err!",x);
	mrp_error(L);
	return 0;
	}
	}else{
	//#else
	tempBuf = (uint16 *)pcText;
	}
	//#endif

	//mr_drawText((char *)tempBuf+1, x, y, MAKERGB(r, g, b)); 
	#ifdef MR_SCREEN_CACHE
	{
	uint16 ch;
	int width, height;
	const char *current_bitmap;
	uint8  *p=(uint8*)tempBuf;
	int i,j;
	uint16 a_,b_;
	uint16 chx=x,chy=y,color=MAKERGB(r, g, b);
	ch = (uint16) ((*p<<8)+*(p+1));
	while(ch)
	{
	current_bitmap = mr_getCharBitmap(ch, MR_FONT_MEDIUM, &width, &height);
	if(current_bitmap)
	{
	for (i = 0; i < height; i++)
	for (j = 0; j < width; j++)
	{
	a_= (j&(0x07));
	b_= i*((width+7)>>3)+((j&0xF8)>>3);
	//scrTxtPoint(chx+j,chy+i, ((uint16)~(current_bitmap[b]))&(0x80>>a));  //zhangzg Eastcom 0703/2002
	if(((uint16)(current_bitmap[b_]))&(0x80>>a_))
	_DrawPoint((int16)(chx+j),(int16)(chy+i), color);
	};
	};
	p+=2;
	chx = chx + width;
	ch = (uint16) ((*p<<8)+*(p+1));
	};
	}
	#else
	mr_drawText((char *)tempBuf, x, y, MAKERGB(r, g, b)); 
	#endif
	//#ifdef MR_DRAW_TXT_AUTO_UNICODE
	if (!is_unicode){
	MR_FREE((void *)tempBuf, TextSize);
	}
	//#endif
	return 0;
	*/
}


static int MRF_DrawTextEx(mrp_State* L)
{
	char* pcText = ((char*)  to_mr_tostring(L,1,0));
	int16 x, y;
	mr_screenRectSt rect;
	mr_colourSt color;
	int32 flag = (int32)mr_L_optnumber(L, 11, DRAW_TEXT_EX_IS_UNICODE|DRAW_TEXT_EX_IS_AUTO_NEWLINE);
	uint16 font = (uint16)mr_L_optnumber(L, 12, MR_FONT_MEDIUM);
	x = ((int16)  to_mr_tonumber(L,2,0));
	y = ((int16)  to_mr_tonumber(L,3,0));
	rect.x = ((int16)  to_mr_tonumber(L,4,0));
	rect.y = ((int16)  to_mr_tonumber(L,5,0));
	rect.w = ((int16)  to_mr_tonumber(L,6,0));
	rect.h = ((int16)  to_mr_tonumber(L,7,0));
	color.r = ((uint8)  to_mr_tonumber(L,8,0));
	color.g = ((uint8)  to_mr_tonumber(L,9,0));
	color.b = ((uint8)  to_mr_tonumber(L,10,0));
	mrp_pushnumber(L, _DrawTextEx(pcText, x, y, rect, color, flag, font));
	return 1;
}


static int MRF_TextWidth(mrp_State* L)
{
	char* pcText;
	int is_unicode;
	uint16 font;

	int TextSize;
	uint16 *tempBuf;
	//int tempret=0;
	uint16 x=0;
	uint16 y=0;

	if(mrp_type(L, 1) == MRP_TSTRING)
	{
		pcText = ((char*)  to_mr_tostring(L,1,0));
		is_unicode = to_mr_toboolean(L, 2, FALSE);
		font = (uint16)mr_L_optlong(L, 3, MR_FONT_MEDIUM);

		if (!pcText)
		{
			mrp_pushfstring(vm_state, "TextWidth: txt is nil!");
			mrp_error(vm_state);
			return 0;
		}

		if (!is_unicode){
			tempBuf = c2u((const char*)pcText, NULL, &TextSize); 
			if (!tempBuf)
			{
				mrp_pushfstring(vm_state, "TextWidth:c2u err!");
				mrp_error(vm_state);
				return 0;
			}
		}else{
			tempBuf = (uint16 *)pcText;
		}

		{
			uint16 ch;
			int width, height;
			uint8  *p=(uint8*)tempBuf;
			ch = (uint16) ((*p<<8)+*(p+1));
			while(ch)
			{
				mr_getCharBitmap(ch, font, &width, &height);
				p+=2;
				x = x + width;
				y = (height>y)? height:y;
				ch = (uint16) ((*p<<8)+*(p+1));
			};
		}
		if (!is_unicode){
			MR_FREE((void *)tempBuf, TextSize);
		}
		mrp_pushnumber(L, x);
		mrp_pushnumber(L, y);

	}else{
		char temp[4];
		uint16 ch = ((uint16)  mrp_tonumber(L,1));
		int width, height;

		is_unicode = to_mr_toboolean(L, 2, FALSE);
		font = (uint16)mr_L_optlong(L, 3, MR_FONT_MEDIUM);

		if(is_unicode){
			mr_getCharBitmap(ch, font, &width, &height);
		}else{
			if(ch < 128){
				mr_getCharBitmap(ch, font, &width, &height);
			}else{
				temp[0] = ch / 256;
				temp[1] = ch % 256;
				temp[3] = 0;
				tempBuf = c2u((const char*)temp, NULL, &TextSize); 
				if (!tempBuf)
				{
					mrp_pushfstring(vm_state, "TextWidth:c2u err!");
					mrp_error(vm_state);
					return 0;
				}
				ch = (uint16) (((tempBuf[0]<<8)+tempBuf[1]));
				mr_getCharBitmap(ch, font, &width, &height);
				MR_FREE((void *)tempBuf, TextSize);
			}
		}
		mrp_pushnumber(L, width);
		mrp_pushnumber(L, height);

	}
	return 2;
}

static int MRF_DrawRect(mrp_State* L)
{
	int16 x = ((int16)  to_mr_tonumber(L,1,0));
	int16 y = ((int16)  to_mr_tonumber(L,2,0));
	int16 w = ((int16)  to_mr_tonumber(L,3,0));
	int16 h = ((int16)  to_mr_tonumber(L,4,0));
	uint8 r = ((uint8)  to_mr_tonumber(L,5,0));
	uint8 g = ((uint8)  to_mr_tonumber(L,6,0));
	uint8 b = ((uint8)  to_mr_tonumber(L,7,0));
	DrawRect(x,  y,  w,  h,  r,  g,  b);
	return 0;
}

static int MRF_DrawPoint(mrp_State* L)
{
	int16 x = ((int16)  to_mr_tonumber(L,1,0));
	int16 y = ((int16)  to_mr_tonumber(L,2,0));
	uint8 r = ((uint8)  to_mr_tonumber(L,3,0));
	uint8 g = ((uint8)  to_mr_tonumber(L,4,0));
	uint8 b = ((uint8)  to_mr_tonumber(L,5,0));
#ifdef MR_ANYKA_MOD
	uint32 nativecolor;
#else
	uint16 nativecolor;
#endif
	/*
	nativecolor = (r/8)<<11;
	nativecolor |=(g/4)<<5;
	nativecolor |=(b/8);        
	*/
	nativecolor = MAKERGB(r, g, b);
	_DrawPoint(x, y, nativecolor);
	return 0;
}

static int MRF_DrawLine(mrp_State* L)
{
	int16 x1 = ((int16)  to_mr_tonumber(L,1,0));
	int16 y1 = ((int16)  to_mr_tonumber(L,2,0));
	int16 x2 = ((int16)  to_mr_tonumber(L,3,0));
	int16 y2 = ((int16)  to_mr_tonumber(L,4,0));
	uint8 r = ((uint8)  to_mr_tonumber(L,5,0));
	uint8 g = ((uint8)  to_mr_tonumber(L,6,0));
	uint8 b = ((uint8)  to_mr_tonumber(L,7,0));
	int x, y, dx, dy, c1, c2, err, swap = 0;

#ifdef MR_ANYKA_MOD
	uint32 nativecolor;
#else
	uint16 nativecolor;
#endif
	/*
	nativecolor = (r/8)<<11;
	nativecolor |=(g/4)<<5;
	nativecolor |=(b/8);     
	*/
	nativecolor = MAKERGB(r, g, b);

	/*   
	if (x1 < 0 || x1 >= MR_SCREEN_W || x2 < 0 || x2 >= MR_SCREEN_W ||
	y1 < 0 || y1 >= MR_SCREEN_H || y2 < 0 || y2 >= MR_SCREEN_H)
	return;
	*/

	dx = x2 - x1; dy = y2 - y1;
	if (((dx < 0) ? -dx : dx) < ((dy < 0) ? -dy : dy))
	{
		swap = 1;                       /* take the long way        */
		x = x1; x1 = y1; y1 = x;
		x = x2; x2 = y2; y2 = x;
	}
	if (x1 > x2)
	{
		x = x1; x1 = x2; x2 = x;        /* always move to the right */
		y = y1; y1 = y2; y2 = y;
	}

	dx = x2 - x1; dy = y2 - y1;
	c1 = dy * 2; dy = 1;
	if (c1 < 0)
	{
		c1 = -c1;
		dy = -1;
	}
	err = c1 - dx; c2 = err - dx;
	x = x1; y = y1;
	while (x <= x2)
	{
		_DrawPoint((int16)(swap?y:x),(int16)(swap?x:y),nativecolor);
		x++;
		if (err < 0)
			err += c1;
		else
		{
			y += dy;
			err += c2;
		}
	}
	return 0;
}
//draw

//bitmap
static int MRF_BitmapLoad(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	char* filename = ((char*)  to_mr_tostring(L,2,0));
	int16 x = ((int16)  to_mr_tonumber(L,3,0));
	int16 y = ((int16)  to_mr_tonumber(L,4,0));
	uint16 w = ((uint16)  to_mr_tonumber(L,5,0));
	uint16 h = ((uint16)  to_mr_tonumber(L,6,0));
	uint16 max_w = ((uint16)  to_mr_tonumber(L,7,0));
#ifdef MR_ANYKA_MOD
	uint8 *dstp;
	uint16 *filebuf,*srcp;

#else
	uint16* filebuf,*srcp,*dstp;
#endif
	int filelen;
	uint16 y2 = y + h;
	uint16 dx,dy;

	if (!(bi&MR_FLAGS_BI))
	{
		mrp_pushfstring(L, "BitmapLoad:cannot read File \"%s\"!",filename);
		mrp_error(L);
		return 0;
	}

	if(i>BITMAPMAX){
		mrp_pushfstring(L, "BitmapLoad:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	if(mr_bitmap[i].p)
	{
		MR_FREE(mr_bitmap[i].p, mr_bitmap[i].buflen);
		mr_bitmap[i].p = NULL;
	}

	if (*filename == '*')
	{
		return 0;
	}
	//MRDBGPRINTF("BitmapLoad:1 %s", filename);
	filebuf = _mr_readFile(filename, &filelen, 0);
	if(!filebuf)
	{
		mrp_pushfstring(L, "BitmapLoad %d:cannot read \"%s\"!", i,filename);
		mrp_error(L);
		return 0;
	}

	mr_bitmap[i].w = w;
	mr_bitmap[i].h = h;

	//MRDBGPRINTF("BitmapLoad:2 %s", filename);
#ifdef MR_ANYKA_MOD
	if (w*h*2 <= filelen)
	{
		mr_bitmap[i].p = MR_MALLOC(w*h*MR_SCREEN_DEEP);
		if(!mr_bitmap[i].p)
		{
			MR_FREE(filebuf, filelen);
			mrp_pushfstring(L, "BitmapLoad %d \"%s\":No memory!", i,filename);
			mrp_error(L);
			return 0;
		}
		mr_bitmap[i].buflen = w*h*MR_SCREEN_DEEP;
		dstp = (uint8*)mr_bitmap[i].p;
		for (dy=y; dy < y2; dy++)
		{
			srcp = filebuf + dy * max_w + x;
			for (dx=0; dx < w; dx++)
			{
				*dstp++ = ((*srcp&0xf800) >> 8);
				*dstp++ = ((*srcp&0x7e0)  >> 3);
				*dstp++ = ((*srcp&0x1f) << 3);
				srcp++;
			}
		}
		MR_FREE(filebuf, filelen);
		//MRDBGPRINTF("BitmapLoad:4 %s", filename);
	}else
	{
		//MRDBGPRINTF("BitmapLoad:5 %s", filename);
		MR_FREE(filebuf, filelen);
		mrp_pushfstring(L, "BitmapLoad %d \"%s\":len err!", i,filename);
		mrp_error(L);
		return 0;
	}
#else
	if ((x==0)&&(y==0)&&(w==max_w))
	{
		mr_bitmap[i].p = filebuf;
		mr_bitmap[i].buflen = filelen;
	}else if (w*h*MR_SCREEN_DEEP < filelen)
	{
		mr_bitmap[i].p = MR_MALLOC(w*h*MR_SCREEN_DEEP);
		if(!mr_bitmap[i].p)
		{
			MR_FREE(filebuf, filelen);
			mrp_pushfstring(L, "BitmapLoad %d \"%s\":No memory!", i,filename);
			mrp_error(L);
			return 0;
		}
		mr_bitmap[i].buflen = w*h*MR_SCREEN_DEEP;
		dstp = mr_bitmap[i].p;
		for (dy=y; dy < y2; dy++)
		{
			srcp = filebuf + dy * max_w + x;
			for (dx=0; dx < w; dx++)
			{
				*dstp = *srcp;
				dstp++;
				srcp++;
			}
		}
		MR_FREE(filebuf, filelen);
		//MRDBGPRINTF("BitmapLoad:4 %s", filename);
	}else
	{
		//MRDBGPRINTF("BitmapLoad:5 %s", filename);
		MR_FREE(filebuf, filelen);
		mrp_pushfstring(L, "BitmapLoad %d \"%s\":len err!", i,filename);
		mrp_error(L);
		return 0;
	}
#endif

	//MRDBGPRINTF("BitmapLoad:3 %s", filename);
	return 0;
}

static int MRF_BitmapShow(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	int16 x = ((int16)  to_mr_tonumber(L,2,0));
	int16 y = ((int16)  to_mr_tonumber(L,3,0));
	uint16 rop = ((uint16)  mr_L_optint(L,4,BM_COPY));
	int16 sx = ((int16)  mr_L_optint(L,5,0));
	int16 sy = ((int16)  mr_L_optint(L,6,0));
	int16 w = ((int16)  mr_L_optint(L,7,-1));
	int16 h = ((int16)  mr_L_optint(L,8,-1));
#ifdef MYTHROAD_DEBUG
	if(i>BITMAPMAX){
		mrp_pushfstring(L, "BitmapShow:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	if (!mr_bitmap[i].p)
	{
		mrp_pushfstring(L, "BitmapShow %d:bitmap is nil!", i);
		mrp_error(L);
		return 0;
	}
#endif

	w = (w==-1)? mr_bitmap[i].w:w;
	h = (h==-1)? mr_bitmap[i].h:h;
	//   mr_drawBitmap(mr_bitmap[i].p, x, y, mr_bitmap[i].w, mr_bitmap[i].h, rop, *(mr_bitmap[i].p));
#ifdef MR_ANYKA_MOD
	_DrawBitmap(mr_bitmap[i].p, x, y, w, h, rop, 
		MR_BITMAP_POINT_COLOUR(mr_bitmap[i].p),
		sx, sy, mr_bitmap[i].w);
#else
	_DrawBitmap(mr_bitmap[i].p, x, y, w, h, rop, *(mr_bitmap[i].p), sx, sy, mr_bitmap[i].w);
#endif

	return 0;
}

static int MRF_BitmapShowEx(mrp_State* L)
{
	uint16* p = ((uint16*)  mrp_tonumber(L,1));
	int16 x = ((int16)  mrp_tonumber(L,2));
	int16 y = ((int16)  mrp_tonumber(L,3));
	int16 mw = ((int16)  mrp_tonumber(L,4));
	int16 w = ((int16)  mrp_tonumber(L,5));
	int16 h = ((int16)  mrp_tonumber(L,6));
	uint16 rop = ((uint16)  mr_L_optint(L,7,BM_COPY));
	int16 sx = ((int16)  mr_L_optint(L,8,0));
	int16 sy = ((int16)  mr_L_optint(L,9,0));

#ifdef MR_ANYKA_MOD
	_DrawBitmap(p, x, y, w, h, rop,
		MR_BITMAP_POINT_COLOUR(p),
		sx, sy, mw);
#else
	_DrawBitmap(p, x, y, w, h, rop, *p, sx, sy, mw);
#endif
	return 0;
}


static int MRF_BitmapNew(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	uint16 w = ((uint16)  to_mr_tonumber(L,2,0));
	uint16 h = ((uint16)  to_mr_tonumber(L,3,0));
	if(i>BITMAPMAX){
		mrp_pushfstring(L, "BitmapNew:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	if(mr_bitmap[i].buflen != w*h*2)
	{
		if(mr_bitmap[i].p)
		{
			MR_FREE(mr_bitmap[i].p, mr_bitmap[i].buflen);
			mr_bitmap[i].p = NULL;
		}
		mr_bitmap[i].p = MR_MALLOC(w*h*2);
		if(!mr_bitmap[i].p)
		{
			mrp_pushfstring(L, "BitmapNew %d :No memory!", i);
			mrp_error(L);
			return 0;
		}
		MEMSET(mr_bitmap[i].p, 0, w*h*2);
	}
	mr_bitmap[i].buflen = w*h*2;
	mr_bitmap[i].w = w;
	mr_bitmap[i].h = h;
	return 0;
}

static int MRF_BitmapDraw(mrp_State* L)
{
	uint16 di = ((uint16)  to_mr_tonumber(L,1,0));
	int16 dx = ((int16)  to_mr_tonumber(L,2,0));
	int16 dy = ((int16)  to_mr_tonumber(L,3,0));
	uint16 si = ((uint16)  to_mr_tonumber(L,4,0));
	int16 sx = ((int16)  to_mr_tonumber(L,5,0));
	int16 sy = ((int16)  to_mr_tonumber(L,6,0));
	uint16 w = ((uint16)  to_mr_tonumber(L,7,0));
	uint16 h = ((uint16)  to_mr_tonumber(L,8,0));
	int16 A = ((int16)  to_mr_tonumber(L,9,0));
	int16 B = ((int16)  to_mr_tonumber(L,10,0));
	int16 C = ((int16)  to_mr_tonumber(L,11,0));
	int16 D = ((int16)  to_mr_tonumber(L,12,0));
	uint16 rop = ((uint16)  to_mr_tonumber(L,13,BM_COPY));

	mr_transMatrixSt Trans;
	mr_bitmapDrawSt srcbmp;
	mr_bitmapDrawSt dstbmp;

	if((si > BITMAPMAX)||( di > BITMAPMAX)){
		mrp_pushfstring(L, "BitmapDraw:index %d or %d invalid!", di, si);
		mrp_error(L);
		return 0;
	}

	if ((!mr_bitmap[si].p)||(!mr_bitmap[di].p))
	{
		mrp_pushfstring(L, "BitmapDraw:index %d or %d invalid!", di, si);
		mrp_error(L);
		return 0;
	}

	Trans.A = A;
	Trans.B = B;
	Trans.C = C;
	Trans.D = D;
	Trans.rop = rop;

	dstbmp.w = mr_bitmap[di].w;
	dstbmp.h = mr_bitmap[di].h;
	dstbmp.x = dx;
	dstbmp.y = dy;
	dstbmp.p = mr_bitmap[di].p;

	srcbmp.w = mr_bitmap[si].w;
	srcbmp.h = mr_bitmap[si].h;
	srcbmp.x = sx;
	srcbmp.y = sy;
	srcbmp.p = mr_bitmap[si].p;

#ifdef MR_ANYKA_MOD
	_DrawBitmapEx(&srcbmp, &dstbmp, w, h, &Trans, MR_BITMAP_POINT_COLOUR(mr_bitmap[si].p));
#else
	_DrawBitmapEx(&srcbmp, &dstbmp, w, h, &Trans, *(mr_bitmap[si].p));
#endif
	return 0;
	/*
	uint16 w = mr_bitmap[i].w;
	uint16 h = mr_bitmap[i].h;
	uint16 dw = mr_bitmap[di].w;
	uint16 dh = mr_bitmap[di].h;
	uint16 *dstp,*srcp;
	uint16 *sp = mr_bitmap[i].p;
	uint16 *dp = mr_bitmap[di].p;
	int16 CenterX = x + w/2;
	int16 CenterY = y + h/2;
	int16 dx,dy;
	int32 I = A * D - B * C;
	int16 MaxY = (ABS(C) * w + ABS(D) * h)>>9;
	int16 MinY = 0-MaxY;

	MaxY = MIN(MaxY, dh - CenterY);
	MinY = MAX(MinY, 0 - CenterY);

	for(dy=MinY;dy<MaxY;dy++)
	{
	int16 MaxX = MIN(D==0? 999:(D>0? (((w * I)>>9) + B * dy )/D:(B * dy - ((w * I)>>9) )/D), 
	C==0? 999:(C>0? (A * dy + ((h * I)>>9))/C:(A * dy - ((h * I) >>9))/C));
	int16 MinX = MAX(D==0? -999:(D>0? (B * dy - ((w * I)>>9) )/D:(((w * I) >>9) + B * dy )/D), 
	C==0? -999:(C>0? (A * dy - ((h * I)>>9))/C:(A * dy + ((h * I)>>9))/C));
	MaxX = MIN(MaxX, dw - CenterX);
	MinX = MAX(MinX, 0 - CenterX);
	dstp = dp + (dy + CenterY) * dw + (MinX + CenterX);
	for(dx=MinX;dx<MaxX;dx++)
	{
	int32 offsety = ((A * dy - C * dx )<<8)/I + h/2;
	int32 offsetx = ((D * dx - B * dy )<<8)/I + w/2;
	if(((offsety < h) && (offsety >= 0))&&((offsetx < w) && (offsetx >= 0))){
	srcp = sp + offsety*w+offsetx;
	*dstp = *srcp;
	}
	dstp++;
	srcp = sp + ( ((A * dy - C * dx )<<8)/I + h/2 ) * w + ((D * dx - B * dy)<<8)/I + w/2;
	*dstp = *srcp;
	dstp++;
	}
	}
	return 0;
	*/
}



static int MRF_BitmapInfo(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	if(i>BITMAPMAX){
		mrp_pushfstring(L, "MRF_BitmapInfo:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	mrp_pushnumber(L, (mrp_Number)mr_bitmap[i].p);
	mrp_pushnumber(L, mr_bitmap[i].buflen);
	mrp_pushnumber(L, mr_bitmap[i].w);
	mrp_pushnumber(L, mr_bitmap[i].h);
	mrp_pushnumber(L, mr_bitmap[i].type);
	return 5;
}
//bitmap

//sprite
static int MRF_SpriteSet(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	uint16 h = ((uint16)  to_mr_tonumber(L,2,0));
	if(i>=SPRITEMAX){
		mrp_pushfstring(L, "SpriteSet:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	mr_sprite[i].h = h;
	return 0;
}

static int MRF_SpriteDraw(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	uint16 spriteindex = ((uint16)  to_mr_tonumber(L,2,0));
	int16 x = ((int16)  to_mr_tonumber(L,3,0));
	int16 y = ((int16)  to_mr_tonumber(L,4,0));
	uint16 mod = ((uint16)  to_mr_tonumber(L,5,BM_TRANSPARENT));
#ifdef MYTHROAD_DEBUG
	if(i>=SPRITEMAX){
		mrp_pushfstring(L, "SpriteDraw:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	if (!mr_bitmap[i].p)
	{
		mrp_pushfstring(L, "SpriteDraw:Sprite %d is nil!",i);
		mrp_error(L);
		return 0;
	}
#endif
	/*
	mr_drawBitmap(mr_bitmap[i].p + spriteindex*mr_bitmap[i].w*mr_sprite[i].h,
	x, y, mr_bitmap[i].w, mr_sprite[i].h, BM_TRANSPARENT, *(mr_bitmap[i].p));
	*/
#ifdef MR_ANYKA_MOD
	_DrawBitmap((uint16*)((uint8*)mr_bitmap[i].p + spriteindex*mr_bitmap[i].w*mr_sprite[i].h*3),
		x, y, mr_bitmap[i].w, mr_sprite[i].h, mod, MR_BITMAP_POINT_COLOUR(mr_bitmap[i].p), 0 , 0, mr_bitmap[i].w);
#else
	_DrawBitmap(mr_bitmap[i].p + spriteindex*mr_bitmap[i].w*mr_sprite[i].h,
		x, y, mr_bitmap[i].w, mr_sprite[i].h, mod, *(mr_bitmap[i].p), 0 , 0, mr_bitmap[i].w);
#endif
	return 0;
}

static int MRF_SpriteDrawEx(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	uint16 spriteindex = ((uint16)  to_mr_tonumber(L,2,0));
	int16 x = ((int16)  to_mr_tonumber(L,3,0));
	int16 y = ((int16)  to_mr_tonumber(L,4,0));
	int16 A = ((int16)  to_mr_tonumber(L,5,0));
	int16 B = ((int16)  to_mr_tonumber(L,6,0));
	int16 C = ((int16)  to_mr_tonumber(L,7,0));
	int16 D = ((int16)  to_mr_tonumber(L,8,0));
	mr_transMatrixSt Trans;
	mr_bitmapDrawSt srcbmp;
	mr_bitmapDrawSt dstbmp;

	if(i>=SPRITEMAX){
		mrp_pushfstring(L, "SpriteDrawEx:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	Trans.A = A;
	Trans.B = B;
	Trans.C = C;
	Trans.D = D;
	Trans.rop = BM_TRANSPARENT;

	dstbmp.w = (uint16)MR_SCREEN_W;
	dstbmp.h = (uint16)MR_SCREEN_H;
	dstbmp.x = x;
	dstbmp.y = y;
	dstbmp.p = mr_screenBuf;

	srcbmp.w = mr_bitmap[i].w;
	srcbmp.h = mr_sprite[i].h;
	srcbmp.x = 0;
	srcbmp.y = 0;
#ifdef MR_ANYKA_MOD
	srcbmp.p = (uint16*)((uint8*)mr_bitmap[i].p + 
		(spriteindex & MR_SPRITE_INDEX_MASK)*mr_bitmap[i].w*mr_sprite[i].h*3);
	_DrawBitmapEx(&srcbmp, &dstbmp, mr_bitmap[i].w, mr_sprite[i].h, &Trans,
		MR_BITMAP_POINT_COLOUR(mr_bitmap[i].p)   );
#else
	srcbmp.p = mr_bitmap[i].p + (spriteindex & MR_SPRITE_INDEX_MASK)*mr_bitmap[i].w*mr_sprite[i].h;
	_DrawBitmapEx(&srcbmp, &dstbmp, mr_bitmap[i].w, mr_sprite[i].h, &Trans, *(mr_bitmap[i].p));
#endif

	//_DrawBitmapEx(mr_bitmap[i].p + (spriteindex & MR_SPRITE_INDEX_MASK)*mr_bitmap[i].w*mr_sprite[i].h,
	//   x, y, mr_bitmap[i].w, mr_sprite[i].h, &Trans, *(mr_bitmap[i].p));
	return 0;
}

//sprite

//tile
static int MRF_TileSet(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	int16 x = ((int16)  to_mr_tonumber(L,2,0));
	int16 y = ((int16)  to_mr_tonumber(L,3,0));
	uint16 w = ((uint16)  to_mr_tonumber(L,4,0));
	uint16 h = ((uint16)  to_mr_tonumber(L,5,0));
	uint16 tileh = ((uint16)  to_mr_tonumber(L,6,0));
#ifdef MYTHROAD_DEBUG
	if (i >= TILEMAX)
	{
		mrp_pushstring(L, "TileSet:tile index out of rang!");
		mrp_error(L);
		return 0;
	}
#endif

	if (w*h*2 != mr_tile[i].w*mr_tile[i].h*2)
	{
		if(mr_map[i])
		{
			MR_FREE(mr_map[i], mr_tile[i].w*mr_tile[i].h*2);
			mr_map[i] = NULL;
		}
		if (w == 0)
		{
			return 0;
		}
	}

	mr_tile[i].x = x;
	mr_tile[i].y = y;
	mr_tile[i].w = w;
	mr_tile[i].h = h;
	mr_tile[i].tileh = tileh;

	if(mr_map[i] == NULL)
		mr_map[i] = MR_MALLOC(w*h*2);
	return 0;
}

static int MRF_TileSetRect(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	int16 x1 = ((int16)  to_mr_tonumber(L,2,0));
	int16 y1 = ((int16)  to_mr_tonumber(L,3,0));
	int16 x2 = ((int16)  to_mr_tonumber(L,4,0));
	int16 y2 = ((int16)  to_mr_tonumber(L,5,0));
#ifdef MYTHROAD_DEBUG
	if (i >= TILEMAX)
	{
		mrp_pushstring(L, "TileSet:tile index out of rang!");
		mrp_error(L);
		return 0;
	}
#endif

	mr_tile[i].x1 = x1;
	mr_tile[i].y1 = y1;
	mr_tile[i].x2 = x2;
	mr_tile[i].y2 = y2;

	return 0;
}

static int MRF_TileDraw(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	int16 x = mr_tile[i].x;
	int16 y = mr_tile[i].y;
	uint16 tilew = mr_bitmap[i].w;
	uint16 tileh = mr_tile[i].tileh;
	uint16 w = mr_tile[i].w;
	uint16 h = mr_tile[i].h;
	unsigned xStart = x > 0 ? 0 : (-x) / mr_bitmap[i].w;
	unsigned xEnd = MIN(w, ((unsigned)(MR_SCREEN_W - x + mr_bitmap[i].w - 1) / mr_bitmap[i].w));
	unsigned yStart = y > 0 ? 0 : (-y) / mr_tile[i].tileh;
	unsigned yEnd = MIN(h, ((unsigned)(MR_SCREEN_H - y + mr_tile[i].tileh - 1) / mr_tile[i].tileh));
	uint16 dx,dy;

#ifdef MYTHROAD_DEBUG
	if (i >= TILEMAX)
	{
		mrp_pushstring(L, "TileDraw:tile index out of rang!");
		mrp_error(L);
		return 0;
	}
#endif
#ifdef MYTHROAD_DEBUG
	if (!mr_bitmap[i].p)
	{
		mrp_pushfstring(L, "TileDraw:Tile %d is nil!",i);
		mrp_error(L);
		return 0;
	}
#endif

	for (dy = yStart; dy < yEnd; dy++)
	{
		for (dx = xStart; dx < xEnd; dx++) 
		{
			//         mr_drawBitmap(mr_bitmap[i].p + mr_map[i][mr_tile[i].w * dy + dx]*mr_bitmap[i].w*mr_tile[i].tileh,
			//            dx * mr_bitmap[i].w + x, dy * mr_tile[i].tileh + y, mr_bitmap[i].w, 
			//            mr_tile[i].tileh, BM_COPY, 0);
			uint16 unTile = mr_map[i][w * dy + dx];

			if ((unTile & MR_SPRITE_INDEX_MASK) != MR_SPRITE_INDEX_MASK) 
			{
				int16 drawX = dx * tilew + x;
				int16 drawY = dy * tileh + y;
				if ((drawX+tilew>= mr_tile[i].x1) &&  (drawX< mr_tile[i].x2) 
					&& (drawY+tileh>= mr_tile[i].y1) &&  (drawY< mr_tile[i].y2) )
#ifdef MR_ANYKA_MOD
					_DrawBitmap((uint16*)((uint8*)mr_bitmap[i].p +
					(unTile&MR_SPRITE_INDEX_MASK)*tilew*tileh*3),
					(int16)drawX, 
					(int16)drawY, 
					(uint16)tilew, 
					(uint16)tileh, 
					(uint16)((unTile & 0xfc00) + 
					((unTile & MR_SPRITE_TRANSPARENT)? BM_TRANSPARENT:BM_COPY)),
					MR_BITMAP_POINT_COLOUR(mr_bitmap[i].p), 0, 0, (uint16)tilew);
#else
					_DrawBitmap(mr_bitmap[i].p +
					(unTile&MR_SPRITE_INDEX_MASK)*tilew*tileh,
					(int16)drawX, 
					(int16)drawY, 
					(uint16)tilew, 
					(uint16)tileh, 
					(uint16)((unTile & 0xfc00) + 
					((unTile & MR_SPRITE_TRANSPARENT)? BM_TRANSPARENT:BM_COPY)),
					(uint16)*(mr_bitmap[i].p), 0, 0, (uint16)tilew);
#endif
			}
		}
	}//for (dy = yStart; dy < yEnd; dy++)
	return 0;
}

static int MRF_TileShift(mrp_State* L)
{
	uint16 i = ((uint16)  mrp_tonumber(L,1));
	uint16 mode = ((uint16)  mrp_tonumber(L,2));

	int32 j;
#ifdef MYTHROAD_DEBUG
	if (i >= TILEMAX)
	{
		mrp_pushstring(L, "TileShift:tile index out of rang!");
		mrp_error(L);
		return 0;
	}
#endif
	switch (mode) 
	{
	case 0:    //up
		memmove(mr_map[i], 
			mr_map[i] + mr_tile[i].w, 
			mr_tile[i].w*(mr_tile[i].h-1)*2);
		break;
	case 1:    //down
		memmove(mr_map[i] + mr_tile[i].w, 
			mr_map[i], 
			mr_tile[i].w*(mr_tile[i].h-1)*2);
		break;
	case 2:    //left
		for(j=0;j<mr_tile[i].h;j++){
			memmove(mr_map[i] + mr_tile[i].w * j, 
				mr_map[i] + mr_tile[i].w * j + 1, 
				(mr_tile[i].w*-1)*2);
		}
		break;
	case 3:    //right
		for(j=0;j<mr_tile[i].h;j++){
			memmove(mr_map[i] + mr_tile[i].w * j + 1, 
				mr_map[i] + mr_tile[i].w * j, 
				(mr_tile[i].w*-1)*2);
		}
		break;
	}
	return 0;
}

static int MRF_TileLoad(mrp_State* L)
{
	uint16 i = ((uint16)  mrp_tonumber(L,1));
	char* filename = ((char*)  mrp_tostring(L,2));
	int filelen;

#ifdef MR_BIG_ENDIAN
	uint16 dx,dy,w;
#endif

#ifdef MYTHROAD_DEBUG
	if (i >= TILEMAX)
	{
		mrp_pushstring(L, "TileLoad:tile index out of rang!");
		mrp_error(L);
		return 0;
	}
#endif
	if(mr_map[i])
	{
		MR_FREE(mr_map[i], mr_tile[i].w*mr_tile[i].h*2);
		mr_map[i] = NULL;
	}

	mr_map[i] = _mr_readFile(filename, &filelen, 0);

#ifdef MYTHROAD_DEBUG
	if(!mr_map[i])
	{
		mrp_pushfstring(L, "TileLoad %d:cannot read \"%s\"!", i,filename);
		mrp_error(L);
		return 0;
	}
#endif

#ifdef MR_BIG_ENDIAN
	for (dy = 0; dy < mr_tile[i].h; dy++)
	{
		for (dx = 0; dx < mr_tile[i].w; dx++) 
		{
			w = mr_tile[i].w;
			mr_map[i][w* dy + dx] = ntohs(mr_map[i][w* dy + dx]);
		}
	}
#endif

	if (mr_tile[i].w*mr_tile[i].h*2 != filelen)
	{
		MR_FREE(mr_map[i], filelen);
		mrp_pushfstring(L, "TileLoad: Map file \"%s\" len err %d %d !", filename, filelen, mr_tile[i].w*mr_tile[i].h*2);
		mr_map[i] = NULL;
		mrp_error(L);
		return 0;
	}
	return 0;
}

static int MRF_GetTile(mrp_State* L)
{
	uint16 i = ((uint16)  mrp_tonumber(L,1));
	uint16 x = ((uint16)  mrp_tonumber(L,2));
	uint16 y = ((uint16)  mrp_tonumber(L,3));
#ifdef MYTHROAD_DEBUG
	if (i >= TILEMAX)
	{
		mrp_pushstring(L, "GetTile:tile index out of rang!");
		mrp_error(L);
		return 0;
	}
#endif
#ifdef MYTHROAD_DEBUG
	if(!mr_map[i])
	{
		mrp_pushfstring(L, "GetTile %d:tile is nil!", i);
		mrp_error(L);
		return 0;
	}
	if((y > mr_tile[i].h)||(x > mr_tile[i].w))
	{
		mrp_pushfstring(L, "GetTile overflow!", i);
		mrp_error(L);
		return 0;
	}
#endif
	{
		int16 to_mr_ret = mr_map[i][mr_tile[i].w * y + x];
		to_mr_pushnumber(L,(mrp_Number)to_mr_ret);
	}
	return 1;
}

static int MRF_SetTile(mrp_State* L)
{
	uint16 i = ((uint16)  mrp_tonumber(L,1));
	uint16 x = ((uint16)  mrp_tonumber(L,2));
	uint16 y = ((uint16)  mrp_tonumber(L,3));
	uint16 v = ((uint16)  mrp_tonumber(L,4));
#ifdef MYTHROAD_DEBUG
	if (i >= TILEMAX)
	{
		mrp_pushstring(L, "SetTile:tile index out of rang!");
		mrp_error(L);
		return 0;
	}
#endif
#ifdef MYTHROAD_DEBUG
	if(!mr_map[i])
	{
		mrp_pushfstring(L, "SetTile %d:tile is nil!", i);
		mrp_error(L);
		return 0;
	}
	if((y > mr_tile[i].h)||(x > mr_tile[i].w))
	{
		mrp_pushfstring(L, "SetTile %d overflow!", i);
		mrp_error(L);
		return 0;
	}
#endif
	mr_map[i][mr_tile[i].w * y + x] = v;
	return 0;
}
//tile

//Screen
static int MRF_ClearScreen(mrp_State* L)
{
	int r = ((int)  mrp_tonumber(L,1));
	int g = ((int)  mrp_tonumber(L,2));
	int b = ((int)  mrp_tonumber(L,3));
	DrawRect(0,0,(int16)MR_SCREEN_W,(int16)MR_SCREEN_H,(uint8)r,(uint8)g,(uint8)b);
	return 0;
}
//Screen

//effect
static int MRF_EffSetCon(mrp_State* L) 
{
	int16 x = ((int16)  to_mr_tonumber(L,1,0));
	int16 y = ((int16)  to_mr_tonumber(L,2,0));
	int16 w = ((int16)  to_mr_tonumber(L,3,0));
	int16 h = ((int16)  to_mr_tonumber(L,4,0));
	int16 perr = ((int16)  to_mr_tonumber(L,5,0));
	int16 perg = ((int16)  to_mr_tonumber(L,6,0));
	int16 perb = ((int16)  to_mr_tonumber(L,7,0));
	return _mr_EffSetCon(x,y,w,h,perr,perg,perb);
}
//effect

//math
static int MRF_GetRand(mrp_State* L)
{
	int32 n = ((int32)  mrp_tonumber(L,1));
	{
		int32 to_mr_ret = (int32)  rand() % n;
		to_mr_pushnumber(L,(mrp_Number)to_mr_ret);
	}
	return 1;
}

static int MRF_mod(mrp_State* L)
{
	int n = ((int)  mrp_tonumber(L,1));
	int m = ((int)  mrp_tonumber(L,2));
	{
		int to_mr_ret = (int)  n % m;
		mrp_pushnumber(L,(mrp_Number)to_mr_ret);
	}
	return 1;
}

static int MRF_and(mrp_State* L)
{
	int n = ((int)  mrp_tonumber(L,1));
	int m = ((int)  mrp_tonumber(L,2));
	{
		int to_mr_ret = (int)  n & m;
		mrp_pushnumber(L,(mrp_Number)to_mr_ret);
	}
	return 1;
}

static int MRF_or(mrp_State* L)
{
	int n = ((int)  mrp_tonumber(L,1));
	int m = ((int)  mrp_tonumber(L,2));
	{
		int to_mr_ret = (int)  n | m;
		mrp_pushnumber(L,(mrp_Number)to_mr_ret);
	}
	return 1;
}

static int MRF_not(mrp_State* L)
{
	int n = ((int)  mrp_tonumber(L,1));
	{
		int to_mr_ret = (int)  !n;
		mrp_pushnumber(L,(mrp_Number)to_mr_ret);
	}
	return 1;
}

static int MRF_xor(mrp_State* L)
{
	int n = ((int)  mrp_tonumber(L,1));
	int m = ((int)  mrp_tonumber(L,2));
	{
		int to_mr_ret = (int)  n ^ m;
		mrp_pushnumber(L,(mrp_Number)to_mr_ret);
	}
	return 1;
}
//math

//music
static void SoundSet(mrp_State* L, uint16 i, char * filename, int32 type)
{
	void* filebuf;
	int filelen;

	if(i>=SOUNDMAX){
		mrp_pushfstring(L, "SoundSet :index %d invalid!", i);
		mrp_error(L);
	}

	if(mr_sound[i].p)
	{
		MR_FREE(mr_sound[i].p, mr_sound[i].buflen);
		mr_sound[i].p = NULL;
	}

	if (*filename == '*')
	{
		return ;
	}
	//MRDBGPRINTF("SoundSet:1 %s", filename);
	filebuf = _mr_readFile(filename, &filelen, 0);
	if(!filebuf)
	{
		mrp_pushfstring(L, "SoundSet %d:cannot read \"%s\"!", i,filename);
		mrp_error(L);
		return ;
	}

	mr_sound[i].p = filebuf;
	mr_sound[i].buflen = filelen;
	mr_sound[i].type = type;
	return ;
}

static int MRF_SoundSet(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	char* filename = ((char*)  to_mr_tostring(L,2,0));
	int32 type = ((int32)  to_mr_tonumber(L,3,MR_SOUND_WAV));
	SoundSet(L, i,  filename, type);
	return 0;
}

static int MRF_SoundPlay(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	int32 loop = (int32)  to_mr_toboolean(L, 2, FALSE);
	if(i>=SOUNDMAX){
		mrp_pushfstring(L, "SoundPlay:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	if (!(mr_state == MR_STATE_RUN) || (!mr_soundOn))
	{
		return 0;
	}
	mr_playSound(mr_sound[i].type, mr_sound[i].p, mr_sound[i].buflen, loop);
	return 0;
}

static int MRF_SoundStop(mrp_State* L)
{
	uint16 i = ((uint16)  to_mr_tonumber(L,1,0));
	if(i>=SOUNDMAX){
		mrp_pushfstring(L, "SoundStop:index %d invalid!", i);
		mrp_error(L);
		return 0;
	}
	mr_stopSound(mr_sound[i].type);
	return 0;
}


static int MRF_BgMusicSet(mrp_State* L)
{
	char* filename = ((char*)  to_mr_tostring(L,1,0));
	int32 type = ((int32)  to_mr_tonumber(L,2,MR_SOUND_MIDI));
	SoundSet(L, 0, filename, type);
	return 0;
}

static int MRF_BgMusicStart(mrp_State* L)
{
	//char* filename = ((char*)  to_mr_tostring(L,1,0));
	//int32 loop = (int32)to_mr_tonumber(L, 1, 1);
	int32 loop = (int32)to_mr_toboolean(L, 1, TRUE);
	if (!(mr_state == MR_STATE_RUN) || (!mr_soundOn))
	{
		return 0;
	}
	mr_playSound(mr_sound[0].type, mr_sound[0].p, mr_sound[0].buflen, loop);
	return 0;
}

static int MRF_BgMusicStop(mrp_State* L)
{
	mr_stopSound(mr_sound[0].type);
	return 0;
}
//music

//common
static int MRF_Exit(mrp_State* L)
{
	/*这里调用内存释放，内存的内容不能被
	清空，不然虚拟机会崩溃。如果内存会被
	清空，使用时钟延时释放内存。*/
	//mr_mem_free(LG_mem_base, LG_mem_len);

	//bi = bi|MR_FLAGS_RI;
	if(old_pack_filename[0])
	{
		MEMSET(pack_filename,0,sizeof(pack_filename));
		STRNCPY(pack_filename,old_pack_filename, sizeof(pack_filename) - 1);
		MEMSET(start_filename,0,sizeof(start_filename));
		STRNCPY(start_filename,old_start_filename,sizeof(start_filename)-1);

		mr_timer_p = (void*)"restart";
		MR_TIME_START(100);
		mr_state = MR_STATE_RESTART;
	}else{
		mr_exit();
		mr_state = MR_STATE_STOP;


		//下面的两句话在1943中曾被去掉，但目前已经不知道
		//这两句话为何被去掉，可能和C代码的调用有关
		//现在先恢复这两句话
		//这两句话用于移植层无需担心mr_stop的调用时间；
		mrp_pushstring(L, "Exiting...");
		mrp_error(L);
		//到这里为止
	}

	return 0;
}
//common

//save
static int bufwriter (mrp_State *L, const void* p, size_t sz, void* ud) {
	SaveF *wi = (SaveF *)ud;

	if(mr_write(wi->f, (void*)p, (uint32)sz)<0)
	{
		mrp_pushstring(L, "SaveTable:mr_write failed");
		mr_close(wi->f);
		mrp_error(L);
		return 0;
	}
	return 0;
}

static int SaveTable(mrp_State* L)
{
	SaveF wi;
	char* filename = ((char*)  to_mr_tostring(L,3,0));

	mrp_settop(L, 2);
	/* perms? rootobj? */
	mr_L_checktype(L, 1, MRP_TTABLE);
	/* perms rootobj? */
	//mr_L_checktype(L, 1, MRP_TTABLE);
	/* perms rootobj */

	wi.f = mr_open(filename, MR_FILE_WRONLY | MR_FILE_CREATE);

	if(wi.f == 0)
	{
		//mrp_pushfstring(L, "SaveTable:mr_open \"%s\" failed",filename);
		//mrp_error(L);
		MRDBGPRINTF("SaveTable:mr_open \"%s\" failed",filename);
		return 0;
	}

	mr_store_persist(L, bufwriter, &wi);

	mrp_settop(L, 0);
	mr_close(wi.f);
	mrp_pushnumber(L, MR_SUCCESS);
	return 1;
}

static const char *bufreader(mrp_State *L, void *ud, size_t *sz) 
{
	LoadF *lf = (LoadF *)ud;
	(void)L;
	*sz = mr_read(lf->f, lf->buff, MRP_L_BUFFERSIZE);
	return (*sz > 0) ? lf->buff : NULL;
}

static int LoadTable(mrp_State* L)
{
	LoadF lf;
	char* filename = ((char*)  to_mr_tostring(L,2,0));

	mrp_settop(L, 2);
	mrp_pop(L, 1);
	//mr_L_checktype(L, 1, MRP_TTABLE);
	/* perms rootobj */
	mr_L_checktype(L, 1, MRP_TTABLE);

	lf.f = mr_open(filename, MR_FILE_RDONLY );
	if (lf.f == 0)
	{
		MRDBGPRINTF("LoadTable:mr_open \"%s\" err",filename);
		mrp_settop(L, 0);
		mrp_settop(L, 1);
		return 1;
	}

	mr_store_unpersist(L, bufreader, &lf);

	mr_close(lf.f);
	return 1;
}
//save


//other
static void setfield (mrp_State *L, const char *key, int value) {
	mrp_pushstring(L, key);
	mrp_pushnumber(L, value);
	mrp_rawset(L, -3);
}

static void setstrfield (mrp_State *L, const char *key, const char *value) {
	mrp_pushstring(L, key);
	mrp_pushstring(L, value);
	mrp_rawset(L, -3);
}

static void setlstrfield (mrp_State *L, const char *key, const char *value, int len) {
	mrp_pushstring(L, key);
	mrp_pushlstring(L, value, len);
	mrp_rawset(L, -3);
}


int _mr_GetSysInfo(mrp_State* L)
{
	int width, height;
	mr_userinfo info;
	uint16 font = (uint16)mr_L_optlong(L, 1, MR_FONT_MEDIUM);

	mrp_newtable(L);

	//mrp_pushliteral(L, "_vmver", );
	//mrp_pushnumber(L, MR_VERSION);
	//mrp_rawset(L, -3);
	setfield(L, "vmver", MR_VERSION); 

#ifdef COMPATIBILITY01
	setfield(L, "ScreenW", MR_SCREEN_W);
	setfield(L, "ScreenH", MR_SCREEN_H);
#endif
	setfield(L, "scrw", MR_SCREEN_W);
	setfield(L, "scrh", MR_SCREEN_H);

	mr_getCharBitmap(0x70b9, font, &width, &height);
#ifdef COMPATIBILITY01
	setfield(L, "ChineseWidth", width);
	setfield(L, "ChineseHigh", height);
#endif
	setfield(L, "chw", width);
	setfield(L, "chh", height);

	mr_getCharBitmap(0x0032, font, &width, &height);
#ifdef COMPATIBILITY01
	setfield(L, "EnglishWidth", width);
	setfield(L, "EnglishHigh", height);
#endif
	setfield(L, "ascw", width);
	setfield(L, "asch", height);

#ifdef COMPATIBILITY01
	setstrfield(L, "PackName", pack_filename);
#endif
	setstrfield(L, "packname", pack_filename);

	if (mr_getUserInfo(&info) == MR_SUCCESS)
	{
		MRDBGPRINTF("mr_getUserInfo ok!");
		setstrfield(L, "hsman", info.manufactory);
		setstrfield(L, "hstype", info.type);
		setlstrfield(L, "IMEI", (const char*)info.IMEI, 16);
		setlstrfield(L, "IMSI", (const char*)info.IMSI, 16);
		setfield(L, "hsver", info.ver);

	}else{
		MRDBGPRINTF("mr_getUserInfo failed!");
		setstrfield(L, "hsman", "none");
		setstrfield(L, "hstype", "none");
		setstrfield(L, "IMEI", "00");
		setstrfield(L, "IMSI", "00");
		setfield(L, "hsver", 0);
	}

	return 1;
}

int _mr_GetDatetime(mrp_State* L)
{
	//int width, height;
	mr_datetime datetime;

	if(MR_SUCCESS == mr_getDatetime(&datetime))
	{
		mrp_newtable(L);
		setfield(L, "year", datetime.year);
		setfield(L, "mon", datetime.month);
		setfield(L, "day", datetime.day);
		setfield(L, "hour", datetime.hour);
		setfield(L, "min", datetime.minute);
		setfield(L, "sec", datetime.second);
		return 1;
	}else
	{
		return 0;
	}
}

static int Call(mrp_State* L)
{
	char* number= ((char*)  to_mr_tostring(L,1,0));

	mrp_settop(L, 1);
	mr_call(number);

	return 0;
}

static int LoadPack(mrp_State* L)
{
	char* packname= ((char*)  to_mr_tostring(L,1,0));

#ifdef MR_AUTHORIZATION
	char input[24];
	MR_FILE_HANDLE f;
	int nTmp;

	//这里还要判断是否是ROM或RAM中的MRP文件，若是则不用进行
	//鉴权。
	if(bi & MR_FLAGS_AI){
		f = mr_open(packname, MR_FILE_RDONLY );
		if (f == 0)
		{
			MRDBGPRINTF( "\"%s\" is unauthorized", packname);
			return 0;
		}

		nTmp = mr_seek(f, 52, MR_SEEK_SET);
		if (nTmp < 0)
		{
			mr_close(f);
			_mr_readFileShowInfo( "unauthorized", 0);
			return 0;
		}

		nTmp = mr_read(f, input, sizeof(input));
		if(nTmp != sizeof(input))
		{
			mr_close(f);
			_mr_readFileShowInfo( "unauthorized", 1);
			return 0;
		}

		mr_close(f);
		if(_mr_isMr(input) == MR_SUCCESS){
			mrp_settop(L, 1);
			mrp_pushstring(L, pack_filename);
			STRCPY(pack_filename, packname);
		}else{
			_mr_readFileShowInfo( "unauthorized", 2);
			return 0;
		}
	}else{
		mrp_settop(L, 1);
		mrp_pushstring(L, pack_filename);
		STRCPY(pack_filename, packname);
	}
#else
	mrp_settop(L, 1);
	mrp_pushstring(L, pack_filename);
	STRCPY(pack_filename, packname);
#endif

	//   return 0;
	return 1;
}

/*
返回值
MR_SUCCESS  0    //成功
MR_FAILED   -1    //失败
MR_IGNORE  1     //未准备好
*/
static int SendSms(mrp_State* L)
{
	char* number= ((char*)  to_mr_tostring(L,1,"0"));
	char* content= ((char*)  to_mr_tostring(L,2,"0"));
	int32 flag= ((int32)  to_mr_tonumber(L,3,MR_ENCODE_ASCII));
	mrp_settop(L, 2);

	mrp_pushnumber(L, mr_sendSms(number, content, flag));
	return 1;
}

/*取得网络ID，0 移动，1 联通*/
static int GetNetworkID(mrp_State* L)
{
	int id = mr_getNetworkID();
	mrp_pushnumber(L, id);
	return 1;
}

static int ConnectWAP(mrp_State* L)
{
	char* wap= ((char*)  to_mr_tostring(L,1,0));

	mrp_settop(L, 1);
	mr_connectWAP(wap);

	return 0;
}

static int MRF_RunFile(mrp_State* L)
{
	char* filename = ((char*)  to_mr_tostring(L,1,0));
	char* runfilename = ((char*)  to_mr_tostring(L,2,0));
	char* runfileparameter = ((char*)  to_mr_tostring(L,3,0));

	memset(pack_filename,0,sizeof(pack_filename));
	//strcpy(pack_filename,"i/");//all installed appliation place under root_dir/i/
	//strncat(pack_filename,filename, sizeof(pack_filename) - 3);
	strncpy(pack_filename,filename, sizeof(pack_filename) - 1);
	memset(start_filename,0,sizeof(start_filename));
	strncpy(start_filename,runfilename,sizeof(start_filename)-1);

	memset(start_fileparameter,0,sizeof(start_fileparameter));
	if (runfileparameter){
		strncpy(start_fileparameter,runfileparameter,sizeof(start_fileparameter)-1);

	}

	mr_timer_p = (void*)"restart";
	MR_TIME_START(100);
	//mr_timer_state = MR_TIMER_STATE_RUNNING;
	mr_state = MR_STATE_RESTART;
	return 0;
}

int mr_Gb2312toUnicode(mrp_State* L)
{
	char* text= ((char*)  to_mr_tostring(L,1,0));

	int TextSize;
	uint16 *tempBuf;
	int tempret=0;
	//tempBuf = c2u((const char*)text, &tempret, &TextSize);
	tempBuf = c2u((const char*)text, NULL, &TextSize); 
	if (!tempBuf)
	{
		mrp_pushfstring(L, "Gb2312toUnicode text[0]=%d: err!", *text);
		mrp_error(L);
		return 0;
	}

	mrp_pushlstring(L, (const char*)tempBuf, TextSize);
	MR_FREE((void *)tempBuf, TextSize);

	return 1;
}

static int MRF_plat(mrp_State* L)
{
	int code = ((int)  to_mr_tonumber(L,1,0));
	int param = ((int)  to_mr_tonumber(L,2,0));
	mrp_pushnumber(L, (mrp_Number)mr_plat(code, param));
	return 1;
}

static int MRF_platEx(mrp_State* L)
{
	int32 input_len,output_len, ret;
	int code = ((int)  to_mr_tonumber(L,1,0));
	uint8* input = (uint8*)mr_L_checklstring(L,2,(size_t*)&input_len);
	uint8* output = NULL;
	MR_PLAT_EX_CB cb = NULL;
	output_len = 0;

	ret = mr_platEx(code, input, input_len, &output, &output_len, &cb);

	if(output&&output_len){
		mrp_pushlstring(L, (const char *)output, output_len);
	}else{
		mrp_pushstring(L, "");
	}

	if(cb){
		cb(output, output_len);
	}
	mrp_pushnumber(L, ret);
	return 2;
}


static int MRF_initNet(mrp_State* L)
{
	const char * mode = (const char *)to_mr_tostring(L,1,"cmnet");
	return mropen_socket(L, mode);
}

static int MRF_closeNet(mrp_State* L)
{
	int32 ret;
	ret = mr_closeNetwork();
	mrp_pushstring(L, "socket");
	mrp_rawget(L, MRP_GLOBALSINDEX);  

	//add this for nil socket obj protect.
	if(!mrp_istable(L, -1)){
		mrp_pop(L, 1);
		MRDBGPRINTF("Socket IDLE!");
		to_mr_pushnumber(L,(mrp_Number)ret);
		return 1;
	}
	//end

	mrp_pushstring(L, "state");
	mrp_pushnumber(L, MRSOCK_CLOSED);
	mrp_rawset(L, -3);
	mrp_pop(L, 1);
	to_mr_pushnumber(L,(mrp_Number)ret);
	return 1;
}
#endif

#ifdef SDK_MOD
int mr_sdk(int code, int param)
{
	int ret = 0;
	switch(code)
	{
	case 1:
		//ret = NU_Retrieve_Clock();
		ret = mr_getTime();
		break;
	case 2:
		//ret = NU_Retrieve_Clock();
		ret = 0xdcb512a5;
		break;
	case 100:
		ret = LG_mem_min;
		break;
	case 101:
		ret = LG_mem_top;
		break;
	case 102:
		ret = LG_mem_left;
		break;
	}
	return ret;
}
#endif

static int _mr_TestCom(mrp_State* L, int input0, int input1)
{
	int ret = 0;

	switch(input0)
	{
	case 1:
		ret = mr_getTime();
		break;
	case 2:
		mr_event_function = (MR_EVENT_FUNCTION)input1;
		break;
	case 3:
		mr_timer_function = (MR_TIMER_FUNCTION)input1;
		break;
	case 4:
		mr_stop_function = (MR_STOP_FUNCTION)input1;
		break;
	case 5:
		mr_pauseApp_function = (MR_PAUSEAPP_FUNCTION)input1;
		break;
	case 6:
		mr_resumeApp_function = (MR_RESUMEAPP_FUNCTION)input1;
		break;

#ifdef MR_PLAT_DRAWTEXT
	case 7:

		return  input1;
#endif

#ifdef MR_VIA_MOD
	case 8:
		return input1;
#endif
	case 9:
		mrc_appInfo_st.ram = input1;
		break;
	case 100:
		ret = LG_mem_min;
		break;
	case 101:
		ret = LG_mem_top;
		break;
	case 102:
		ret = LG_mem_left;
		break;
	case 200:
		if (!(mr_state == MR_STATE_RUN ) || (!mr_shakeOn))
		{
			ret = MR_SUCCESS;
			break;
		}
		ret = mr_startShake(input1);
		break;
	case 201:
		//ret = mr_stopShake();
		break;
	case 300:
		mr_soundOn = input1;
		break;
	case 301:
		mr_shakeOn = input1;
		break;
	case 302:
		bi = bi|MR_FLAGS_RI;
		break;
	case 303:
		bi = bi&(~MR_FLAGS_RI);
		break;
	case 304:
		bi = bi|MR_FLAGS_EI;
		break;
	case 305:
		bi = bi&(~MR_FLAGS_EI);
		break;
	case 306:
		mr_sms_return_flag = 1;
		mr_sms_return_val = input1;
		break;
	case 307:
		mr_sms_return_flag = 0;
		break;
	case 400:
		mr_sleep(input1);
		break;
	case 401:
		ret = MR_SCREEN_MAX_W;
		MR_SCREEN_MAX_W = input1;
		break;
	case 404:
		ret = mr_newSIMInd((int16)input1, NULL);
		break;
	case 405:
		ret = mr_closeNetwork();
		break;
	case 406:
		ret = MR_SCREEN_H;
		MR_SCREEN_H = input1;
		break;
	case 407:
		mr_timer_run_without_pause = input1;
		mr_plat(1202, input1);
		break;

	case 408:
		if(mr_bitmap[BITMAPMAX].type == MR_SCREEN_FIRST_BUF){
			mr_bitmap[BITMAPMAX].p = (uint16*)MR_MALLOC(input1);
			if (mr_bitmap[BITMAPMAX].p){
				MR_FREE(mr_screenBuf, mr_bitmap[BITMAPMAX].buflen);
				mr_screenBuf = mr_bitmap[BITMAPMAX].p;
				mr_bitmap[BITMAPMAX].buflen = input1;
				ret = MR_SUCCESS;
			}else{
				ret = MR_FAILED;
			}
		}else if(mr_bitmap[BITMAPMAX].type == MR_SCREEN_SECOND_BUF){
			if (mr_bitmap[BITMAPMAX].buflen >= input1){
				ret = MR_SUCCESS;
			}else{
				ret = MR_FAILED;
			}
		}
		break;


	case 500:
#ifdef MR_SM_SURPORT
		ret = _mr_load_sms_cfg();//only for sm dsm;
		//#ifndef ADI_MOD
		//         if(ret != MR_FAILED){
		//            mr_close(ret);
		//         }
		//#endif
#endif
		break;
	case 503:
#ifdef MR_SM_SURPORT
		{
			uint8 flag=0;
			_mr_smsGetBytes(CFG_USE_UNICODE_OFFSET, (char*)&flag, 1);
			ret = flag;
		}
#endif
		break;
	case 504:
		ret = _mr_save_sms_cfg(input1);
		break;
	case 3629:
		if (input1 == 2913)
			bi = bi|MR_FLAGS_BI;
		break;
	case 3921:
		if (input1 == 98352)
			bi = bi|MR_FLAGS_AI;
		break;
	case 3251:
		if (input1 == 648826)
			bi = bi&(~MR_FLAGS_AI);
		break;
	}


	return ret;
}

int32 _mr_c_function_new(MR_C_FUNCTION f, int32 len)
{
	if (mr_c_function_P){
		MR_FREE(mr_c_function_P, mr_c_function_P_len);
	}
	mr_c_function_P = MR_MALLOC(len);
	if(!mr_c_function_P)
	{
#if 0
		mrp_pushfstring(vm_state, "c_function:No memory!");
		mrp_error(vm_state);
#endif
		mr_state = MR_STATE_ERROR;
		return MR_FAILED;
	}
	mr_c_function_P_len = len;
	MEMSET(mr_c_function_P, 0, mr_c_function_P_len);
	mr_c_function = f;
#ifdef SDK_MOD
	*((void**)(sdk_mr_c_function_table) -1) = mr_c_function_P;
#else
	if (mr_c_function_fix_p){
		*((void**)(mr_c_function_fix_p) + 1) = mr_c_function_P;
	}else{
		*((void**)(mr_load_c_function) -1) = mr_c_function_P;
	}
#endif

	return MR_SUCCESS;
}


static int _mr_TestCom1(mrp_State* L, int input0, char* input1, int32 len)
{
	int ret = 0;
	//mr_printf("strCom:%d", input0);
	//mr_printf("strCom:%s", input1);
	//mr_printf("strCom:%d", len);

	switch(input0)
	{
	case 2:
		if(mr_ram_file){
			MR_FREE(mr_ram_file, mr_ram_file_len);
			mr_ram_file = NULL;
		}
		mr_ram_file = input1;
		mr_ram_file_len = len;
		break;
	case 3:
		{
#if 0
			uint8* start_filename = ((uint8*)  mr_L_optstring(L,3,MR_START_FILE));
			MEMSET(old_pack_filename,0,sizeof(old_pack_filename));
			if(input1){
				STRNCPY(old_pack_filename,input1, sizeof(old_pack_filename) - 1);
			}
			MEMSET(old_start_filename,0,sizeof(old_start_filename));
			STRNCPY(old_start_filename,start_filename,sizeof(old_start_filename)-1);
			break;
#else
			MEMSET(old_pack_filename,0,sizeof(old_pack_filename));
			if(input1){
				STRNCPY(old_pack_filename,input1, sizeof(old_pack_filename) - 1);
			}
			MEMSET(old_start_filename,0,sizeof(old_start_filename));
			STRNCPY(old_start_filename,MR_START_FILE,sizeof(old_start_filename)-1);
			break;

#endif
		}
	case 4:
		{
			MEMSET(start_fileparameter,0,sizeof(start_fileparameter));
			if(input1){
				STRNCPY(start_fileparameter,input1, sizeof(start_fileparameter) - 1);
			}
			break;
		}
		//1948 add exception set
	case 5:
		mr_exception_str = input1;
		break;
	case 6:
		mr_exception_str = NULL;
		break;
		//1948 add exception set
	case 9:
#ifdef  MR_MSTAR_MOD
		{
			//extern void sys_Invalidate_data_cache(void);

			//sys_Invalidate_data_cache();
			mr_cacheSync((void*)input1,len);	
		}
#endif            

#ifndef MR_MSTAR_MOD
#ifndef MR_BREW_MOD
#ifdef MTK_MOD
		{
			//extern int32 clean_arm9_dcache(uint32 addr, uint32 len);
			//extern int32 invalidate_arm9_icache(int32 addr, int32 len);

			//clean_arm9_dcache((uint32)((uint32)(input1)&(~0x0000001F)), 
			//                                          ((len+0x0000001F*3)&(~0x0000001F)));
			//invalidate_arm9_icache((uint32)((uint32)(input1)&(~0x0000001F)), 
			//                                          ((len+0x0000001F*3)&(~0x0000001F)));
			mr_cacheSync((void*)((uint32)(input1)&(~0x0000001F)), 
				((len+0x0000001F*3)&(~0x0000001F)));		   
		}
#endif
#endif
#endif

#ifdef MR_SPREADTRUM_MOD
		{
			// 移植层提供的函数， 用来同步Cache的。
			mr_cacheSync(NULL,0);
		}
#endif

#ifdef MR_HT_MOD
		{
			mr_cacheSync(NULL,0);
		}
#endif

#ifdef MR_BREW_MOD
		mr_cacheSync(NULL,0);
#endif

		return 0;

	case 200:
		mr_updcrc(NULL, 0);           /* initialize crc */
		mr_updcrc((unsigned char*)input1, len);
		ret = mr_updcrc((unsigned char*)input1, 0);
		break;
	case 300:
		{
#if 0
			uint32 unzip_len;
			mr_gzInBuf = (uint8*)input1;
			LG_gzoutcnt = 0;
			LG_gzinptr = 0;

			ret = mr_get_method(len);
			if (ret < 0) 
			{
				mrp_pushlstring(L, input1, len);
				return 1;             
			}

#ifdef MR_PKZIP_MAGIC
			if (mr_zipType == PACKED){
				unzip_len = LG(mr_gzInBuf + LOCLEN);
				mr_gzOutBuf = MR_MALLOC(unzip_len);
			}else{
				//unzip_len  = *(uint32*)(input1 + len - 4);
				MEMCPY(&unzip_len, (input1 + len - 4), 4);
				//MRDBGPRINTF("unzip_len1 = %d", unzip_len);
#ifdef MR_BIG_ENDIAN
				unzip_len  = ntohl(unzip_len);
#endif
				//MRDBGPRINTF("unzip_len2 = %d", unzip_len);

				mr_gzOutBuf = MR_MALLOC(unzip_len);
			}
#else
			//unzip_len  = *(uint32*)(input1 + len - 4);
			MEMCPY(&unzip_len, (input1 + len - 4), 4);
			//MRDBGPRINTF("unzip_len1 = %d", unzip_len);
#ifdef MR_BIG_ENDIAN
			unzip_len  = ntohl(unzip_len);
#endif
			//MRDBGPRINTF("unzip_len2 = %d", unzip_len);

			mr_gzOutBuf = MR_MALLOC(unzip_len);
#endif



			if(mr_gzOutBuf == NULL)
			{
				//MR_FREE(mr_gzInBuf, oldlen);
				//MR_FREE(mr_gzOutBuf, ret);
				MRDBGPRINTF("unzip  Not memory unzip!");
				return 0;
			}
			if (mr_unzip() != 0) {
				MR_FREE(mr_gzOutBuf, unzip_len);
				MRDBGPRINTF("unzip:  Unzip err1!");
				return 0;
			}

			mrp_pushlstring(L, (const char*)mr_gzOutBuf, unzip_len);
			MR_FREE(mr_gzOutBuf, unzip_len);
			return 1;             
#endif
			break;
		}
#if 0
	case 500:
		{
			md5_state_t state;
			md5_byte_t digest[16];

			mr_md5_init(&state);
			mr_md5_append(&state, (const md5_byte_t *)input1, len);
			mr_md5_finish(&state, digest);
			mrp_pushlstring(L, (const char*)digest, 16);
			return 1;             
		}
		break;
	case 501:
		{
			int32 outlen = len * 4  / 3 + 8;
			uint8 *buf = MR_MALLOC(outlen);
			if(!buf){
				return 0;
			}
			ret = _mr_encode((uint8*)input1, (uint32)len, (uint8*)buf);
			if(ret == MR_FAILED){
				MR_FREE(buf, outlen);
				return 0;
			}
			mrp_pushlstring(L, (const char*)buf, ret);
			MR_FREE(buf, outlen);
			return 1;             
		}
		break;
	case 502:
		{
			uint8 *buf = MR_MALLOC(len);
			if(!buf){
				return 0;
			}
			ret = _mr_decode((uint8*)input1, (uint32)len, (uint8*)buf);
			if(ret == MR_FAILED){
				MR_FREE(buf, len);
				return 0;
			}
			mrp_pushlstring(L, (const char*)buf, ret);
			MR_FREE(buf, len);
			return 1;             
		}
		break;
	case 600:
		{
			char* mr_m0_file;
			if (input1[0] == '*'){/*m0 file?*/
				int32 index = input1[1]-0x41;
				if ((index >= 0) && (index < (sizeof(mr_m0_files)/sizeof(const unsigned char *)))){
					mr_m0_file = (char*)mr_m0_files[index]; //这里定义文件名为*A即是第一个m0文件
					//*B是第二个.........
				}else{
					mr_m0_file = NULL;
				}
			}else{
				mr_m0_file = mr_ram_file;
			}

			if (mr_m0_file){
				int32 offset = ((int32)  to_mr_tonumber(L,3,0));
				int32 buflen = ((int32)  to_mr_tonumber(L,4,0));
				if((buflen == -1) && (input1[0] == '$')){
					buflen = mr_ram_file_len;
				}
				mrp_pushlstring(L, (const char*)mr_m0_file + offset, buflen);
				return 1;
			}else{
				return 0;
			}
		}
		break;

	case 601:
		{
			char* filebuf;
			filebuf = _mr_readFile((const char *)input1, &ret, 0);
			//MRDBGPRINTF("1base=%d,end=%d",  (int32)LG_mem_base, (int32)LG_mem_end);   
			//MRDBGPRINTF( "filebuf  =%d", filebuf);
			if(filebuf)
			{
				mrp_pushlstring(L, filebuf, ret);
				//MRDBGPRINTF( "filebuf  =%d", filebuf);
				//MRDBGPRINTF( "filelen  =%d", ret);
				MR_FREE(filebuf, ret);
				//MRDBGPRINTF("100base=%d,end=%d",  (int32)LG_mem_base, (int32)LG_mem_end);   
				//MRDBGPRINTF( "601 free ok");
			}else{
				mrp_pushnil(L);
			}
			return 1;
		}
		break;
	case 602:
		{
			if(_mr_readFile((const char *)input1, &ret, 1)==NULL)
			{
				mrp_pushnil(L);
			}else{
				mrp_pushnumber(L, MR_SUCCESS);
			}
			return 1;
		}
		break;
	case 603:
		{
			char* filebuf;
			filebuf = _mr_readFile((const char *)input1, &ret, 2);
			if(filebuf)
			{
				mrp_pushnumber(L, (mrp_Number)filebuf);
				mrp_pushnumber(L, (mrp_Number)ret);
				return 2;
			}else{
				mrp_pushnil(L);
				return 1;
			}
		}
		break;
#endif
	case 700:
		{
#if 0
			int type = ((int)  to_mr_tonumber(L,3,0));
			ret = mr_newSIMInd(type, (uint8*)input1);
#else
			int type = len;
			ret = mr_newSIMInd(type, (uint8*)input1);
#endif
			break;
		}
	case 701:
		{
#if 0
			uint8* pNum = ((uint8*)  mrp_tostring(L,3));
			int32 type = ((int32)  mr_L_optnumber(L, 4, MR_ENCODE_ASCII));
			ret = mr_smsIndiaction((uint8*)input1, len, pNum, type);
#else
			uint8* pNum = ((uint8*)  (input1+1));
			int32 type = *((int32*)  (input1+2));
			ret = mr_smsIndiaction((uint8*)input1, len, pNum, type);
#endif
			break;
		}
#if 0
#ifdef  SDK_MOD
	case 800:
		{
			int32 input_len,output_len, ret;
			int code = ((int)  mr_L_optint(L,3,0));
			mr_load_c_function = mr_c_function_load;
			*((void**)(input1)) = _mr_c_function_table;
			sdk_mr_c_function_table = input1 + 8;

			ret = mr_load_c_function(code);

			mrp_pushnumber(L, ret);
			return 1;
		}
		break;
	case 801:
		{
			int32 input_len,output_len, ret;
			int code = ((int)  to_mr_tonumber(L,3,0));
			//uint8* input = (uint8*)mr_L_checklstring(L,4,(size_t*)&input_len);
			uint8* output = NULL;
			output_len = 0;

			ret = mr_c_function(mr_c_function_P, code, input1, len, &output, &output_len);

			if(output&&output_len){
				mrp_pushlstring(L, (const char *)output, output_len);
			}else{
				mrp_pushstring(L, "");
			}

			mrp_pushnumber(L, ret);
			return 2;
		}
		break;
#else
	case 800:
		{
			int32 input_len,output_len, ret;
			int code = ((int)  mr_L_optint(L,3,0));
			mr_load_c_function = (MR_LOAD_C_FUNCTION)(input1+8);
			*((void**)(input1)) = (void*)_mr_c_function_table;

			ret = mr_load_c_function(code);

			mrp_pushnumber(L, ret);
			return 1;
		}
		break;
	case 801:
		{
			int32 input_len,output_len, ret;
			int code = ((int)  to_mr_tonumber(L,3,0));
			//uint8* input = (uint8*)mr_L_checklstring(L,4,(size_t*)&input_len);
			uint8* output = NULL;
			output_len = 0;

			ret = mr_c_function(mr_c_function_P, code, (uint8*)input1, len, (uint8**)&output, &output_len);

			if(output&&output_len){
				mrp_pushlstring(L, (const char *)output, output_len);
			}else{
				mrp_pushstring(L, "");
			}

			mrp_pushnumber(L, ret);
			return 2;
		}
		break;
	case 802:
		{
			int32 input_len,output_len, ret;
			int code = ((int)  mr_L_optint(L,3,0));
			mr_c_function_fix_p = ((int32*)  mr_L_optint(L,4,0));
			mr_load_c_function = (MR_LOAD_C_FUNCTION)(input1+8);
			*((void**)(mr_c_function_fix_p)) = (void*)_mr_c_function_table;

			ret = mr_load_c_function(code);

			mrp_pushnumber(L, ret);
			return 1;
		}
		break;
#endif
#endif
	case 900:
		ret = mr_platEx(200001, (uint8*)_mr_c_port_table, sizeof(_mr_c_port_table), NULL, NULL, NULL);
		break;
	}


	return ret;
}

#ifdef SDK_MOD
extern void mr_c_function_load(void);
#endif

static int _mr_TestComC(int input0, char* input1, int32 len, int32 code)
{
	int ret = 0;
	//mr_printf("strCom:%d", input0);
	//mr_printf("strCom:%s", input1);
	//mr_printf("strCom:%d", len);

	switch(input0)
	{
#ifdef  SDK_MOD
	case 800:
		{
			int32 input_len,output_len, ret;
			//int code = ((int)  mr_L_optint(L,3,0));
			mr_load_c_function = mr_c_function_load;
			*((void**)(input1)) = _mr_c_function_table;
			sdk_mr_c_function_table = input1 + 8;

			ret = mr_load_c_function(code);

#if 0
			mrp_pushnumber(L, ret);
			return 1;
#endif
		}
		break;
	case 801:
		{
			int32 input_len,output_len, ret;
			//int code = ((int)  to_mr_tonumber(L,3,0));
			uint8* output = NULL;
			output_len = 0;

			ret = mr_c_function(mr_c_function_P, code, input1, len, &output, &output_len);

#if 0
			if(output&&output_len){
				mrp_pushlstring(L, (const char *)output, output_len);
			}else{
				mrp_pushstring(L, "");
			}

			mrp_pushnumber(L, ret);
			return 2;
#endif
		}
		break;
#else
	case 800:
		{
			int32 ret;
			//int code = ((int)  mr_L_optint(L,3,0));
			mr_load_c_function = (MR_LOAD_C_FUNCTION)(input1+8);
			*((void**)(input1)) = (void*)_mr_c_function_table;

#ifdef  MR_MSTAR_MOD
			{
				//extern void sys_Invalidate_data_cache(void);


				//mr_printf("load addr:%x", (int)mr_load_c_function);

				//sys_Invalidate_data_cache();
				mr_cacheSync((void*)input1,len);
			}
#endif            

#ifndef MR_MSTAR_MOD
#ifndef MR_BREW_MOD
#ifdef MTK_MOD
			{
				//extern int32 clean_arm9_dcache(uint32 addr, uint32 len);
				//extern int32 invalidate_arm9_icache(int32 addr, int32 len);

				//clean_arm9_dcache((uint32)((uint32)(input1)&(~0x0000001F)), 
				//                                          ((len+0x0000001F*3)&(~0x0000001F)));
				//invalidate_arm9_icache((uint32)((uint32)(input1)&(~0x0000001F)), 
				//                                          ((len+0x0000001F*3)&(~0x0000001F)));
				mr_cacheSync((void*)((uint32)(input1)&(~0x0000001F)), 
					((len+0x0000001F*3)&(~0x0000001F)));
			}
#endif
#endif
#endif

#ifdef MR_SPREADTRUM_MOD
			{
				// 移植层提供的函数， 用来同步Cache的。
				mr_cacheSync(NULL,0);
			}
#endif       

#ifdef MR_BREW_MOD
			mr_cacheSync(NULL,0);
#endif

			ret = mr_load_c_function(code);

		}
		break;
	case 801:
		{
			int32 output_len, ret;
			//int code = ((int)  to_mr_tonumber(L,3,0));
			uint8* output = NULL;
			output_len = 0;

			ret = mr_c_function(mr_c_function_P, code, (uint8*)input1, len, (uint8**)&output, &output_len);
		}
		break;
#endif
	}
	return ret;
}


//other


typedef enum {
	MRP_ORICODING,//直接读取编码。只有斯凯的特权用户方可执行该类型操作。
	MRP_FILENAME,//MRP文件名称，最大11个有效字符，不包括'\0'
	MRP_APPNAME,//MRP应用名称，用户可见的应用名，最大23个有效字符，不包括'\0'，例如"雷电1944"。
	MRP_APPID,//每个应用对应一个唯一的ID，uint32类型。
	MRP_APPVER,//uint32类型   
	MRP_VENDER,//开发商信息，最大39个有效字符，不包括'\0'。
	MRP_DESCRIPTION,   //应用描述信息 ，最大63个有效字符
	MRP_VERSION,//计费通道版本编号,uint16类型，两个字节。
	MRP_CHANNEL,//计费通道编号,两个字节，低地址的RecBuf[0]是移动，高地址的RecBuf[1]是联通。
	MRP_RAM,//MRP启动需要的RAM大小
	MRP_NONE
}E_MRP_INFOID;

/*
函数名称:		mrc_GetMrpInfo

函数功能:		从当前MRP文件中读取头信息。

输入参数:		CMD，操作命令码。
RecBuf，接收返回数据的缓冲区指针；
BufLen，接收返回数据的缓冲区字节数；

返回值:			当操作不支持、操作失败等情况时，函数返回值小于等于0；
若操作成功，函数返回值为RecBuf中实际填写的字节数。
备注:
该函数尚未实现。
*/
/*
函数名称:		mrc_GetMrpInfo

函数功能:		从当前MRP文件中读取头信息。

输入参数:		CMD，操作命令码。
RecBuf，接收返回数据的缓冲区指针；
BufLen，接收返回数据的缓冲区字节数；

返回值:			当操作不支持、操作失败等情况时，函数返回值小于等于0；
若操作成功，函数返回值为RecBuf中实际填写的字节数。
*/
#define MRP_FILENAME_LEN	12
static int32 mrc_GetMrpInfoClose(int32 IsFixed,int32 handle)
{
	if(!handle)
		return MR_FAILED;
	if(IsFixed)
		return MR_SUCCESS;
	else
		return mr_close(handle);
}

static int32 mrc_GetMrpInfoOpen(char *MrpName,int32 *IsFixed)
{
	uint32 Handle=0;
	//LIB_EXB_LOG(("mrc_GetMrpInfoOpen:开始入参检查。"));
	if(!IsFixed||!MrpName)
		return 0;
	//LIB_EXB_LOG(("mrc_GetMrpInfoOpen:Open file %s",MrpName));
	if (MrpName[0] == '*')
	{/*m0 file?*/
		*IsFixed=1;     
		Handle=(uint32)mr_m0_files[MrpName[1]-0x41];
		//LIB_EXB_LOG(("mrc_GetMrpInfoOpen:Open * file %s,Handle=%0x",MrpName,Handle));
	}else if(MrpName[0]=='$')
	{
		*IsFixed=1;     
		Handle=(uint32)mr_ram_file;
		//LIB_EXB_LOG(("mrc_GetMrpInfoOpen:Open $ file %s,Handle=%0x",MrpName,Handle));
	}
	else
	{
		*IsFixed=0; 
		Handle=mr_open(MrpName,MR_FILE_RDONLY);
		//LIB_EXB_LOG(("mrc_GetMrpInfoOpen:Open normal file %s,Handle=%0x",MrpName,Handle));
	}
	//LIB_EXB_LOG(("mrc_GetMrpInfoOpen:ret Handle %d",Handle));    
	return Handle;
}

static int32 mrc_GetMrpInfoRead(int32 IsFixed,int32 Handle,int32 Offset,uint8 *RecBuf,int32 BufLen)
{
	int32 ret;
	//LIB_EXB_LOG(("mrc_GetMrpInfoRead:入参检查。"));  				
	if(!RecBuf)
		return MR_FAILED;

	if(IsFixed)
	{
		memcpy(RecBuf,(uint8 *)Handle+Offset,BufLen);
		ret=BufLen;
		//LIB_EXB_LOG(("mrc_GetMrpInfoRead:从固化读取到。"));  			
	}else
	{
		ret=mr_seek(Handle,Offset,MR_SEEK_SET);
		if(MR_SUCCESS!=ret)
			return MR_FAILED;
		ret=mr_read(Handle,RecBuf,BufLen);
		if(ret!=BufLen)
			return MR_FAILED;
		//LIB_EXB_LOG(("mrc_GetMrpInfoRead:从文件读取到%d个字节。",ret));  			
	}
	return ret;		
}


int32 mrc_GetMrpInfoEx(int32 IsFixed, int32 Handle, E_MRP_INFOID CMD,uint8 *RecBuf,int32 BufLen)
{
	int32 ret=MR_FAILED,DotOffset=0;
	uint32 temp,*p32;
	//LIB_EXB_LOG(("mrc_GetMrpInfo:开始入参检查。"));
	memset(RecBuf,0,BufLen);
	switch(CMD)
	{
#if 0
	case MRP_FILENAME://MRP文件名称，最大11个有效字符，不包括'\0'
		if(BufLen<12)
			break;	
		ret=mrc_GetMrpInfoRead(IsFixed,Handle,16,RecBuf,12);
		pDot= (char*)strrchr((char *)RecBuf,'.');
		DotOffset = (int32)pDot -(int32)RecBuf ;
		memset(RecBuf+DotOffset,0,BufLen-DotOffset);			
		break;		
	case MRP_APPNAME://MRP应用名称，用户可见的应用名，最大23个有效字符，不包括'\0'。
		if(BufLen<24)
			break;	
		ret=mrc_GetMrpInfoRead(IsFixed,Handle,28,RecBuf,24);
		break;		
#endif
	case MRP_APPID://每个应用对应一个唯一的ID，uint32类型。
		if(BufLen<4)
			break;	
		ret=mrc_GetMrpInfoRead(IsFixed,Handle,192,RecBuf,4);
		p32=(uint32 *)RecBuf;
		temp=*p32;
#ifndef MR_SPREADTRUM_MOD//LIB_SPREADTRUM_MOD
		//MTK平台，需要转为网络字节序
		temp=htonl(temp);
		//LIB_DBG(("MTK平台，转换为网络字节序后temp=%0x",temp));
#endif			
		memcpy(RecBuf,&temp,4);
		break;		
	case MRP_APPVER://uint32类型   
		if(BufLen<4)
			break;	
		ret=mrc_GetMrpInfoRead(IsFixed,Handle,196,RecBuf,4);
		p32=(uint32 *)RecBuf;
		temp=*p32;
#ifndef MR_SPREADTRUM_MOD//LIB_SPREADTRUM_MOD
		//MTK平台，需要转为网络字节序
		temp=htonl(temp);
		//LIB_DBG(("MTK平台，转换为网络字节序后temp=%0x",temp));
#endif	
		memcpy(RecBuf,&temp,4);			
		break;		
	case MRP_RAM://uint32类型   
		{
			uint16 ram,ram_check;
			if(BufLen<4)
				break;	
			ret=mrc_GetMrpInfoRead(IsFixed,Handle,228,(uint8*)&ram,2);
			ret=mrc_GetMrpInfoRead(IsFixed,Handle,230,(uint8*)&ram_check,2);
#ifndef MR_SPREADTRUM_MOD//LIB_SPREADTRUM_MOD
			//MTK平台，需要转为网络字节序
			ram=htonl(ram);
			ram_check=htonl(ram_check);
			//LIB_DBG(("MTK平台，转换为网络字节序后temp=%0x",temp));
#endif	
			mr_updcrc(NULL, 0);           /* initialize crc */
			mr_updcrc((unsigned char*)&ram, 2);
			mr_updcrc((unsigned char*)&ram, 2);
			if(ram_check == (mr_updcrc((unsigned char*)&ram, 0)&0xFF)){
				*(uint32 *)RecBuf = ram;
			}
			break;		
		}
#if 0
	case MRP_VENDER://开发商信息，最大39个有效字符，不包括'\0'。
		if(BufLen<40)
			break;	
		ret=mrc_GetMrpInfoRead(IsFixed,Handle,88,RecBuf,40);
		break;		
	case MRP_DESCRIPTION:  //应用描述信息 ，最大63个有效字符
		if(BufLen<64)
			break;	
		ret=mrc_GetMrpInfoRead(IsFixed,Handle,128,RecBuf,64);	
		break;
	case MRP_VERSION://
		//200:char[2]  =   计费通道版本号，网络字节序				
		if(BufLen<2)
			break;	
		ret=mrc_GetMrpInfoRead(IsFixed,Handle,200,RecBuf,2);
		data=RecBuf[0]*256+RecBuf[1];
		memcpy(RecBuf,&data,sizeof(data));	
		break;	
	case MRP_CHANNEL:			
		if(BufLen<2)
			break;	
		//在原有方案中，202:char[2]  =   计费通道编号，网络字节序								
		//ret=mrc_GetMrpInfoRead(IsFixed,Handle,202,RecBuf,2);
		//data=RecBuf[0]*256+RecBuf[1];
		//MR_MEMCPY(RecBuf,&data,sizeof(data));				
		//在自动切换方案中，计费通道编号,两个字节，低地址的RecBuf[0]是移动，高地址的RecBuf[1]是联通。				
		RecBuf[0]=0;
		RecBuf[1]=0;
		ret=2;
		mrc_GetMrpInfoRead(IsFixed,Handle,203,RecBuf,1);
		mrc_GetMrpInfoRead(IsFixed,Handle,209,RecBuf+1,1);
		break;	
#endif
	default:
		;
	}
	return ret;	
}


/*
def pay_getVersion(mrpfilename)
local firstchar = string.sub(mrpfilename,1,1)
local sId,appId,appVer

if firstchar == "*" || firstchar == "$" then
sId = _strCom(600,mrpfilename,68,4)
appId = _strCom(600,mrpfilename,192,4)
appVer = _strCom(600,mrpfilename,196,4)
else
local f = file.open(mrpfilename, 1)
if f then
f:seek(0, 68)
sId = f:read(4) 
f:seek(0, 192)
appId = f:read(4) 
f:seek(0, 196)
appVer = f:read(4) 
f:close()
end       
end
if sId then
return sId,appId,appVer
else
return
end
end
*/

static int32 getAppInfo()
{
	int32 IsFixed=0,Handle=0;

	Handle=mrc_GetMrpInfoOpen(pack_filename,&IsFixed);
	if(Handle==0)
	{
		//LIB_EXB_LOG(("mrc_GetMrpInfo:mrc_GetMrpInfoOpen返回NULL。"));
		return MR_FAILED;
	}
	//LIB_EXB_LOG(("mrc_GetMrpInfoOpen:成功返回句柄"));   
	mrc_GetMrpInfoEx(IsFixed, Handle, MRP_APPID, (uint8 *)&mrc_appInfo_st.id, 4);
	mrc_GetMrpInfoEx(IsFixed, Handle, MRP_APPVER, (uint8 *)&mrc_appInfo_st.ver, 4);
	if (mrc_appInfo_st.ram==0){
		mrc_GetMrpInfoEx(IsFixed, Handle, MRP_RAM, (uint8 *)&mrc_appInfo_st.ram, 4);
	}
	//mrc_GetMrpInfo(IsFixed, Handle, MRP_FILENAME, (uint8 *)&mrc_appInfo_st.sidName[0], 12);
	mrc_appInfo_st.sidName = NULL;
	mrc_GetMrpInfoClose(IsFixed,Handle);
	return MR_SUCCESS;
}

uint32 mrp_version (void) {
	return MR_VERSION;
}

//main

int32 mr_doExt(char* extName)
{
	char* filebuf;
	int32 filelen, ret;

	filebuf = _mr_readFile((const char*)extName, (int*)&filelen, 0);
	if(!filebuf)
	{
		MRDBGPRINTF("err:%d",11001);
		return MR_FAILED;
	}
	_mr_TestCom(NULL, 3629,2913);


	if (_mr_TestComC( 800, filebuf, filelen, 0) == 0){
		_mr_TestComC( 801, filebuf, MR_VERSION, 6);
		_mr_TestComC( 801, (char*)&mrc_appInfo_st, sizeof(mrc_appInfo_st), 8);
		ret = _mr_TestComC( 801, filebuf, MR_VERSION, 0);
	}else{
		MRDBGPRINTF("err:%d",11002);
		return MR_FAILED;
	}
	return MR_SUCCESS;
}


static int32 _mr_intra_start(char* appExName, const char* entry)
{
	int i,ret;

#ifdef MR_PLAT_READFILE
	mr_flagReadFileForPlat = FALSE;
#endif

	//MRDBGPRINTF("restart timer15");
	//mr_sleep(50);


#if defined(MR_BREW_OTA_MOD)
	{
		extern int32 __rt_lib_init(char* heapbase, char* heaptop);
		static char buf[96];
		__rt_lib_init(buf, buf+96);
	}
#endif

	getAppInfo();
	//ret = mr_plat(1250, mrc_appInfo_st.ram);


	Origin_LG_mem_len = _mr_getMetaMemLimit();

	if (_mr_mem_init_ex(mrc_appInfo_st.ram) != MR_SUCCESS)
	{
		return MR_FAILED;
	}

	mr_event_function = NULL;
	mr_timer_function = NULL;
	mr_stop_function = NULL;
	mr_pauseApp_function = NULL;
	mr_resumeApp_function = NULL;

	mr_ram_file=NULL;

	mr_c_function_P = NULL;
	mr_c_function_P_len  = 0;
	mr_c_function_fix_p = NULL;

	mr_exception_str = NULL;

#ifdef SDK_MOD
	MRDBGPRINTF("Total memory:%d", LG_mem_len);
#endif

	{
		int32 len = 0;
		mr_screenBuf = NULL;
		if(mr_platEx(1001, NULL, 0, (uint8**)&mr_screenBuf, &len, NULL) == MR_SUCCESS){
			if((mr_screenBuf != NULL) && (len >= MR_SCREEN_MAX_W*MR_SCREEN_H*MR_SCREEN_DEEP)){
				mr_bitmap[BITMAPMAX].type = MR_SCREEN_SECOND_BUF;
				mr_bitmap[BITMAPMAX].buflen = len;
			}else if(mr_screenBuf != NULL){
				mr_platEx(1002, (uint8*)mr_screenBuf, len, (uint8**)NULL, NULL, NULL);
				mr_screenBuf = NULL;
			}
		}
		if(mr_screenBuf == NULL){
			mr_screenBuf = (uint16*)MR_MALLOC(MR_SCREEN_MAX_W*MR_SCREEN_H*MR_SCREEN_DEEP);
			mr_bitmap[BITMAPMAX].type = MR_SCREEN_FIRST_BUF;
			mr_bitmap[BITMAPMAX].buflen = MR_SCREEN_MAX_W*MR_SCREEN_H*MR_SCREEN_DEEP;
		}
	}

	mr_bitmap[BITMAPMAX].p = mr_screenBuf;
	mr_bitmap[BITMAPMAX].h = mr_screen_h;
	mr_bitmap[BITMAPMAX].w = mr_screen_w;

	vm_state = NULL;
	mr_timer_state = MR_TIMER_STATE_IDLE;
	mr_timer_run_without_pause = FALSE;
	bi = bi&MR_FLAGS_AI;
	MEMSET(mr_bitmap, 0, sizeof(mr_bitmapSt)*BITMAPMAX);
	MEMSET(mr_sound, 0, sizeof(mr_sound));

	MEMSET(mr_sprite, 0, sizeof(mr_sprite));
	MEMSET(mr_tile, 0, sizeof(mr_tile));
	MEMSET(mr_map, 0, sizeof(mr_map));

	for(i=0;i<TILEMAX;i++)
	{
		mr_tile[i].x1 = 0;
		mr_tile[i].y1 = 0;
		mr_tile[i].x2 = (int16)MR_SCREEN_W;
		mr_tile[i].y2 = (int16)MR_SCREEN_H;
	}

	//入口变量
	if(!entry)
	{
		entry = "_dsm";
	}
	STRNCPY(mr_entry, entry, sizeof(mr_entry)-1);
	//入口变量

#ifdef SDK_MOD
	MRDBGPRINTF("Used by VM(include screen buffer):%d bytes", LG_mem_len - LG_mem_left);
#endif

	mr_state = MR_STATE_RUN;




	//ret = mrp_dofile(vm_state, appExName);
	ret = mr_doExt(appExName);
	if (0 != ret)
		ret = mr_doExt("logo.ext"); //尝试加载 logo.ext

	//这里需要完善
	if(ret != 0)
	{
		mr_state = MR_STATE_ERROR;
		mr_stop();
		MRDBGPRINTF("init failed");
		mr_connectWAP(MR_ERROR_WAP);
		return MR_FAILED;
	}

	return MR_SUCCESS;
}

int32 mr_start_dsm_ex(const char* filename, const char* entry)
{
	mr_screeninfo screeninfo;
	if (mr_getScreenInfo(&screeninfo) != MR_SUCCESS)
	{
		return MR_FAILED;
	}
	mr_screen_w = screeninfo.width;
	mr_screen_h = screeninfo.height;
	mr_screen_bit = screeninfo.bit;

	MEMSET(pack_filename,0,sizeof(pack_filename));
	if(filename && (*filename == '*')){
		STRCPY(pack_filename,filename);
	}else if(filename && (*filename == '%')){
		STRCPY(pack_filename,filename+1);
	}else if(filename && (*filename == '#') && (*(filename+1) == '<')){
		STRCPY(pack_filename,filename+2);
	}else{
		STRCPY(pack_filename,MR_DEFAULT_PACK_NAME);
	}

	MEMSET(old_pack_filename, 0, sizeof(old_pack_filename));
	MEMSET(old_start_filename, 0, sizeof(old_start_filename));

	MEMSET(start_fileparameter, 0, sizeof(start_fileparameter));
	mrc_appInfo_st.ram = 0;

#ifdef SDK_MOD
	return _mr_intra_start("cfunction.ext", entry);
#else
	//#if defined(MR_BREW_MOD)
	//   return _mr_intra_start("cfunction.ext", entry);
	//#else
	return _mr_intra_start("logo.ext", entry);
	//#endif
#endif
}

int32 mr_start_dsmB(const char* entry)
{
	mr_screeninfo screeninfo;
	if (mr_getScreenInfo(&screeninfo) != MR_SUCCESS)
	{
		return MR_FAILED;
	}
	mr_screen_w = screeninfo.width;
	mr_screen_h = screeninfo.height;
	mr_screen_bit = screeninfo.bit;

	MEMSET(pack_filename,0,sizeof(pack_filename));
	if(entry && (*entry == '*')){
		STRCPY(pack_filename,entry);
		//以后%的方式要从VM 中去掉
	}else if(entry && (*entry == '%')){
		char *loc;
		loc=(char*)strchr(entry, ',');
		if(loc!=NULL){
			*loc=0;
			STRCPY(pack_filename,entry+1);
			*loc=',';
		}else{
			STRCPY(pack_filename,entry+1);
		}
	}else if(entry && (*entry == '#') && (*(entry+1) == '<')){
		STRCPY(pack_filename,entry+2);
	}else{
		STRCPY(pack_filename,MR_DEFAULT_PACK_NAME);
	}
	//strcpy(pack_filename,"*A");
	MRDBGPRINTF(pack_filename);


	MEMSET(old_pack_filename, 0, sizeof(old_pack_filename));
	MEMSET(old_start_filename, 0, sizeof(old_start_filename));

	MEMSET(start_fileparameter, 0, sizeof(start_fileparameter));

	mrc_appInfo_st.ram = 0;

#ifdef SDK_MOD
	return _mr_intra_start("cfunction.ext", entry);
#else
	return _mr_intra_start("logo.ext", entry);
#endif

}

int32 mr_start_dsm(const char* entry)
{
	mr_screeninfo screeninfo;
	if (mr_getScreenInfo(&screeninfo) != MR_SUCCESS)
	{
		return MR_FAILED;
	}
	mr_screen_w = screeninfo.width;
	mr_screen_h = screeninfo.height;
	mr_screen_bit = screeninfo.bit;

	MEMSET(pack_filename,0,sizeof(pack_filename));
	if(entry && (*entry == '*')){
		STRCPY(pack_filename,entry);
		//以后%的方式要从VM 中去掉
	}else if(entry && (*entry == '%')){
		STRCPY(pack_filename,entry+1);
	}else if(entry && (*entry == '#') && (*(entry+1) == '<')){
		STRCPY(pack_filename,entry+2);
	}else{
		STRCPY(pack_filename,MR_DEFAULT_PACK_NAME);
	}
	MRDBGPRINTF(pack_filename);


	MEMSET(old_pack_filename, 0, sizeof(old_pack_filename));
	MEMSET(old_start_filename, 0, sizeof(old_start_filename));

	MEMSET(start_fileparameter, 0, sizeof(start_fileparameter));

	mrc_appInfo_st.ram = 0;
#ifdef SDK_MOD
	return _mr_intra_start("cfunction.ext", entry);
#else
	return _mr_intra_start("logo.ext", entry);
#endif
}

int32 mr_start_dsmC(char* start_file, const char* entry)
{
	mr_screeninfo screeninfo;
	if (mr_getScreenInfo(&screeninfo) != MR_SUCCESS)
	{
		return MR_FAILED;
	}
	mr_screen_w = screeninfo.width;
	mr_screen_h = screeninfo.height;
	mr_screen_bit = screeninfo.bit;

	MEMSET(pack_filename,0,sizeof(pack_filename));
	if(entry && (*entry == '*')){
		STRCPY(pack_filename,entry);
		//以后%的方式要从VM 中去掉
	}else if(entry && (*entry == '%')){
		STRCPY(pack_filename,entry+1);
	}else if(entry && (*entry == '#') && (*(entry+1) == '<')){
		STRCPY(pack_filename,entry+2);
	}else{
		STRCPY(pack_filename,MR_DEFAULT_PACK_NAME);
	}
	MRDBGPRINTF(pack_filename);


	MEMSET(old_pack_filename, 0, sizeof(old_pack_filename));
	MEMSET(old_start_filename, 0, sizeof(old_start_filename));

	MEMSET(start_fileparameter, 0, sizeof(start_fileparameter));
	mrc_appInfo_st.ram = 0;

	return _mr_intra_start(start_file, entry);
}


int32 mr_stop_ex(int16 freemem)
{
	//int i;
	if (mr_state == MR_STATE_IDLE){
		return MR_IGNORE;
	}

	if ((mr_state == MR_STATE_RUN) || (mr_state == MR_STATE_PAUSE))
	{
#if 0
		mrp_getglobal(vm_state, "dealevent");
		if (mrp_isfunction(vm_state, -1)) {
			mrp_pushnumber(vm_state, MR_EXIT_EVENT);
			_mr_pcall(1,0);

		} else {  /* no dealevent function */
			MRDBGPRINTF("exit de is nil!");
			mrp_pop(vm_state, 1);  /* remove dealevent */
		}
#else
		c_event_st.code = MR_EXIT_EVENT;
		c_event_st.param0 = 0;
		c_event_st.param1 = 0;

		_mr_TestComC( 801, (char*)&c_event_st, sizeof(c_event_st), 1);
#endif
	}


	//MRDBGPRINTF("restart timer12");
	//mr_sleep(50);

	mr_state = MR_STATE_IDLE;
	mr_timer_state = MR_TIMER_STATE_IDLE;
	mr_timer_run_without_pause = FALSE;


	if (freemem){
#ifdef MR_SCREEN_CACHE
#ifdef MR_SCREEN_CACHE_BITMAP
		//MR_FREE(mr_screenBMP, MR_SCREEN_W * MR_SCREEN_H * 2 + MR_BMP_FILE_HEADER_LEN);
		mr_screenBMP = NULL;
#else
		if(mr_bitmap[BITMAPMAX].type == MR_SCREEN_FIRST_BUF){
			//MR_FREE(mr_screenBuf, mr_bitmap[BITMAPMAX].buflen);
		}else if(mr_bitmap[BITMAPMAX].type == MR_SCREEN_SECOND_BUF){
			mr_platEx(1002, (uint8*)mr_screenBuf, mr_bitmap[BITMAPMAX].buflen, (uint8**)NULL, NULL, NULL);
		}
#endif
#else
		//MR_FREE(mr_screenBuf, MR_SCREEN_W * MR_SCREEN_H * 2);
#endif
		mr_screenBuf = NULL;
	}

#if 0
	for(i=0;i<BITMAPMAX;i++)
	{
		if(mr_bitmap[i].p)
		{
			MR_FREE(mr_bitmap[i].p, mr_bitmap[i].buflen);
			mr_bitmap[i].p = NULL;
		}
	}

	for(i=0;i<TILEMAX;i++)
	{
		if(mr_map[i])
		{
			MR_FREE(mr_map[i], mr_tile[i].w*mr_tile[i].h*2);
			mr_map[i] = NULL;
		}
	}

	for(i=0;i<SOUNDMAX;i++)
	{
		if(mr_sound[i].p)
		{
			MR_FREE(mr_sound[i].p, mr_sound[i].buflen);
			mr_sound[i].p = NULL;
		}
	}

	if(mr_ram_file){
		MR_FREE(mr_ram_file, mr_ram_file_len);
		mr_ram_file = NULL;
	}
#endif

	//MRDBGPRINTF("restart timer13");
	//mr_sleep(50);

	if (freemem){
		mr_mem_free(Origin_LG_mem_base, Origin_LG_mem_len);
	}
	//mr_timerStop();
	return MR_SUCCESS;
}

int32 mr_stop(void) //int16 freemem)
{
	if (mr_stop_function){
		int status = mr_stop_function();
		mr_stop_function = NULL;   //1943
		if(status!=MR_IGNORE)
			return status;
	}
	return mr_stop_ex(TRUE);
}

/*暂停应用*/
int32 mr_pauseApp(void)
{
	// mr_initOk = FALSE;
	if (mr_state == MR_STATE_RUN){
		mr_state = MR_STATE_PAUSE;
	}else if  ( mr_state == MR_STATE_RESTART){
		MR_TIME_STOP();  
		//mr_timer_state = MR_TIMER_STATE_IDLE;
		return MR_SUCCESS;
	}else{
		return MR_IGNORE;
	};

	if (mr_pauseApp_function){
		int status = mr_pauseApp_function();
		if(status!=MR_IGNORE)
			return status;
	}

	_mr_TestComC( 801, NULL, 1, 4);

	if(!mr_timer_run_without_pause){
		if(mr_timer_state == MR_TIMER_STATE_RUNNING){
			MR_TIME_STOP();
			mr_timer_state = MR_TIMER_STATE_SUSPENDED;
		}
	}
	return MR_SUCCESS;
}

/*恢复应用*/
int32 mr_resumeApp(void)
{
	if (mr_state == MR_STATE_PAUSE){
		mr_state = MR_STATE_RUN;
	}else if  ( mr_state == MR_STATE_RESTART){
		mr_timer_p = (void*)"restart";
		MR_TIME_START(100);
		//mr_timer_state = MR_TIMER_STATE_RUNNING;
		return MR_SUCCESS;
	}else{
		return MR_IGNORE;
	};

	if (mr_resumeApp_function){
		int status = mr_resumeApp_function();
		if(status!= MR_IGNORE)
			return status;
	}

	_mr_TestComC( 801, NULL, 1, 5);

	if(mr_timer_state == MR_TIMER_STATE_SUSPENDED){
		MR_TIME_START(300);
		//mr_timer_state = MR_TIMER_STATE_RUNNING;
	}
	return MR_SUCCESS;
}

int32 mr_event(int16 type, int32 param1, int32 param2)
{

	if ((mr_state == MR_STATE_RUN) || ((mr_timer_run_without_pause) && (mr_state == MR_STATE_PAUSE)))
	{
		if (mr_event_function){
			int status = mr_event_function(type, param1, param2);
			if(status!= MR_IGNORE)
				return status;
		}


		c_event_st.code = type;
		c_event_st.param0 = param1;
		c_event_st.param1 = param2;

		_mr_TestComC( 801, (char*)&c_event_st, sizeof(c_event_st), 1);

		//MRDBGPRINTF("after event");

		return MR_SUCCESS;  //deal
	}

	return MR_IGNORE;  //didnot deal

}

int32 mr_timer(void)
{
	//MRDBGPRINTF("timer %d,%d",mr_state, mr_timer_state);
	if (mr_timer_state != MR_TIMER_STATE_RUNNING){
		MRDBGPRINTF("warning:mr_timer event unexpected!");
		return MR_IGNORE;
	}
	mr_timer_state = MR_TIMER_STATE_IDLE;

	if ((mr_state == MR_STATE_RUN) || ((mr_timer_run_without_pause) && (mr_state == MR_STATE_PAUSE))){
	}else if  ( mr_state == MR_STATE_RESTART){
		//MRDBGPRINTF("restart timer1");
		//mr_sleep(50);

		mr_stop();                     //1943 修改为mr_stop
		//mr_stop_ex(TRUE);      //1943
		/* 不重新初始化内存
		_mr_mem_init();
		#ifdef MR_SCREEN_CACHE
		mr_screenBuf = (uint16*)MR_MALLOC(MR_SCREEN_MAX_W*MR_SCREEN_H*2);
		#else
		mr_screenBuf = (uint16*)mr_getScreenBuf();
		#endif
		*/
		//MRDBGPRINTF("restart timer11");
		//mr_sleep(50);

		_mr_intra_start(start_filename, NULL);

		//MRDBGPRINTF("restart timer2");
		//mr_sleep(50);

		return MR_SUCCESS;
	}else{
		return MR_IGNORE;
	};


	//MRDBGPRINTF("before timer");

	if (mr_timer_function){
		int status = mr_timer_function();
		if(status!=MR_IGNORE)
			return status;
	}

	_mr_TestComC( 801, NULL, 1, 2);

	//MRDBGPRINTF("after timer");
	//mrp_setgcthreshold(vm_state, 0);
	return MR_SUCCESS;
}

int32 mr_registerAPP(uint8 *p, int32 len, int32 index)
{
	MRDBGPRINTF("mr_registerAPP:%d p:%d",index,p);		
	if( index < (sizeof(mr_m0_files)/sizeof(uint8*)) ){
		mr_m0_files[index] = p;
	}else{
		MRDBGPRINTF("mr_registerAPP err!");
		return MR_FAILED;
	}

	MRDBGPRINTF("mr_registerAPP mr_m0_files[%d]:%d ",index,mr_m0_files[index]);

	return MR_SUCCESS;
}

int mr_wstrlen(char * txt)
{
	int lenth=0;
	unsigned char * ss=(unsigned char*)txt;

	while((*ss<<8)+*(ss+1)!=0)
	{
		lenth+=2;
		ss+=2;
	}
	return lenth;
}

int mr_transBMP(int check)
{
	return (check * 11)+0x1234abc;
}

#ifdef SDK_MOD

uint32 *mr_get_helper(void)
{
	return (*(((uint32 **)sdk_mr_c_function_table) - 2));
}
uint32 *mr_get_c_function_p(void)
{
	return  (*(((uint32 **)sdk_mr_c_function_table) - 1));
}

#endif



//****************************短信

/*
首先定义包的定义：
一个包为：数据长度（一个字节，数据内容的长度，长度不包括
自己的一个字节）＋数据内容（数据内容的长度为"数据长度"字段定义）
一个包内的数据内容可以是预定义的数据格式，其中也可以包含若干个
子包。
如一个包，数据内容为CD F2 D5，则整个包为03 CD F2 D5。


功能:
从数据缓冲中取得一个数据包。

输入
in:数据指针
inlen:数据长度
输出

in:剩下的数据指针
inlen:剩下的数据长度
chunk:取得的数据包指针
chunklen:取得的数据包长度

*/
int32 _mr_getChunk(uint8 **in, int32 *inlen, uint8 **chunk, int32 *chunklen)
{
	if (*inlen <= 1)
	{                   // 数据包已经读完
		*chunk = *in;
		*chunklen = *inlen;
		return MR_IGNORE;
	}
	*chunklen = **in;
	if((*chunklen >= *inlen)||(*chunklen <= 0))
	{                      // 数据包比数据缓冲还长，出错
		*chunk = *in;
		*chunklen = *inlen;
		return MR_FAILED;
	}

	*chunk = *in+1;
	*in = *in + *chunklen+1;
	*inlen = *inlen - *chunklen - 1;
	return MR_SUCCESS;
}

#ifdef MR_CFG_USE_A_DISK
static int32 _mr_change_to_root(void)
{
	char* root;
	uint8* output;
	int32 output_len;
	MR_PLAT_EX_CB cb;
	int32 ret;
	root =  "Y:";
	ret = mr_platEx(1204, (uint8*)root, strlen(root)+1, &output, &output_len, &cb);
	if (ret != MR_SUCCESS){
		memset(temp_current_path, 0, sizeof(temp_current_path));
		return MR_FAILED;
	}
	strncpy(temp_current_path, (char*)output, sizeof(temp_current_path));
	root =  "X:";
	ret = mr_platEx(1204, (uint8*)root, strlen(root)+1, &output, &output_len, &cb);
	if (ret != MR_SUCCESS){
		memset(temp_current_path, 0, sizeof(temp_current_path));
		return MR_FAILED;
	}
	return MR_SUCCESS;
}

static int32 _mr_change_to_current(void)
{
	uint8* output;
	int32 output_len;
	MR_PLAT_EX_CB cb;
	if (temp_current_path[0]){
		mr_platEx(1204, (uint8*)temp_current_path, strlen(temp_current_path)+1, &output, &output_len, &cb);
	}
	return MR_SUCCESS;
}
#endif

static int32 _mr_save_sms_cfg(MR_FILE_HANDLE f)
{
	int32 ret;


	//MRDBGPRINTF("mr_save_sms_cfg before check!");
	if(mr_sms_cfg_need_save){
		mr_sms_cfg_need_save = FALSE;
		//MRDBGPRINTF("mr_save_sms_cfg before mr_seek!");

#ifdef MR_CFG_USE_A_DISK
		_mr_change_to_root();
#endif
		f = mr_open(DSM_CFG_FILE_NAME, MR_FILE_WRONLY |MR_FILE_CREATE);
#ifdef MR_CFG_USE_A_DISK
		_mr_change_to_current();
#endif
		if(f == 0)
		{
			return MR_FAILED;
		}
		ret = mr_seek(f, 0, MR_SEEK_SET);
		if(ret == MR_FAILED)
		{
			//MRDBGPRINTF("mr_save_sms_cfg mr_seek err!");
			mr_close(f);
			return MR_FAILED;
		}
		//MRDBGPRINTF("mr_save_sms_cfg before mr_write!");
		ret = mr_write(f, mr_sms_cfg_buf, MR_SMS_CFG_BUF_LEN);
		if(ret == MR_FAILED)
		{
			//MRDBGPRINTF("mr_save_sms_cfg mr_write err!");
			mr_close(f);
			return MR_FAILED;
		}
		mr_close(f);
	}
	//MRDBGPRINTF("mr_save_sms_cfg end!");
	return MR_SUCCESS;
}

//#endif


//查看DSM配置文件是否存在，不存在则创建之
static int32 _mr_load_sms_cfg(void)
{
	MR_FILE_HANDLE f;
	int32 ret;

	mr_sms_cfg_need_save = FALSE;

	MEMSET(mr_sms_cfg_buf, 0, MR_SMS_CFG_BUF_LEN);

#ifdef MR_CFG_USE_A_DISK
	_mr_change_to_root();
#endif

	if(mr_info(DSM_CFG_FILE_NAME) == MR_IS_FILE)
	{
		f = mr_open(DSM_CFG_FILE_NAME, MR_FILE_RDONLY);
		if(f == 0)
		{
#ifdef MR_CFG_USE_A_DISK
			_mr_change_to_current();
#endif
			return MR_FAILED;
		}
		ret = mr_read(f, mr_sms_cfg_buf, MR_SMS_CFG_BUF_LEN);
		mr_close(f);
		if (ret != MR_SMS_CFG_BUF_LEN){
			f = mr_open(DSM_CFG_FILE_NAME, MR_FILE_WRONLY|MR_FILE_CREATE);
			if(f == 0)
			{
#ifdef MR_CFG_USE_A_DISK
				_mr_change_to_current();
#endif
				return MR_FAILED;
			}
			mr_close(f);
			_mr_smsAddNum(0, "518869058");
			_mr_smsAddNum(1, "918869058");
			_mr_smsAddNum(3, "aa");
		}
		//#endif
	}else{

		_mr_smsAddNum(0, "518869058");
		_mr_smsAddNum(1, "918869058");
		_mr_smsAddNum(3, "aa");

	}

#ifdef MR_CFG_USE_A_DISK
	_mr_change_to_current();
#endif
	return MR_SUCCESS;
}

int32 _mr_smsGetBytes(int32 pos, char* p, int32 len)
{
	//MRDBGPRINTF("_mr_smsGetBytes");

	if((pos>=MR_SMS_CFG_BUF_LEN)||(pos < 0)||( (pos + len) >= MR_SMS_CFG_BUF_LEN)){
		return MR_FAILED;
	}
	MEMCPY(p, mr_sms_cfg_buf+pos, len);
	return MR_SUCCESS;

}

static int32 _mr_smsSetBytes(int32 pos, char* p, int32 len)
{
	//MRDBGPRINTF("_mr_smsGetBytes");


	if((pos>=MR_SMS_CFG_BUF_LEN)||(pos < 0)||( (pos + len) >= MR_SMS_CFG_BUF_LEN)){
		return MR_FAILED;
	}
	mr_sms_cfg_need_save = TRUE;
	MEMCPY(mr_sms_cfg_buf+pos, p, len);
	//MRDBGPRINTF("mr_smsSetBytes %d", *p);
	return MR_SUCCESS;

}

int32 _mr_smsGetNum(int32 index, char* pNum)
{
	char num[MR_MAX_NUM_LEN];
	uint32 len;


	//MRDBGPRINTF("_mr_smsGetNum");
	_mr_smsGetBytes(MR_MAX_NUM_LEN * index + MR_SECTION_LEN, num, MR_MAX_NUM_LEN);
	len = _mr_decode((uint8*)num, STRNLEN(num, MR_MAX_NUM_LEN-1), (uint8*)pNum);
	if ((len == 0)||(len >= MR_MAX_NUM_LEN))
	{
		pNum[0] = 0;
		return MR_FAILED;
	}
	pNum[len] = 0;

	return MR_SUCCESS;

}





/*
1、   文件格式说明：
2、   第一个120字节：
a)   第1个32字节：4字节（版本号），1（是否使用SMS更新数据，>128，使用）；
b)   第2个32字节：32字节消息指示。
3、   第二、第三个120字节：
a)   240个字节（定长），每24个字节存放一个接收号码，最多10个接收号码
，每24个字节的格式为，号码字符串包，长度不够后面填充\0。
b)   前三个号码定义：移动发送号码，联通发送号码，统一发送号码。
4、   第四个120字节：
a)   120字节，WAP的URL。
5、   第5～36个120字节：
a)   120×32个字节，每120个字节存放一条DSM更新短消息的全部内容。
*/


/**********************************************
*name:        _mr_smsAddNum
*description: add a cmd num form the ffs
*input:
*                  index---Num index
*                  pNum---pointer to the Num address
*return:     
*                  MR_SUCCESS---success, 
*                  MR_FAILED--failed
*                  MR_IGNORE--already exist
*Note: 
***********************************************/
static int32 _mr_smsAddNum(int32 index, char* pNum)
{
	int32 len = STRLEN(pNum);
	char num[MR_MAX_NUM_LEN];
	if(len > (((MR_MAX_NUM_LEN-1)/4 * 3)))
	{
		MRDBGPRINTF("num too long");
		return MR_FAILED;
	}

	//   MRDBGPRINTF("_mr_smsAddNum");
	MEMSET(num, 0, MR_MAX_NUM_LEN);

	_mr_encode((uint8 *)pNum, len, (uint8 *)num);

	_mr_smsSetBytes(MR_MAX_NUM_LEN * index + MR_SECTION_LEN, num, MR_MAX_NUM_LEN);

	return MR_SUCCESS;

}


/**********************************************
*name:        _mr_smsDelNum
*description: del a cmd num form the ffs
*input:
*                  index---Num index
*return:     
*                  MR_SUCCESS---success, 
*                  MR_FAILED--failed
*                  MR_IGNORE--already exist
*Note: 
***********************************************/
int32 _mr_smsDelNum(int32 index)
{
	char num[MR_MAX_NUM_LEN];


	//MRDBGPRINTF("_mr_smsDelNum");

	MEMSET(num, 0, MR_MAX_NUM_LEN);

	_mr_smsSetBytes(MR_MAX_NUM_LEN * index + MR_SECTION_LEN, num, MR_MAX_NUM_LEN);

	return MR_SUCCESS;

}

/**********************************************
*name:        _mr_smsUpdateURL
*description: update URL form the ffs
*input:
*                  pURL---pointer to the URL
*return:     
*                  MR_SUCCESS---success, 
*                  MR_FAILED--failed
*                  MR_IGNORE--already exist
*Note: 
***********************************************/
int32 _mr_smsUpdateURL(uint8* pURL, uint8 len)
{
	uint8 flag=128;
	uint8 out[MR_SECTION_LEN];

	//MRDBGPRINTF("_mr_smsUpdateURL");
	if(len > (((MR_SECTION_LEN-1)/4 * 3)))
	{
		MRDBGPRINTF("url too long");
		return MR_FAILED;
	}


	_mr_smsSetBytes(CFG_USE_URL_UPDATE_OFFSET, (char*)&flag , 1);

	MEMSET(out, 0, sizeof(out));
	len = _mr_encode(pURL, len, out);
	_mr_smsSetBytes(MR_SECTION_LEN*3, (char*)out, MR_SECTION_LEN);

	return MR_SUCCESS;

}

/**********************************************
*name:        _mr_smsDsmSave
*description: save a sms content to the ffs
*input:
*                  pSMSContent---pointer to the input sms content buf
*                  
*return:     
*                  MR_SUCCESS---save success
*                  MR_FAILED--save failed
*Note: 
***********************************************/
int32 _mr_smsDsmSave(char*  pSMSContent, int32 len)
{
	uint8 contnet[MR_SECTION_LEN];
	uint8 flag=128;
	int32 index;

	MRDBGPRINTF("_mr_smsDsmSave");

	MEMSET(contnet, 0, MR_SECTION_LEN);

	MEMCPY((char *)contnet, (char *)pSMSContent, len);
	index = contnet[2]; //取得消息的位置号

	if((index > 31))
	{
		return MR_FAILED;
	}

	_mr_smsSetBytes(CFG_USE_SM_UPDATE_OFFSET, (char*)&flag , 1);

	_mr_smsSetBytes(CFG_SM_FLAG_OFFSET+index, (char*)&flag , 1);

	_mr_smsSetBytes(MR_SECTION_LEN * (index+4), (char*)contnet , MR_SECTION_LEN);

	return MR_SUCCESS;
}

/**********************************************
*name:        _mr_smsReplyServer
*description: send a sms back to server
*input:
*                  pNum---pointer to the input number address
*                  
*return:     
*                  MR_SUCCESS---send sms success
*                  MR_FAILED--send sms failed
*Note: 
***********************************************/
int32 _mr_smsReplyServer(char *pNum, uint8* old_IMSI)
{
	uint8 sms[MR_SECTION_LEN];
	uint8 smsstring[MR_MAX_SM_LEN];
	mr_userinfo info;
	uint32 offset = 0;
	//MR_FILE_HANDLE f;

	if(mr_getUserInfo(&info) != MR_SUCCESS)
	{
		return MR_FAILED;
	}

	MEMSET(smsstring, 0, sizeof(smsstring));


	sms[0] = 0xFA;sms[1] = 0xF1;
	offset = offset + 2;

	if (old_IMSI){
		//长度16+1
		sms[offset] = 17;
		offset = offset + 1;
		//旧IMSI
		sms[offset] = 6;
		offset = offset + 1;
		MEMCPY(&sms[offset], old_IMSI, 16);
		offset = offset + 16;
	}

	//长度16+1
	sms[offset] = 17;
	offset = offset + 1;
	//IMEI
	sms[offset] = 2;
	offset = offset + 1;
	MEMCPY(&sms[offset], &info.IMEI, sizeof(info.IMEI));
	offset = offset + 16;

	//长度16+1
	sms[offset] = 17;
	offset = offset + 1;
	//IMSI
	sms[offset] = 3;
	offset = offset + 1;
	MEMCPY(&sms[offset], &info.IMSI, sizeof(info.IMSI));
	offset = offset + 16;

	//长度20+1
	sms[offset] = 21;
	offset = offset + 1;
	//手机信息
	sms[offset] = 4;
	offset = offset + 1;
	info.ver= htonl(info.ver);
	MEMCPY(&sms[offset], &info.manufactory, 20);
	offset = offset + 20;

	_mr_encode(sms, offset, smsstring);
	mr_sendSms((char*)pNum, (char*)smsstring, MR_ENCODE_ASCII | MR_SMS_REPORT_FLAG |MR_SMS_RESULT_FLAG);

	//MRDBGPRINTF("Debug:send sms content=");
	//MRDBGPRINTF((char*)smsstring);
	//MRDBGPRINTF("Debug:send sms num=");
	//MRDBGPRINTF((char*)pNum);

	return MR_SUCCESS;
}


static int32 _mr_u2c_c(char * input, int32 inlen, char* output, int32 outlen){
	int32 pos=0;
	int32 upos=0;
	MEMSET(output, 0, outlen);
	while((upos<(inlen-1))&&((*(input+upos)+*(input+upos+1))!=0)&&(pos<outlen)){
		if(*(input+upos) == 0){
			output[pos] = *(input+upos+1);
			pos = pos + 1;
			upos = upos + 2;
		}else{
			break;
		}
	}
	return pos;
}

static const char *_mr_memfind_c (const char *s1, size_t l1,
								  const  char *s2, size_t l2) {
									  if (l2 == 0) return s1;  /* empty strings are everywhere */
									  else if (l2 > l1) return NULL;  /* avoids a negative `l1' */
									  else {
										  const char *init;  /* to search for a `*s2' inside `s1' */
										  l2--;  /* 1st char will be checked by `memchr' */
										  l1 = l1-l2;  /* `s2' cannot be found after that */
										  while (l1 > 0 && (init = (const char *)MEMCHR(s1, *s2, l1)) != NULL) {
											  init++;   /* 1st char is already checked */
											  if (MEMCMP(init, s2+1, l2) == 0)
												  return init-1;
											  else {  /* correct `l1' and `s1' to try again */
												  l1 -= init-s1;
												  s1 = init;
											  }
										  }
										  return NULL;  /* not found */
									  }
}



static int32 _mr_smsIndiaction(uint8 *pContent, int32 nLen, uint8 *pNum, int32 type)//nLen 变为 int32，方便以后扩展
{
	uint8   outbuf[160];
	int32   memlen;

	if((mr_state == MR_STATE_RUN) || ((mr_timer_run_without_pause) && (mr_state == MR_STATE_PAUSE))){
		c_event_st.code = MR_SMS_INDICATION;
		c_event_st.param0 = (int32)pContent;
		c_event_st.param1 = (int32)pNum;
		c_event_st.param2 = type;
		c_event_st.param3 = nLen;

		_mr_TestComC( 801, (char*)&c_event_st, sizeof(c_event_st), 1);
	}



	//decode the content
	if((nLen < 12) || (nLen > 160)){
		return MR_IGNORE;
	}


	/*
	短信接口说明：
	1、   每条短信内容120个字节。
	2、   网络发往手机的短消息；前两个字节作为本条短信内容指示： 
	a)   FA  F1：DSM配置短信；内容：如"DSM配置短信格式说明"。
	b)   FA  F2：DSM更新短信；内容：如前面（"DSM更新短信格式说明"）。
	3、   手机发往网络的短消息；前两个字节作为本条短信内容指示：
	a)   FA  F1：手机上发消息；内容：如"手机上发消息格式说明"。
	*/

	/*
	DSM配置短信格式说明：
	1、   一个DSM配置短信内容由若干个包构成。
	2、   包的内容：操作码（一个字节）＋操作码对应的操作数据。
	3、   操作码定义：
	1: 添加一个命令接收号码；操作数据：号码位置（一个字节）＋号码字符串＋\0。
	2: 删除一个命令接收号码；操作数据：号码位置（一个字节）。
	3: 设置WAP的URL；操作数据：URL字符串＋\0。
	4: 要求手机回复版本及信息消息；操作数据：无。

	*/

	MEMSET(outbuf, 0, sizeof(outbuf));
	switch(type)
	{
	case MR_ENCODE_ASCII:
		if ((pContent[0] == 'M') && (pContent[1] == 'R')
			&&(pContent[2] == 'P') && (pContent[3] == 'G')){
				//这里放宽了要求
				memlen = _mr_decode(pContent+4, nLen-4, outbuf);
		}else{
			//mr_printf("mr_sms not  cmd num");
			const char *s1 = _mr_memfind_c((const char *)pContent, nLen, (const char *)"~#^~", 4);
			const char *s2;
			if (s1) {
				s2 = _mr_memfind_c((const char *)s1, nLen - ((uint8*)s1-pContent), (const char *)"&^", 2);
				if (s2){
					memlen = _mr_decode((uint8*)s1+4, (s2-s1-4), outbuf);
				}else{
					return MR_IGNORE;
				}
			}else{
				return MR_IGNORE;
			}
		}
		break;
	case MR_ENCODE_UNICODE:
		{
			const char *s1 = _mr_memfind_c((const char *)pContent, nLen, (const char *)"\0~\0#\0^\0~", 8);
			const char *s2;
			if (s1) {
				s2 = _mr_memfind_c((const char *)s1, nLen - ((uint8*)s1-pContent), (const char *)"\0&\0^", 4);
				if (s2){
					char   inbuf[70];
					int32 inlen;
					inlen = _mr_u2c_c((char*)s1+8, (s2-s1-8), inbuf, sizeof(inbuf));
					memlen = _mr_decode((uint8*)inbuf, inlen, outbuf);
				}else{
					return MR_IGNORE;
				}
			}else{
				return MR_IGNORE;
			}
			break;
		}
	default:
		return MR_IGNORE;
		break;
	}


	if (memlen < 0)
	{
		//mr_printf("_mr_decode failed");
		return MR_IGNORE;
	}

	MRDBGPRINTF("mr_smsIndiaction check ok!");
	{
		int32 f;
		f = _mr_load_sms_cfg();
		if ((outbuf[0] == 0xfc) && (outbuf[1] == 0xfc)) {
			uint8 *in;
			int32 inlen;
			uint8 *chunk;
			int32 chunklen;
			int32 ret;
			in = (uint8 *)outbuf + 2;
			inlen = memlen - 2;

			ret = _mr_getChunk(&in, &inlen, &chunk, &chunklen);    //取得一个Chunk
			while(ret == MR_SUCCESS)
			{
				int32 code = *chunk;
				int32 tempret = MR_FAILED;
				//uint8 flag=128;
				switch(code)
				{
				case 1:
					tempret = _mr_smsAddNum(*(chunk+1),  (char*)(chunk+2));
					break;
				case 2:
					tempret = _mr_smsDelNum(*(chunk+1));
					break;
				case 3:
					tempret = _mr_smsUpdateURL((chunk+1), (uint8)(chunklen-1));
					break;
				case 4:
					tempret = _mr_smsReplyServer((char*)pNum, NULL);
					break;
				case 5:
					tempret = _mr_smsSetBytes(CFG_USE_UNICODE_OFFSET, (char*)(chunk+1), 1);
					//MRDBGPRINTF("mr_smsIndiaction UNICODE!");
					break;
				case 6:
					tempret = _mr_smsSetBytes( ( (*(chunk+1)) * 256 )+ (*(chunk+2) ) , (char*)(chunk+4), *(chunk+3));
					break;
				case 7:
				case 17:
				case 27:
				case 37:
					tempret = MR_SUCCESS;
					break;
				default:
					_mr_save_sms_cfg(f);
					return MR_FAILED;
				}   //switch
				if (tempret != MR_SUCCESS){
					_mr_save_sms_cfg(f);
					return MR_FAILED;
				}
				ret = _mr_getChunk(&in, &inlen, &chunk, &chunklen);    //取得下一个Chunk
			}     //while
		}else if((outbuf[0] == 0xfa) && (outbuf[1] == 0xf2)) {
			_mr_smsDsmSave((char *)outbuf, memlen);      
		}
		_mr_save_sms_cfg(f);
		return MR_SUCCESS;
	}
}

/**********************************************
*name:        mr_smsIndiaction
*description: get a new sms coming, check it whether was send by cmd server
*input:
*                  pNum---pointer to the Num address
*                  pContent---pointer to the sms content
*            nLen   ---
*return:     
*                  MR_SUCCESS---packet ok
*                  MR_FAILED--something error when doing the sending action
*            MR_IGNORE--- normal sms , do not do mr treating.
*Note: 
***********************************************/
int32 mr_smsIndiaction(uint8 *pContent, int32 nLen, uint8 *pNum, int32 type)//nLen 变为 int32，方便以后扩展
{
	int32 ret;
	mr_sms_return_flag=0;
	ret = _mr_smsIndiaction(pContent, nLen, pNum, type);
	if (mr_sms_return_flag==1)
		ret =  mr_sms_return_val;
	return ret;
}

static int32 _mr_newSIMInd(int16  type, uint8* old_IMSI)
{
	int32 id = mr_getNetworkID();
	uint8 flag;
	char num[MR_MAX_NUM_LEN];
	int32 f;

	if( (MR_SIM_NEW == type) || (MR_SIM_CHANGE == type) )
	{
		f = _mr_load_sms_cfg();
		_mr_save_sms_cfg(f);

		_mr_smsGetBytes(5, (char*)&flag, 1);
		if(flag >= 128)
		{
			_mr_smsGetNum(3, num);
		}else
		{
			switch(id)
			{
			case MR_NET_ID_MOBILE:
				if (_mr_smsGetNum(MR_NET_ID_MOBILE, num) == MR_FAILED)
					return MR_FAILED;
				break;
			case MR_NET_ID_CN:
			case MR_NET_ID_CDMA:
				if (_mr_smsGetNum(MR_NET_ID_CN, num) == MR_FAILED)
					return MR_FAILED;
				break;
			default:
				return MR_FAILED;
				break;
			}
		}
		_mr_smsReplyServer(num, old_IMSI);
	}
	return MR_SUCCESS;
}

int32 mr_newSIMInd(int16  type, uint8* old_IMSI)
{
	int32 ret;
	ret = _mr_newSIMInd(type, old_IMSI);
	return ret;
}

//****************************短信

int32 mr_save_mrp(void *p,uint32 l)
{
	char filename[15];
	MR_FILE_HANDLE f;

	if(p == NULL || l == 0)
	{
		return MR_FAILED;
	}

	if((*(char*)p == 'M') && (*((char*)p + 1) == 'R') && (*((char*)p + 2) == 'P') && (*((char*)p + 3) == 'G'))
	{

		MEMSET(filename, 0, sizeof(filename));
		MEMCPY(filename , (char*)p + MR_OFFSET_FORM_FILEHEAD, 12);

		/*if the same name file is exist, cover it*/
		//if(mr_ffsStat(fileName, fsize))
		{
			mr_remove(filename);
		}

		f = mr_open(filename,  MR_FILE_WRONLY | MR_FILE_CREATE);

		if(f != 0)
		{
			if(mr_write(f, (void *)p, l) != (int32)l)
			{
				mr_close(f);
				return MR_FAILED;
			}
		}

		mr_close(f);
		return MR_SUCCESS;
	}
	else
	{
		return MR_IGNORE;
	}
}


static void encode02(char *  value, int len, unsigned char cBgnInit, unsigned char cEndInit)//简单加密
{
	int iLeft;
	int iRight;

	for(iLeft = 0; iLeft < len; iLeft++)
	{
		if( iLeft==0) value[0] ^= cBgnInit;
		else
		{
			if(value[iLeft] != value[iLeft-1])
				value[iLeft] ^=value[iLeft-1];
		}
	}
	for(iRight = len -1 ; iRight >= 0 ; iRight--)
	{
		if(iRight == len -1 ) value[ iRight ] ^= cEndInit;
		else
		{
			if(value[iRight] != value[iRight+1])
				value[iRight] ^= value[iRight+1];
		}
	}
}

static int _mr_isMr(char*  input)
{
	mr_userinfo info;
	char enc[16];
	int appid, appver;
	int ret = MR_FAILED;

	if (mr_getUserInfo(&info) == MR_SUCCESS){
		appid = htonl(*((int*)&input[16]));
		appver = htonl(*((int*)&input[20]));
		enc[0] = info.IMEI[1];
		enc[1] = info.IMEI[2];
		enc[2] = info.IMEI[3];
		enc[3] = info.IMEI[4];
		enc[4] = info.IMEI[5];
		enc[5] = info.IMEI[7];
		enc[6] = info.IMEI[8];
		enc[7] = appid % 239;
		enc[8] = appver % 237;
		enc[9] = info.manufactory[0];
		enc[10] = info.type[0];
		enc[11] = STRLEN(info.manufactory);
		enc[12] = MR_VERSION % 251;
		enc[13] = MR_VERSION % 247;
		enc[14] = info.ver % 253;
		enc[15] = info.ver % 241;
		encode02(enc, 16, (info.IMEI[0] % 10)*21+info.IMEI[6], 
			((info.IMEI[11] + info.IMEI[12]) % 10)*21+info.IMEI[14]);
		if (MEMCMP(enc, input, 16) == 0){
			ret = MR_SUCCESS;
		}
	}else{
	}
	return ret;
}



//////////////////////////////////////////md5

#define T1 0xd76aa478
#define T2 0xe8c7b756
#define T3 0x242070db
#define T4 0xc1bdceee
#define T5 0xf57c0faf
#define T6 0x4787c62a
#define T7 0xa8304613
#define T8 0xfd469501
#define T9 0x698098d8
#define T10 0x8b44f7af
#define T11 0xffff5bb1
#define T12 0x895cd7be
#define T13 0x6b901122
#define T14 0xfd987193
#define T15 0xa679438e
#define T16 0x49b40821
#define T17 0xf61e2562
#define T18 0xc040b340
#define T19 0x265e5a51
#define T20 0xe9b6c7aa
#define T21 0xd62f105d
#define T22 0x02441453
#define T23 0xd8a1e681
#define T24 0xe7d3fbc8
#define T25 0x21e1cde6
#define T26 0xc33707d6
#define T27 0xf4d50d87
#define T28 0x455a14ed
#define T29 0xa9e3e905
#define T30 0xfcefa3f8
#define T31 0x676f02d9
#define T32 0x8d2a4c8a
#define T33 0xfffa3942
#define T34 0x8771f681
#define T35 0x6d9d6122
#define T36 0xfde5380c
#define T37 0xa4beea44
#define T38 0x4bdecfa9
#define T39 0xf6bb4b60
#define T40 0xbebfbc70
#define T41 0x289b7ec6
#define T42 0xeaa127fa
#define T43 0xd4ef3085
#define T44 0x04881d05
#define T45 0xd9d4d039
#define T46 0xe6db99e5
#define T47 0x1fa27cf8
#define T48 0xc4ac5665
#define T49 0xf4292244
#define T50 0x432aff97
#define T51 0xab9423a7
#define T52 0xfc93a039
#define T53 0x655b59c3
#define T54 0x8f0ccc92
#define T55 0xffeff47d
#define T56 0x85845dd1
#define T57 0x6fa87e4f
#define T58 0xfe2ce6e0
#define T59 0xa3014314
#define T60 0x4e0811a1
#define T61 0xf7537e82
#define T62 0xbd3af235
#define T63 0x2ad7d2bb
#define T64 0xeb86d391

static void md5_process(md5_state_t *pms, const md5_byte_t *data /*[64]*/)
{
	md5_word_t
		a = pms->abcd[0], b = pms->abcd[1],
		c = pms->abcd[2], d = pms->abcd[3];
	md5_word_t t;

	//#ifndef ARCH_IS_BIG_ENDIAN
	//#define ARCH_IS_BIG_ENDIAN 1	/* slower, default implementation */
	//#endif


#ifdef MR_BIG_ENDIAN

	/*
	* On big-endian machines, we must arrange the bytes in the right
	* order.  (This also works on machines of unknown byte order.)
	*/
	md5_word_t X[16];
	const md5_byte_t *xp = data;
	int i;

	for (i = 0; i < 16; ++i, xp += 4)
		X[i] = xp[0] + (xp[1] << 8) + (xp[2] << 16) + (xp[3] << 24);

#else  /* !MR_BIG_ENDIAN */

	/*
	* On little-endian machines, we can process properly aligned data
	* without copying it.
	*/
	md5_word_t xbuf[16];
	const md5_word_t *X;

	if (!((data - (const md5_byte_t *)0) & 3)) 
	{
		/* data are properly aligned */
		X = (const md5_word_t *)data;
	} 
	else 
	{
		/* not aligned */
		MEMCPY(xbuf, data, 64);
		X = xbuf;
	}
#endif

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

	/* Round 1. */
	/* Let [abcd k s i] denote the operation
	a = b + ((a + F(b,c,d) + X[k] + T[i]) <<< s). */
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define SET(a, b, c, d, k, s, Ti)\
	t = a + F(b,c,d) + X[k] + Ti;\
	a = ROTATE_LEFT(t, s) + b
	/* Do the following 16 operations. */
	SET(a, b, c, d,  0,  7,  T1);
	SET(d, a, b, c,  1, 12,  T2);
	SET(c, d, a, b,  2, 17,  T3);
	SET(b, c, d, a,  3, 22,  T4);
	SET(a, b, c, d,  4,  7,  T5);
	SET(d, a, b, c,  5, 12,  T6);
	SET(c, d, a, b,  6, 17,  T7);
	SET(b, c, d, a,  7, 22,  T8);
	SET(a, b, c, d,  8,  7,  T9);
	SET(d, a, b, c,  9, 12, T10);
	SET(c, d, a, b, 10, 17, T11);
	SET(b, c, d, a, 11, 22, T12);
	SET(a, b, c, d, 12,  7, T13);
	SET(d, a, b, c, 13, 12, T14);
	SET(c, d, a, b, 14, 17, T15);
	SET(b, c, d, a, 15, 22, T16);
#undef SET

	/* Round 2. */
	/* Let [abcd k s i] denote the operation
	a = b + ((a + G(b,c,d) + X[k] + T[i]) <<< s). */
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
	t = a + G(b,c,d) + X[k] + Ti;\
	a = ROTATE_LEFT(t, s) + b
	/* Do the following 16 operations. */
	SET(a, b, c, d,  1,  5, T17);
	SET(d, a, b, c,  6,  9, T18);
	SET(c, d, a, b, 11, 14, T19);
	SET(b, c, d, a,  0, 20, T20);
	SET(a, b, c, d,  5,  5, T21);
	SET(d, a, b, c, 10,  9, T22);
	SET(c, d, a, b, 15, 14, T23);
	SET(b, c, d, a,  4, 20, T24);
	SET(a, b, c, d,  9,  5, T25);
	SET(d, a, b, c, 14,  9, T26);
	SET(c, d, a, b,  3, 14, T27);
	SET(b, c, d, a,  8, 20, T28);
	SET(a, b, c, d, 13,  5, T29);
	SET(d, a, b, c,  2,  9, T30);
	SET(c, d, a, b,  7, 14, T31);
	SET(b, c, d, a, 12, 20, T32);
#undef SET

	/* Round 3. */
	/* Let [abcd k s t] denote the operation
	a = b + ((a + H(b,c,d) + X[k] + T[i]) <<< s). */
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define SET(a, b, c, d, k, s, Ti)\
	t = a + H(b,c,d) + X[k] + Ti;\
	a = ROTATE_LEFT(t, s) + b
	/* Do the following 16 operations. */
	SET(a, b, c, d,  5,  4, T33);
	SET(d, a, b, c,  8, 11, T34);
	SET(c, d, a, b, 11, 16, T35);
	SET(b, c, d, a, 14, 23, T36);
	SET(a, b, c, d,  1,  4, T37);
	SET(d, a, b, c,  4, 11, T38);
	SET(c, d, a, b,  7, 16, T39);
	SET(b, c, d, a, 10, 23, T40);
	SET(a, b, c, d, 13,  4, T41);
	SET(d, a, b, c,  0, 11, T42);
	SET(c, d, a, b,  3, 16, T43);
	SET(b, c, d, a,  6, 23, T44);
	SET(a, b, c, d,  9,  4, T45);
	SET(d, a, b, c, 12, 11, T46);
	SET(c, d, a, b, 15, 16, T47);
	SET(b, c, d, a,  2, 23, T48);
#undef SET

	/* Round 4. */
	/* Let [abcd k s t] denote the operation
	a = b + ((a + I(b,c,d) + X[k] + T[i]) <<< s). */
#define I(x, y, z) ((y) ^ ((x) | ~(z)))
#define SET(a, b, c, d, k, s, Ti)\
	t = a + I(b,c,d) + X[k] + Ti;\
	a = ROTATE_LEFT(t, s) + b
	/* Do the following 16 operations. */
	SET(a, b, c, d,  0,  6, T49);
	SET(d, a, b, c,  7, 10, T50);
	SET(c, d, a, b, 14, 15, T51);
	SET(b, c, d, a,  5, 21, T52);
	SET(a, b, c, d, 12,  6, T53);
	SET(d, a, b, c,  3, 10, T54);
	SET(c, d, a, b, 10, 15, T55);
	SET(b, c, d, a,  1, 21, T56);
	SET(a, b, c, d,  8,  6, T57);
	SET(d, a, b, c, 15, 10, T58);
	SET(c, d, a, b,  6, 15, T59);
	SET(b, c, d, a, 13, 21, T60);
	SET(a, b, c, d,  4,  6, T61);
	SET(d, a, b, c, 11, 10, T62);
	SET(c, d, a, b,  2, 15, T63);
	SET(b, c, d, a,  9, 21, T64);
#undef SET

	/* Then perform the following additions. (That is increment each
	of the four registers by the value it had before this block
	was started.) */
	pms->abcd[0] += a;
	pms->abcd[1] += b;
	pms->abcd[2] += c;
	pms->abcd[3] += d;
}

void mr_md5_init(md5_state_t *pms)
{
	pms->count[0] = pms->count[1] = 0;
	pms->abcd[0] = 0x67452301;
	pms->abcd[1] = 0xefcdab89;
	pms->abcd[2] = 0x98badcfe;
	pms->abcd[3] = 0x10325476;
}

void mr_md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes)
{
	const md5_byte_t *p = data;
	int left = nbytes;
	int offset = (pms->count[0] >> 3) & 63;
	md5_word_t nbits = (md5_word_t)(nbytes << 3);

	if (nbytes <= 0)
		return;

	/* Update the message length. */
	pms->count[1] += nbytes >> 29;
	pms->count[0] += nbits;

	if (pms->count[0] < nbits)
		pms->count[1]++;

	/* Process an initial partial block. */
	if (offset) 
	{
		int copy = (offset + nbytes > 64 ? 64 - offset : nbytes);

		MEMCPY(pms->buf + offset, p, copy);

		if (offset + copy < 64)
			return;

		p += copy;
		left -= copy;
		md5_process(pms, pms->buf);
	}

	/* Process full blocks. */
	for (; left >= 64; p += 64, left -= 64)
		md5_process(pms, p);

	/* Process a final partial block. */
	if (left)
		MEMCPY(pms->buf, p, left);
}

void mr_md5_finish(md5_state_t *pms, md5_byte_t digest[16])
{
	static const md5_byte_t pad[64] = 
	{
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	md5_byte_t data[8];
	int i;

	/* Save the length before padding. */
	for (i = 0; i < 8; ++i)
		data[i] = (md5_byte_t)(pms->count[i >> 2] >> ((i & 3) << 3));

	/* Pad to 56 bytes mod 64. */
	mr_md5_append(pms, pad, ((55 - (pms->count[0] >> 3)) & 63) + 1);

	/* Append the length. */

	mr_md5_append(pms, data, 8);
	for (i = 0; i < 16; ++i)
		digest[i] = (md5_byte_t)(pms->abcd[i >> 2] >> ((i & 3) << 3));
}

//////////////////////////////////////////md5



int FF_Divide( int dividend, int divisor )
{
	int i;
	int remainder1,remainder2,div;
	int temp;
	int result=0;
	int flag;

	if (divisor==0)
		return 0x7fFFFFFF;		

	remainder2=dividend;
	if(remainder2<0)remainder2 =-remainder2;
	div=divisor;
	if(div < 0)div = -div;

	result=(int)(remainder2/div);
	result=(result<<15);		//INT fraction

	//FLOAT fraction
	remainder1=(int)(remainder2%div);
	remainder1=(remainder1<<1);
	remainder2=0;

	for (i=0;i<15;i++)
	{
		temp=remainder1;
		remainder1-=div;
		flag=1;
		if(remainder1<0){
			remainder1=temp;
			flag=0;
		}
		remainder1=(remainder1<<1);
		remainder2=(remainder2<<1);
		if (flag)
			remainder2=remainder2+1;
	}

	result+=remainder2;

	if(((dividend>0) && (divisor<0)) || ((dividend<0) && (divisor>0)))
		result=-result;

	return result;
}


#ifdef MR_MSTAR_MOD

#undef strncpy
//extern char *strncpy(char * s1, const char * s2, int n);

char *mr_strncpy(char * s1, const char * s2, int n)
{
	if (s1&&s2&&n){
		return strncpy(s1,s2,n);
	}else{
		MRDBGPRINTF("mr_strncpy %x,%x,%x",s1,s2,n);
		return NULL;
	}
}
#define strncpy mr_strncpy
#endif


#if defined(MR_BREW_OTA_MOD)
int mrp_sprintf(char * s,const char * format, ...)
{
	int ret;
	va_list ap;

	va_start(ap, format);
	ret = VSPRINTF(s, format, ap);
	va_end(ap);
	return ret;
}
#endif

uint32 mr_ltoh(char * startAddr)
{
	return  (startAddr[3] << 24) | ((startAddr[2] & 0xff)<< 16)  | ((startAddr[1] & 0xff) << 8) | (startAddr[0] & 0xff);
}

uint32 mr_ntohl(char * startAddr)
{
	return  ((startAddr[0] & 0xff)<< 24) | ((startAddr[1] & 0xff)<< 16) | ((startAddr[2] & 0xff) << 8) | (startAddr[3] & 0xFF);
}


#define CFG_FILENAME  "#807022#*"

int32 _mr_getMetaMemLimit()
{
	int32 nTmp;
	int32 len = 0,file_len = 0;
	void* workbuffer = NULL;

	int32 f;

	char TempName[MR_MAX_FILENAME_SIZE];
	int is_rom_file = FALSE;
	uint32 headbuf[4];
	char* this_packname;
	char* mr_m0_file;
	char _v[4];	
	int32 memValue;


	this_packname = pack_filename;


	if ((this_packname[0] == '*')||(this_packname[0] == '$'))
	{
		/*read file from m0*/
		uint32 pos = 0;
		uint32 m0file_len;


		if (this_packname[0] == '*'){/*m0 file?*/
			mr_m0_file = (char*)mr_m0_files[this_packname[1]-0x41]; //这里定义文件名为*A即是第一个m0文件
			//*B是第二个.........
		}else{
			mr_m0_file = mr_ram_file;
		}


		if (mr_m0_file == NULL){

			return 0;
		}

		pos = pos + 4;
		MEMCPY(&_v[0], &mr_m0_file[pos], 4);

		len =  mr_ltoh((char*)_v);

		pos = pos + 4;

		if((this_packname[0] == '$')){
			m0file_len = mr_ram_file_len;
		}else{
			MEMCPY(&_v[0], &mr_m0_file[pos], 4);

			m0file_len = mr_ltoh((char*)_v);
		}

		pos = pos + len;

		if (((pos+4) >= m0file_len)||(len<1)||(len>=MR_MAX_FILE_SIZE))
		{
			return 0;
		}
		MEMCPY(&_v[0], &mr_m0_file[pos], 4);

		len = mr_ltoh((char*)_v);

		pos = pos + 4;
		if (((len + pos) >= m0file_len)||(len<1)||(len>=MR_MAX_FILENAME_SIZE))
		{
			return 0;
		}

		MEMCPY(TempName, &mr_m0_file[pos], len);
		TempName[len] = 0;

		pos = pos + len;
		if (STRCMP(CFG_FILENAME, TempName)==0)
		{
			MEMCPY(&_v[0], &mr_m0_file[pos], 4);

			len = mr_ltoh((char*)_v);

			pos = pos + 4;

			if (((len + pos) > m0file_len)||(len<1)||(len>=MR_MAX_FILE_SIZE))
			{
				return 0;
			}
		}else
		{
			return 0;
		}

		file_len = len;
		if (file_len <= 0 )
		{
			return 0;
		}

		pos += 3*4;	
		MEMCPY(&_v[0],&mr_m0_file[pos],4);
	}else   /*read file from efs , EFS 中的文件*/
	{
		f =  mr_open(this_packname, MR_FILE_RDONLY );


		MEMSET(headbuf, 0, sizeof(headbuf));
		nTmp = mr_read(f, &headbuf, sizeof(headbuf));


		headbuf[0] = mr_ltoh((char*)&headbuf[0]);
		headbuf[1] = mr_ltoh((char*)&headbuf[1]);
		headbuf[2] = mr_ltoh((char*)&headbuf[2]);
		headbuf[3] = mr_ltoh((char*)&headbuf[3]);

		if( (nTmp != 16)||(headbuf[0] != 1196446285) ||(headbuf[1] <= 232))
		{
			mr_close(f);
			return 0;
		}
		{                             //新版mrp 
			uint32 indexlen = headbuf[1] + 8 - headbuf[3];
			uint32 pos = 0;
			uint32 file_pos = 0;

			nTmp = mr_seek(f, headbuf[3] - 16, MR_SEEK_CUR);
			if (nTmp < 0)
			{
				mr_close(f);
				return 0;
			}



			nTmp = mr_read(f,&_v[0],4);
			if (nTmp!=4)
			{
				mr_close(f);
				return 0;
			}

			len = mr_ltoh((char*)_v);

			pos = pos + 4;
			if (((len + pos) > indexlen)||(len<1)||(len>=MR_MAX_FILENAME_SIZE))
			{
				mr_close(f);

				return 0;
			}

			nTmp = mr_read(f,&TempName[0],len);	
			if (nTmp!=len)
			{
				mr_close(f);
				return 0;
			}

			TempName[len] = 0;

			pos = pos + len;
			if (STRCMP(CFG_FILENAME, TempName)==0)
			{
				nTmp = mr_read(f,&_v[0],4);
				pos = pos + 4;
				file_pos = mr_ltoh((char*)_v);

				nTmp = mr_read(f,&_v[0],4);
				pos = pos + 4;
				file_len = mr_ltoh((char*)_v);


				if ((file_pos + file_len) > headbuf[2])
				{
					mr_close(f);

					return 0;
				}
			}else
			{
				mr_close(f);
				MRDBGPRINTF("can not found %s",CFG_FILENAME);
				return 0; 
			}



			nTmp = mr_seek(f,file_pos + 3*4,MR_SEEK_SET); /*精简虚拟器读取第四个int32*/
			if (nTmp<0)
			{
				mr_close(f);
				return 0;
			}


#ifdef MR_SPREADTRUM_MOD
			if ((file_len < 0)){
				mr_close(f);

				return 0;
			}
#endif

			nTmp = mr_read(f,&_v[0],4);

			mr_close(f);
			if (nTmp!=4)
			{
				return nTmp;	
			}

		}
	}

	memValue = mr_ntohl((char*)_v);


	return memValue;
}




void mr_getScrBuf(uint16** buf,int32 *width,int32 *height)
{
	if (buf)     *buf    = mr_screenBuf;
	if (width)   *width  = mr_screen_w;
	if (height)  *height = mr_screen_h;
}

void mr_getMemoryInfo(uint32 *total, uint32 *free, uint32 *top)
{
	if(total) *total = LG_mem_len;
	if(free) *free = LG_mem_left;
	if(top) *top = LG_mem_top;
}

void mr_setScrBuf(uint16* buf)
{
	mr_screenBuf = buf;
}
