/*
 * mythroad引擎代码
 * 实现虚拟机主要功能（类dsm.lib）
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys\mman.h>

#include "mr_helper.h"
#include "mr_types.h"
#include "mrporting.h"
#include "mythroad.h"
#include "mr_draw.h"
#include "mr_mem.h"
#include "mr_file.h"
#include "mr_utils.h"
#include "mr_smscfg.h"

typedef int32 (*MR_EVENT_FUNC)(int32 code, int32 param0, int32 param1);
typedef int32 (*MR_RESUMEAPP_FUNC)(void);
typedef int32 (*MR_PAUSEAPP_FUNC)(void);
typedef int32 (*MR_TIMER_FUNC)(void);
typedef int32 (*MR_STOP_FUNC)(void);

/*-----------------------------------*/
// 引擎全局变量

// 在后面定义和初始化
extern mr_internal_table    _mr_c_internal_table;
extern mr_c_port_table      _mr_c_port_table;
extern const mr_table       _mr_c_function_table;

//#define MRP_PATH

char	pack_filename[128];
char	start_filename[128];
char	old_pack_filename[128];
char	old_start_filename[128];
char 	start_fileparameter[128];

char	*mr_ram_file;
int32   mr_ram_file_len;
uint8   *mr_m0_files[50];

static mr_bitmapSt	mr_bitmap[BITMAPMAX + 1];
static mr_tileSt	mr_tile[TILEMAX];
static mr_soundSt	mr_sound[SOUNDMAX];
static mr_spriteSt	mr_sprite[SPRITEMAX];
static int16		*mr_map[3];

static mrc_appInfoSt    mrc_appInfo_st;
static mr_c_event_st    c_event_st;

static char mr_entry[128];
static char *mr_exception_str;

static mrc_timerCB mr_exit_cb;      //1951
static int32       mr_exit_cb_data; //1951

static MR_LOAD_C_FUNCTION   mr_load_c_function;
static MR_C_FUNCTION        mr_c_function;
static mr_c_function_st     *mr_c_function_P;
static uint32               mr_c_function_P_len;
static void                 *mr_c_function_fix_p;

static MR_EVENT_FUNC        mr_event_function;
static MR_RESUMEAPP_FUNC    mr_resumeApp_function;
static MR_PAUSEAPP_FUNC     mr_pauseApp_function;
static MR_TIMER_FUNC        mr_timer_function;
static MR_STOP_FUNC         mr_stop_function;

static uint32   vm_state;
static int32    mr_state;
static int32    bi;

static void     *mr_timer_p;
static uint32   mr_timer_state;
static void     *mr_timer_run_without_pause;

static int8	mr_soundOn;
static int8	mr_shakeOn;


extern void mr_cacheFlush();

/*-----------------------------------*/
// 辅助函数

int mr_wstrlen(char *str)
{
    int lenth = 0;
    unsigned char *ss = (unsigned char*)str;

    while(((*ss << 8) + *(ss + 1)) != 0)
    {
        lenth += 2;
        ss += 2;
    }

    return lenth;
}

int32 mrp_version(void)
{
	LOGI("mrp_version=%d", MYTHROAD_VERSION);
    return MYTHROAD_VERSION;
}

uint32 mrc_ntohl(uint32 num)
{
    return (num << 24) | ((num & 0x0000FF00) << 8) | ((num & 0x00FF0000) >> 8) | (num >> 24);
}

/*-----------------------------------*/
// 库函数封装
static void *mr_memcpy(void *dst, const void *src, int len) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_memcpy(%#p,%#p,%d)", dst, src, len);
		return dst;
	}
	return memcpy(dst, src, len);
}

static void *mr_memmove(void *dst, const void *src, int len) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_memmove(%#p,%#p,%d)", dst, src, len);
		return dst;
	}
	return memmove(dst, src, len);
}

static char *mr_strcpy(char *dst, const char *src) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_strcpy(%#p,%#p)", dst, src);
		return dst;
	}
	return strcpy(dst, src);
}

static char *mr_strncpy(char *dst, const char *src, int len) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_strncpy(%#p,%#p,%d)", dst, src, len);
		return dst;
	}
	return strncpy(dst, src, len);
}

static char *mr_strcat(char *dst, const char *src) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_strcat(%#p,%#p)", dst, src);
		return dst;
	}
	return strcat(dst, src);
}

static char *mr_strncat(char *dst, const char *src, int len) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_strncat(%#p,%#p,%d)", dst, src, len);
		return dst;
	}
	return strncat(dst, src, len);
}

