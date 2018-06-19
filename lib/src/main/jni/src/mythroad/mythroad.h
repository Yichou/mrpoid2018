#ifndef _ENGINE_H
#define _ENGINE_H

#include "mr_types.h"
#include "mr_helper.h"


#ifndef NO_DEBUG
#include <android/log.h>

#define LOG_TAG "mythroad"

// 打印调试信息
#define LOGI(...) \
	((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) \
	((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) \
	((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

//#define LOG_TEST_COM
//#define LOG_EVENT
//#define LOG_DRAW
//#define LOG_FILE


#else
#define LOGI(...)
#define LOGE(...)
#define LOGW(...)
#endif




#define BITMAPMAX  30
#define SPRITEMAX  10
#define TILEMAX    3
#define SOUNDMAX   5


#define START_FILE_NAME 	"cfunction.ext"
#define MYTHROAD_VERSION 	2012


#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

extern char pack_filename[128];
extern char start_filename[128];
extern char old_pack_filename[128];
extern char old_start_filename[128];
extern char start_fileparameter[128];

extern char	 *mr_ram_file;
extern int32 mr_ram_file_len;
extern uint8 *mr_m0_files[50];

extern const mr_table _mr_c_function_table;


/*当启动DSM应用的时候，应该调用DSM的初始化函数，
用以对DSM平台进行初始化*/
int32 mr_start_dsm(const char* entry);
int32 mr_start_dsmB(char *entry);
int32 mr_start_dsmC(const char *startfile, const char *entry);
int32 mr_start_dsm_ex(const char *path, const char *entry);

/*退出Mythroad并释放相关资源*/
int32 mr_stop(void);

/*暂停应用*/
int32 mr_pauseApp(void);

/*恢复应用*/
int32 mr_resumeApp(void);

/*在Mythroad平台中对按键事件进行处理，press	= MR_KEY_PRESS按键按下，
= MR_KEY_RELEASE按键释放，key	对应的按键编码*/
int32 mr_event(int16 type, int32 param1, int32 param2);

/*定时器到期时调用定时器事件，Mythroad平台将对之进行处理。
p是启动定时器时传入的Mythroad定时器数据*/
int32 mr_timer(void);

/*当手机收到短消息时调用该函数*/
int32 mr_smsIndiaction(uint8 *pContent, int32 nLen, uint8 *pNum, int32 type);

/*对下载内容（保存在内存区中的一个下载的文件）进行判断，
若下载文件是DSM菜单，由DSM引擎对下载文件进行保存。使用
本函数时，下载文件应该已经下载完全，并且全部内容保存在
所给的内存中。*/
int32 mr_save_mrp(void *p,uint32 l);

/*用户SIM卡变更*/
int32 mr_newSIMInd(int16 type, uint8* old_IMSI);

/*注册固化应用*/
int32 mr_registerAPP(uint8 *p, int32 len, int32 index);

#endif  // _ENGINE_H