static int mr_memcmp(const void *dst, const void *src, int len) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_memcmp(%#p,%#p,%d)", dst, src, len);
		return -2;
	}
	return memcmp(dst, src, len);
}

static int mr_strcmp(const char *dst, const char *src) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_strcmp(%#p,%#p)", dst, src);
		return -2;
	}
	return strcmp(dst, src);
}

static int mr_strncmp(const char *dst, const char *src, int len) {
	if (NULL == dst || NULL == src) {
		LOGE("mr_strncmp(%#p,%#p,%d)", dst, src, len);
		return -2;
	}
	return strncmp(dst, src, len);
}

static int mr_strcoll(const char *dst, const char *src) {
	return strcoll(dst, src);
}

static void* mr_memchr(const void *s, int c, int len) {
	return memchr(s, c, len);
}

static void* mr_memset(void *s, int c, int len) {
	if (NULL == s) {
		LOGE("mr_memset(%#p,%d,%d)", s, c, len);
		return s;
	}
	return memset(s, c, len);
}

static int mr_strlen(const char *s) {
	return strlen(s);
}

static char *mr_strstr(const char *s1, const char *s2) {
	return strstr(s1, s2);
}

static int mr_sprintf(char *buf, const char *fmt, ...)
{
	__va_list vars;
	int ret;

	va_start(vars, fmt);
	ret = vsprintf(buf, fmt, vars);
	ret = vsnprintf(buf, fmt, vars);
	va_end(vars);

	return ret;
}

static int mr_atoi(const char *s) {
	return atoi(s);
}

static unsigned long mr_strtoul(const char *nptr, char **endptr, int base) {
	return strtoul(nptr, endptr, base);
}

/*static void mr_sand(uint32 seed) {
    return srand(seed);
}*/

static int mr_rand(void) {
	return rand();
}

static int32 _mr_div(int32 a, int32 b) {
    return a / b;
}

static int32 _mr_mod(int32 a, int32 b) {
    return a % b;
}

/*-----------------------------------*/
// 内部函数实现

static int32 _DispUpEx(int16 x, int16 y, uint16 w, uint16 h)
{
	//LOGI("_DispUpEx(%d,%d,%d,%d)", x, y, w, h);

    if (1 == vm_state)
        mr_drawBitmap(mr_screenBuf, x, y, w, h);

    return 0;
}

static int32 _mr_TestCom(int32 L, int input0, int input1)
{
    int32 ret = 0;

#ifdef LOG_TEST_COM
    LOGI("_mr_TestCom(%d, %d, %d)", L, input0, input1);
#endif

    switch (input0)
    {
    case 0x1:
        ret = mr_getTime();
        break;
    case 0x2:
        mr_event_function = (MR_EVENT_FUNC)input1;
        break;
    case 0x3:
        mr_timer_function = (MR_TIMER_FUNC)input1;
        break;
    case 0x4:
        mr_stop_function = (MR_STOP_FUNC)input1;
        break;
    case 0x5:
        mr_pauseApp_function = (MR_PAUSEAPP_FUNC)input1;
        break;
    case 0x6:
        mr_resumeApp_function = (MR_RESUMEAPP_FUNC)input1;
        break;
    case 0x7:
        ret = input1;
        break;
    case 0x64:
        ret = LG_mem_min;
        break;
    case 0x65:
        ret = LG_mem_top;
        break;
    case 0x66:
        ret = LG_mem_left;
        break;
    case 0xc8:
        if (1 == mr_state && 0 != mr_shakeOn)
            ret = mr_startShake(input1);
        break;
    case 0x12c:
        mr_soundOn = (int8)input1;
        break;
    case 0x12d:
        mr_shakeOn = (int8)input1;
        break;
    case 0x12e:
        bi |= 0x4;
        break;
    case 0x12f:
        bi &= ~0x4;
        break;
    case 0x130:
        bi |= 0x8;
        break;
    case 0x131:
        bi &= ~0x8;
        break;
    case 0x132:
        mr_sms_return_flag = 1;
        mr_sms_return_val = input1;
        break;
    case 0x133:
        mr_sms_return_flag = 0;
        break;
    case 0x190:
        mr_sleep(input1);
        break;
    case 0x191:
        ret = mr_screen_w;
        mr_screen_w = input1;
        break;
    case 0x194:
        _mr_newSIMInd((int16)input1, NULL);
        break;
    case 0x195:
        ret = mr_closeNetwork();
        break;
    case 0x196:
        ret = mr_screen_h;
        mr_screen_h = input1;
        break;
    case 0x197:
        mr_timer_run_without_pause = (void*)input1;
        mr_plat(1202, input1);
        break;
    case 0x198:
        if (0 == mr_bitmap[BITMAPMAX].type)
        {
            mr_bitmap[BITMAPMAX].p = mr_malloc(input1);
            if (NULL == mr_bitmap[BITMAPMAX].p)
            {
                ret = -1;
            }
            else
            {
                mr_free(mr_screenBuf, mr_bitmap[BITMAPMAX].buflen);
                mr_screenBuf = mr_bitmap[BITMAPMAX].p;
                mr_bitmap[BITMAPMAX].buflen = input1;
            }
        }
        else if (1 == mr_bitmap[BITMAPMAX].type)
        {
            if (mr_bitmap[BITMAPMAX].buflen > input1)
            {
                ret = -1;
            }
        }
        break;
    case 0x1f4:
        ret = _mr_load_sms_cfg();
        break;
    case 0x1f7:
        {
            uint8 dst;

            _mr_smsGetBytes(5, &dst, 1);
            ret = dst;
        }
        break;
    case 0x1f8:
        ret = _mr_save_sms_cfg(input1);
        break;

    case 0xcb3:
        if (0x9E67A == input1)
            bi &= ~0x2;
        break;
    case 0xe2d:
        if (0xb61 == input1)
            bi |= 0x1;
        break;
    case 0xf51:
        if (0x18030 == input1)
            bi |= 0x2;
        break;
    default:
		LOGW("unknown mr_TestCom(%d, %d, %d)!", L, input0, input1);
        mr_printf("unknown mr_TestCom(%d, %d, %d)!", L, input0, input1);
        ret = MR_IGNORE;
        break;
    }

    return ret;
}

static int32 _mr_TestCom1(int32 L, int input0, char* input1, int32 len)
{
    int32 ret = 0;

#ifdef LOG_TEST_COM
    LOGI("mr_TestCom1(%d, %d, %#p, %d)", L, input0, input1, len);
#endif

    switch (input0)
    {
    case 2:
        if (NULL != mr_ram_file)
        {
            mr_free(mr_ram_file, mr_ram_file_len);
            mr_ram_file = NULL;
        }
        mr_ram_file = input1;
        mr_ram_file_len = len;
        break;
    case 3:
        memset(old_pack_filename, 0, 128);
        if (NULL != input1)
        {
            strncpy(old_pack_filename, input1, 127);
        }
        memset(old_start_filename, 0, 128);
        strncpy(old_start_filename, START_FILE_NAME, 127);
        break;
    case 4:
        memset(start_fileparameter, 0, 128);
        if (NULL != input1)
        {
            strncpy(start_fileparameter, input1, 127);
        }
        break;
    case 5:
        mr_exception_str = input1;
        break;
    case 6:
        mr_exception_str = NULL;
        break;
    case 7:
		ret = 0;
		break;
    case 8:
		ret = 0;
		break;
    case 9:
		{
			mr_cacheSync((void*)input1,len);
			ret = 0;

			mr_cacheSync((void*)((uint32)(input1)&(~0x0000001F)),
					((len+0x0000001F*3)&(~0x0000001F)));

			break;
		}
    case 200:
        mr_updcrc(NULL, 0);
        mr_updcrc(input1, len);
        ret = mr_updcrc(input1, 0);
        break;
    case 700:
        ret = mr_newSIMInd(((len << 16) >> 16), input1);
        break;
    case 701:
        ret = mr_smsIndiaction(input1, len, input1 + 1, *(input1 + 2));
        break;
    case 900:
        ret = mr_platEx(200001, (uint8*)&_mr_c_port_table, 0x10, NULL, NULL, NULL);
        break;
    default:
		LOGW("unknown mr_testCom1(%d,%d,%#p,%d)!", L, input0, input1, len);
    	mr_printf("unknown mr_testCom1(%d,%d,%#p,%d)!", L, input0, input1, len);
        ret = MR_IGNORE;
        break;
    }

    return ret;
}

static int32 getAppInfo(void)
{
    int32 fd;
    int32 memfile;

    fd = 0;
    memfile = 0;

    fd = mrc_GetMrpInfoOpen(pack_filename, &memfile);
    if (0 != fd)
    {
        mrc_GetMrpInfoEx(memfile, fd, 3, &mrc_appInfo_st.id, 4);
        mrc_GetMrpInfoEx(memfile, fd, 4, &mrc_appInfo_st.ver, 4);
        /*if (NULL != mrc_appInfo_st.sidName)
            mrc_GetMrpInfoEx(memfile, fd, 9, (char *)&mrc_appInfo_st + 12, 4);*/
        mrc_appInfo_st.sidName = NULL;
        mrc_GetMrpInfoClose(memfile, fd);

        return 0;
    }

    return -1;
}

/*-----------------------------------*/
// 

static int32 _mr_c_function_new(MR_C_FUNCTION f, int32 len)
{
#ifdef LOG_TEST_COM
	LOGI("_mr_c_function_new(%#p, %d)", f, len);
#endif

    if (NULL != mr_c_function_P)
        mr_free(mr_c_function_P, mr_c_function_P_len);

    mr_c_function_P = (mr_c_function_st *)mr_malloc(len);
    if (NULL != mr_c_function_P)
    {
        mr_c_function_P_len = len;
        memset(mr_c_function_P, 0, mr_c_function_P_len);
        mr_c_function = f;
    }
    else
    {
        mr_state = 5;
        return -1;
    }

    if (NULL == mr_c_function_fix_p)
    {
        *(uint32*)((uint32)mr_load_c_function - 4) = (uint32)mr_c_function_P;
    }
    else
    {
        *(uint32*)((uint32)mr_c_function_fix_p + 4) = (uint32)mr_c_function_P;
    }

	mr_cacheFlush(1);

    return MR_SUCCESS;
}

static int32 _mr_TestComC(int32 type, uint8 *input, int32 len, int32 code)
{
#ifdef LOG_TEST_COM
	LOGI("_mr_TestComC(type=%d, input=%#p, len=%d, code=%d)", type, input, len, code);
#endif

    if (type == 800)
    {
        mr_load_c_function = (MR_LOAD_C_FUNCTION)(input + 8);
        *(uint32 *)input = (uint32)&_mr_c_function_table;

		mr_cacheSync((void*)((uint32)(input)&(~0x0000001F)),
				((len+0x0000001F*3)&(~0x0000001F)));


        mr_load_c_function(code);
    }
    else if (type == 801)
    {
		//mr_cacheFlush(3); //不需要
        mr_c_function(mr_c_function_P, code, input, len, NULL, NULL);
    }

    return 0;
}

static int32 mr_doExt(const char *filename)
{
    int32 ret;
    int32 len;
    uint8 *data;


#ifdef LOG_TEST_COM
	LOGI("mr_doExt(%s)", filename);
#endif

    data = _mr_readFile(filename, &len, 0);
    if (NULL != data)
    {
        _mr_TestCom(0, 0xe2d, 0xb61);
        if (_mr_TestComC(800, data, len, 0))
        {
			LOGE("mr_doExt err:%d", 11002);
            mr_printf("mr_doExt err:%d", 11002);

            return -1;
        }
        else
        {
            ret = _mr_TestComC(801, data, MYTHROAD_VERSION, 6);
            ret = getAppInfo();
            ret = _mr_TestComC(801, (uint8*)&mrc_appInfo_st, 16, 8);
            ret = _mr_TestComC(801, data, MYTHROAD_VERSION, 0);
            return 0;
        }
    }
    else
    {
		LOGE("mr_doExt err:%d", 11001);
        mr_printf("mr_doExt err:%d", 11001);
        return -1;
    }
}

static int32 _mr_intra_start(const char *filename, const char *entry)
{
    int32 ret;
    int32 len;
    int32 i;

#ifdef LOG_TEST_COM
    LOGI("_mr_intra_start(%s, %s)", filename, entry);
#endif

    if (-1 == mr_mem_init())
        return -1;

    mr_event_function = NULL;
    mr_timer_function = NULL;
    mr_stop_function = NULL;
    mr_pauseApp_function = NULL;
    mr_resumeApp_function = NULL;

    mr_ram_file = NULL;
    mr_c_function_P = NULL;
    mr_c_function_P_len = 0;
    mr_c_function_fix_p = NULL;
    mr_exception_str = NULL;

    len = 0;
    mr_screenBuf = NULL;

    ret = mr_platEx(1001, NULL, 0, (uint8**)&mr_screenBuf, &len, NULL);
    if (0 == ret)
    {
        if (NULL != mr_screenBuf)
        {
            if (len >= 2 * mr_screen_h * mr_screen_w)
            {
                mr_bitmap[BITMAPMAX].type = 1;
                mr_bitmap[BITMAPMAX].buflen = len;
            }
            else
            {
                mr_platEx(1002, (uint8*)mr_screenBuf, len, NULL, NULL, NULL);
                mr_screenBuf = NULL;
            }
        }
    }

    if (NULL == mr_screenBuf)
    {
        mr_screenBuf = mr_malloc(2 * mr_screen_h * mr_screen_w);
        mr_bitmap[BITMAPMAX].type = 0;
        mr_bitmap[BITMAPMAX].buflen = 2 * mr_screen_h * mr_screen_w;
    }

    vm_state = 0;
    mr_timer_state = 0;
    mr_timer_run_without_pause = 0;
    bi &= 2;

    memset(mr_bitmap, 0, sizeof(mr_bitmapSt) * BITMAPMAX);
    memset(mr_sound, 0, sizeof(mr_soundSt) * SOUNDMAX);
    memset(mr_sprite, 0, sizeof(mr_spriteSt) * SPRITEMAX);
    memset(mr_tile, 0, sizeof(mr_tileSt) * TILEMAX);
    memset(mr_map, 0, 12);

    for (i = 0; i < TILEMAX; i++)
    {
        mr_tile[i].x1 = 0;
        mr_tile[i].y1 = 0;
        mr_tile[i].x2 = (int16)(mr_screen_w & 0xffff);
        mr_tile[i].y2 = (int16)(mr_screen_h & 0xffff);
    }

    if (NULL == entry)
        entry = "_dsm";
    mr_strncpy(mr_entry, entry, 0x7F);
    mr_state = 1;

    ret = mr_doExt(filename);
    if (0 != ret)
    	ret = mr_doExt("logo.ext"); //尝试加载 logo.ext

    if (0 != ret)
    {
        mr_state = 5;
        mr_stop();
		LOGE("_mr_intra_start, init failed");
        mr_printf("init failed");
        mr_connectWAP("http://wap.mrpej.com");
    }

    return 0;
}

static int32 mr_stop_ex(int16 freemem)
{
#ifdef LOG_TEST_COM
	LOGI("mr_stop_ex(%d)", freemem);
#endif

    if (0 == mr_state)
        return MR_IGNORE;

    if (mr_state == 1 || mr_state == 2)
    {
        c_event_st.code = 8;
        c_event_st.param0 = 0;
        c_event_st.param1 = 0;
        _mr_TestComC(801, (uint8*)&c_event_st, sizeof(c_event_st), 1);
    }

    mr_state = 0;
    mr_timer_state = 0;
    mr_timer_run_without_pause = 0;

    if (0 != freemem)
    {
        if (mr_bitmap[BITMAPMAX].type != 0 && mr_bitmap[BITMAPMAX].type == 1)
            mr_platEx(1002, (uint8*)mr_screenBuf, mr_bitmap[BITMAPMAX].buflen, 0, 0, 0);
        mr_screenBuf = NULL;
        //mr_mem_free(Origin_LG_mem_base, Origin_LG_mem_len);
        mr_mem_end();
    }

    return MR_SUCCESS;
}

void mr_reserve()
{
	LOGI("mr_reserve");
}

/*-----------------------------------*/
// 函数表

mr_internal_table _mr_c_internal_table = {
    mr_m0_files,
    &vm_state,
    &mr_state,
    &bi,

    &mr_timer_p,
    &mr_timer_state,
    &mr_timer_run_without_pause,

    &mr_gzInBuf,
    &mr_gzOutBuf,
    &LG_gzinptr,
    &LG_gzoutcnt,

    &mr_sms_cfg_need_save,
    _mr_smsSetBytes,
    _mr_smsAddNum,
    _mr_newSIMInd,

    _mr_isMr,

    // 以下省略部分只有在支持mr解释器的版本使用
};

mr_c_port_table _mr_c_port_table = {
    NULL,
    NULL,
    NULL,
    NULL //mr_c_gcfunction
};

//虚拟机 默认的函数表
const mr_table _mr_c_function_table = {
    mr_malloc,
    mr_free,
    mr_realloc,

    mr_memcpy,
    mr_memmove,
    mr_strcpy,
    mr_strncpy,
    mr_strcat,
    mr_strncat,
    mr_memcmp,
    mr_strcmp,
    mr_strncmp,
    mr_strcoll,
    mr_memchr,
    mr_memset,
    mr_strlen,
    mr_strstr,
    mr_sprintf,
    mr_atoi,
    mr_strtoul,
    mr_rand,

    NULL,  // reserve
    mr_stop_ex,

    &_mr_c_internal_table,
    &_mr_c_port_table,
    _mr_c_function_new,

    mr_printf,
    mr_mem_get,
    mr_mem_free,
    mr_drawBitmap,
    mr_getCharBitmap,
    mr_timerStart,
    mr_timerStop,
    mr_getTime,
    mr_getDatetime,
    mr_getUserInfo,
    mr_sleep,

    mr_plat,
    mr_platEx,

    //file io
    mr_ferrno,
    mr_open,
    mr_close,
    mr_info,
    mr_write,
    mr_read,
    mr_seek,
    mr_getLen,
    mr_remove,
    mr_rename,
    mr_mkDir,
    mr_rmDir,
    mr_findStart,
    mr_findGetNext,
    mr_findStop,

    mr_exit,
    mr_startShake,
    mr_stopShake,
    mr_playSound,
    mr_stopSound,

    mr_sendSms,
    mr_call,
    mr_getNetworkID,
    mr_connectWAP,

    mr_menuCreate,
    mr_menuSetItem,
    mr_menuShow,
    NULL,  // reserve
    mr_menuRelease,
    mr_menuRefresh,

    mr_dialogCreate,
    mr_dialogRelease,
    mr_dialogRefresh,

    mr_textCreate,
    mr_textRelease,
    mr_textRefresh,

    mr_editCreate,
    mr_editRelease,
    mr_editGetText,

    mr_winCreate,
    mr_winRelease,

    mr_getScreenInfo,

    mr_initNetwork,
    mr_closeNetwork,
    mr_getHostByName,
    mr_socket,
    mr_connect,
    mr_closeSocket,
    mr_recv,
    mr_recvfrom,
    mr_send,
    mr_sendto,

    &mr_screenBuf,
    &mr_screen_w,
    &mr_screen_h,
    &mr_screen_bit,

    // 资源
    mr_bitmap,
    mr_tile,
    mr_map,
    mr_sound,
    mr_sprite,

    //pack
    pack_filename,
    start_filename,
    old_pack_filename,
    old_start_filename,

    &mr_ram_file,
    &mr_ram_file_len,

    &mr_soundOn,
    &mr_shakeOn,

    &LG_mem_base,
    &LG_mem_len,
    &LG_mem_end,
    &LG_mem_left,

    mr_sms_cfg_buf,

    mr_md5_init,
    mr_md5_append,
    mr_md5_finish,

    _mr_load_sms_cfg,
    _mr_save_sms_cfg,

    _DispUpEx,
    _DrawPoint,
    _DrawBitmap,
    _DrawBitmapEx,
    DrawRect,
    _DrawText,
    _BitmapCheck,
    _mr_readFile,
    mr_wstrlen,
    NULL,  // mr_registerAPP,
    _DrawTextEx,
    _mr_EffSetCon,
    _mr_TestCom,
    _mr_TestCom1,
    mr_c2u,
    _mr_div,
    _mr_mod,

    &LG_mem_min,
    &LG_mem_top,

    mr_updcrc,

    start_fileparameter,

    &mr_sms_return_flag,
    &mr_sms_return_val,

    mr_unzip,

    &mr_exit_cb,
    &mr_exit_cb_data,
    mr_entry,
    mr_platDrawChar,
    &LG_mem_free,
    mr_transbitmapDraw,
    mr_drawRegion

    //-- 占坑函数 -------
};

/*-----------------------------------*/
// 引擎对外接口

/*当启动DSM应用的时候，用以对DSM平台进行初始化*/
int32 mr_start_dsm(const char *entry)
{
    mr_screeninfo sinfo;

    if (!mr_getScreenInfo(&sinfo))
    {
        mr_screen_w = sinfo.width;
        mr_screen_h = sinfo.height;
        mr_screen_bit = sinfo.bit;

        memset(pack_filename, 0, 0x80);
        if (NULL != entry && *entry == '*')
            strcpy(pack_filename, entry);
        else if (NULL != entry && *entry == '%')
            strcpy(pack_filename, entry + 1);
        else if (NULL != entry  && *entry == '#' && entry[1] == '<')
            strcpy(pack_filename, entry + 2);
        else
            strcpy(pack_filename, "*A");
        mr_printf(pack_filename);

        memset(old_pack_filename, 0, 0x80);
        memset(old_start_filename, 0, 0x80);
        memset(start_fileparameter, 0, 0x80);

        return _mr_intra_start(START_FILE_NAME, entry);
    }

    return -1;
}

// 版本2：启动参数支持用逗号分隔mrp路径和其它参数
int32 mr_start_dsmB(char *entry)
{
    mr_screeninfo sinfo;

    if (!mr_getScreenInfo(&sinfo))
    {
        mr_screen_w = sinfo.width;
        mr_screen_h = sinfo.height;
        mr_screen_bit = sinfo.bit;

        memset(pack_filename, 0, 0x80);
        if (NULL != entry  && *entry == '*') {
            strcpy(pack_filename, entry);
        } else if (NULL != entry  && *entry == '%') {
            char *p = strchr(entry, ',');

            if (NULL != p)
            {
                *p = 0;
                strcpy(pack_filename, entry + 1);
                *p = ',';
            }
            else
            {
                strcpy(pack_filename, entry + 1);
            }
        } else if (NULL != entry && *entry == '#' && entry[1] == '<') {
            strcpy(pack_filename, entry + 2);
        } else {
            strcpy(pack_filename, "*A");
        }

        mr_printf(pack_filename);

        memset(old_pack_filename, 0, 0x80);
        memset(old_start_filename, 0, 0x80);
        memset(start_fileparameter, 0, 0x80);

        return _mr_intra_start(START_FILE_NAME, entry);
    }

    return -1;
}

// 版本3：支持设置启动文件
int32 mr_start_dsmC(const char *startfile, const char *entry)
{
    mr_screeninfo sinfo;

    if (!mr_getScreenInfo(&sinfo))
    {
        mr_screen_w = sinfo.width;
        mr_screen_h = sinfo.height;
        mr_screen_bit = sinfo.bit;

        memset(pack_filename, 0, 0x80);
        if (NULL != entry && *entry == '*')
            strcpy(pack_filename, entry);
        else if (NULL != entry && *entry == '%')
            strcpy(pack_filename, entry + 1);
        else if (NULL != entry  && *entry == '#' && entry[1] == '<')
            strcpy(pack_filename, entry + 2);
        else
            strcpy(pack_filename, "*A");
        mr_printf(pack_filename);

        memset(old_pack_filename, 0, 0x80);
        memset(old_start_filename, 0, 0x80);
        memset(start_fileparameter, 0, 0x80);

        return _mr_intra_start(startfile, entry);
    }

    return -1;
}

// 版本4：支持分别设置mrp路径和启动参数
int32 mr_start_dsm_ex(const char *path, const char *entry)
{
    mr_screeninfo sinfo;

    if (!mr_getScreenInfo(&sinfo))
    {
        mr_screen_w = sinfo.width;
        mr_screen_h = sinfo.height;
        mr_screen_bit = sinfo.bit;

        memset(pack_filename, 0, 0x80);
        if (NULL != path && *path == '*')
            strcpy(pack_filename, path);
        else if (NULL != path && *path == '%')
            strcpy(pack_filename, path + 1);
        else if (NULL != path  && *path == '#' && path[1] == '<')
            strcpy(pack_filename, path + 2);
        else
            strcpy(pack_filename, "*A");
        mr_printf(pack_filename);

        memset(old_pack_filename, 0, 0x80);
        memset(old_start_filename, 0, 0x80);
        memset(start_fileparameter, 0, 0x80);

        return _mr_intra_start(START_FILE_NAME, entry);
    }

    return -1;
}

/*退出Mythroad并释放相关资源*/
int32 mr_stop(void){
    int32 ret;

    if (NULL != mr_stop_function)
    {
        ret = mr_stop_function();
        mr_stop_function = NULL;
        if (1 != ret)
            return 1;
    }

    return mr_stop_ex(1);
}

/*暂停应用*/
int32 mr_pauseApp(void){
#ifdef LOG_EVENT
	LOGI("mr_pauseApp,mr_state=%d,func=%#p", mr_state, mr_pauseApp_function);
#endif

    if (mr_state == 1)
    {
        mr_state = 2;
        if (!mr_pauseApp_function || (mr_pauseApp_function() == 1))
        {
            _mr_TestComC(801, 0, 1, 4);
            if (!mr_timer_run_without_pause && mr_timer_state == 1)
            {
                mr_timerStop();
                mr_timer_state = 2;
            }
            return 0;
        }
    }
    else if (mr_state == 3)
    {
        mr_timerStop();
        mr_timer_state = 0;
        return 0;
    }

    return 1;
}

/*恢复应用*/
int32 mr_resumeApp(void){
#ifdef LOG_EVENT
	LOGI("mr_resumeApp,mr_state=%d,func=%#p", mr_state, mr_resumeApp_function);
#endif

    if (mr_state == 2)
    {
        mr_state = 1;
        if (!mr_resumeApp_function || (mr_resumeApp_function() == 1))
        {
            _mr_TestComC(801, 0, 1, 5);
            if (mr_timer_state == 2)
            {
                mr_timerStart(300);
                mr_timer_state = 1;
            }
            return 0;
        }
    }
    else if ( mr_state == 3 )
    {
        mr_timer_p = "restart";
        mr_timerStart(100);
        mr_timer_state = 1;

        return 0;
    }

    return 1;
}

/*在Mythroad平台中对事件进行处理*/
int32 mr_event(int16 type, int32 param1, int32 param2){
#ifdef LOG_EVENT
	LOGI("mr_event(%d, %d, %d)", type, param1, param2);
#endif

    if (mr_state == 1 || (mr_timer_run_without_pause && mr_state == 2))
    {
        if (!mr_event_function || (mr_event_function(type, param1, param2) == 1))
        {
        	//LOGI("mr_event(%d,%d,%d)", type, param1, param2);
            c_event_st.code = type;
            c_event_st.param0 = param1;
            c_event_st.param1 = param2;
            _mr_TestComC(801, (uint8*)&c_event_st, sizeof(c_event_st), 1);
            return 0;
        }
    }

    return 1;
}

/*定时器到期时调用定时器事件，Mythroad平台将对之进行处理。*/
int32 mr_timer(void){
#ifdef LOG_EVENT
	LOGI("mr_timer,mr_state=%d,mr_timer_state=%d,func=%#p", mr_state, mr_timer_state, mr_timer_function);
#endif

    if (mr_timer_state == 1)
    {
        mr_timer_state = 0;
        if (mr_state == 1 || (mr_timer_run_without_pause && mr_state == 2))
        {
            if (!mr_timer_function || (mr_timer_function() == 1))
                _mr_TestComC(801, 0, 1, 2);
            return 0;
        }
        else if (mr_state == 3)
        {
            mr_stop();
            _mr_intra_start(start_filename, NULL);
            return 0;
        }
    }
    else
    {
		LOGW("warning:mr_timer event unexpected!");
        mr_printf("warning:mr_timer event unexpected!");
    }

    return 1;
}

/*当手机收到短消息时调用该函数*/
int32 mr_smsIndiaction(uint8 *pContent, int32 nLen, uint8 *pNum, int32 type){
    int32 ret;

#ifdef LOG_EVENT
	LOGI("mr_smsIndiaction(content=%#p, len=%d, num=%s, type=%d)", pContent, nLen, pNum, type);
#endif

    mr_sms_return_flag = 0;

    // 这段代码来自_mr_smsIndiaction
    if (mr_state == 1 || (mr_timer_run_without_pause && mr_state == 2))
    {
        c_event_st.code = 7;
        c_event_st.param0 = (int32)pContent;
        c_event_st.param1 = (int32)pNum;
        c_event_st.param2 = (int32)type;
        c_event_st.param3 = (int32)nLen;
        _mr_TestComC(801, (uint8*)&c_event_st, sizeof(c_event_st), 1);
    }
    /*-----------------------------------*/

    ret = _mr_smsIndiaction(pContent, nLen, pNum, type);

    if (mr_sms_return_flag == 1)
        ret = mr_sms_return_val;

    return ret;
}

/*对下载内容（保存在内存区中的一个下载的文件）进行判断，
若下载文件是DSM菜单，由DSM引擎对下载文件进行保存。使用
本函数时，下载文件应该已经下载完全，并且全部内容保存在
所给的内存中。*/
int32 mr_save_mrp(void *p, uint32 l){
    int32 fd;
    int32 ret = -1;
    char Dst[15];
    char *head;

    head = (char*)p;

    if (NULL != p && l > 4
        && head[0] == 'M' && head[1] == 'R' && head[2] == 'P' && head[3] == 'G')
    {
        memset(Dst, 0, 15);
        memcpy(Dst, p + 16, 12);
        mr_remove(Dst);

        fd = mr_open(Dst, MR_FILE_WRONLY | MR_FILE_CREATE);
        if (fd && mr_write(fd, p, l) == l)
            ret = 0;
        mr_close(fd);
    }

    return ret;
}

/*用户SIM卡变更*/
int32 mr_newSIMInd(int16 type, uint8* old_IMSI){
    return _mr_newSIMInd(type, old_IMSI);
}

/*注册固化应用*/
int32 mr_registerAPP(uint8 *p, int32 len, int32 index){
    if (index >= 50)
    {
        mr_printf("mr_registerAPP err!");
        return -1;
    }
    else
    {
        mr_m0_files[index] = p;
        return 0;
    }
}
