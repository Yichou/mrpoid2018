#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <jni.h>
#include <linux/types.h>
#include <time.h>
#include <android/log.h>
#include <pthread.h>

//#include <mr/mrporting.h>

#include "mr_types.h"
#include "mr_helper.h"
#include "mr_plat.h"
#include "utils.h"
#include "dsm.h"
//#include "vm.h"


#define LOG_TAG "mrpoid_jni"

#define LOGI(...) \
	((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) \
	((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) \
	((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#define LOGD(...) \
	((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))


enum {
	CALLBACK_PLAYMUSIC = 0x1001,
	CALLBACK_GETHOSTBYNAME,
	CALLBACK_TIMER_OUT,

	CALLBACK_MAX
};

typedef enum {
	EMU_MSG_STATR = 0x10000,
	EMU_MSG_GET_HSOT,

	EMU_MSG_MAX
}E_EMU_MSGID;


typedef struct _EmuEnv {
	int showInputLog; //系统事件输入logo
	int showFile; //文件 I/O
	int showMrPlat; //mr_plat/mr_platEx
	int showNet; //网络
	int showTimerLog; //定时器监视
	int showFW; //显示框架消息
	int showMrPrintf; //mr_printf
	int androidDrawChar;	//使用 android 绘制字符
	int useSysScreenbuf;	//使用系统屏幕缓冲区
	int enableExram;		//拓展内存
	int platform;			//平台号
	int memSize;			//内存大小（单位 M）
	int b_tsfInited;		//tsf 加载结果
	int b_vm_running;		//vm正在运行标志
	int font_sky_status;    //sky字体加载结果(风的影子)
	struct timeval dsmStartTime; //虚拟机初始化时间，用来计算系统运行时间
	char* vm_mem_base;		//虚拟机内存地址
	int32 vm_mem_len;		//虚拟机内存大小
	char* vm_mem_end;		//虚拟机内存地址
	char* exMem;			//拓展内存地址
	uint16 *cacheScreenBuffer;	//缓冲屏幕地址
	uint16 *screenBuffer;	//缓冲屏幕地址
}T_EMUENV;



typedef struct {
	char name[64];
	int index;
	int pid;
}T_PROCINFO;

typedef struct {
	char *title;
	char ** items;
	int itemCount;
}T_MR_MENU, *PT_MR_MENU;


typedef int32 (*MR_CALLBACK)(int32 result);


//--- 万能函数 标示 ----------------------------
#define EMU_FUNC_menuCreate "menuCreate"
#define EMU_FUNC_menuSetItem "menuSetItem"
#define EMU_FUNC_menuShow "menuShow"
#define EMU_FUNC_menuSetFocus "menuSetFocus"
#define EMU_FUNC_menuRelease "menuRelease"
#define EMU_FUNC_menuRefresh "menuRefresh"

#define EMU_FUNC_dialogCreate "dialogCreate"
#define EMU_FUNC_dialogRelease "dialogRelease"
#define EMU_FUNC_dialogRefresh "dialogRefresh"

#define EMU_FUNC_textCreate "textCreate"
#define EMU_FUNC_textRelease "textRelease"
#define EMU_FUNC_textRefresh "textRefresh"

//#define EMU_FUNC_ ""



//--- 获取系统信息的 key ----------------------------
#define SYSINFO_NETTYPE 	"netType"
#define SYSINFO_IMSI 		"imsi"
#define SYSINFO_IMEI 		"imei"


//--- DSM 配置参数 ----------------------------
#define SCNBIT				16
#define DSM_MAX_FILE_LEN	256
#define DSM_MAX_NAME_LEN	128
#define MAX_IMEI_LEN		15
#define MAX_IMSI_LEN		15
#define MAX_SMS_CENTER_LEN	15


//------- 全局变量 -----------------------------------------
extern int 			SCNW;
extern int 			SCNH;
#define screenW 	SCNW
#define screenH 	SCNH

extern JavaVM		*gJVM;
extern JNIEnv		*gMainJniEnv;
extern JNIEnv		*gVmJniEnv;

extern T_EMUENV			gEmuEnv; //API LOG 控制

extern int					showApiLog;
extern uint16				*cacheScreenBuffer;	//缓冲屏幕地址
extern mr_socket_struct 	mr_soc;
extern T_DSM_MEDIA_PLAY 	dsmMediaPlay; //音乐播放接口回调
extern int					dsmNetType;
extern char					dsmSmsCenter[MAX_SMS_CENTER_LEN+1];
extern T_PROCINFO			g_procInfo;



//------- 宏函数 ----------------------------------------------


#define FREE_SET_NULL(ptr) \
	if(ptr){ \
		free(ptr); ptr = NULL; \
	}

#define MKDIR(as) \
	mkdir(as, 0777)

#define CHECK_AND_REMOVE(as) \
	do{\
		if(MR_IS_FILE == getFileType(as)) \
			remove(as); \
	}while(0)

#define MR_CHECK_AND_REMOVE(as) \
	do{\
		if(MR_IS_FILE == mr_info(as)) \
			mr_remove(as); \
	}while(0)


///////// 注册方法 J->N /////////////////////////////////////////
void j2n_create(JNIEnv *env, jobject self, jobject mrpScreen, jobject emuAudio);
int j2n_startMrp(JNIEnv * env, jobject self, jstring path);
void j2n_pause(JNIEnv *env, jobject self);
void j2n_resume(JNIEnv *env, jobject self);
void j2n_stop(JNIEnv *env, jobject self);
void j2n_event(JNIEnv *env, jobject self, jint code, jint p0, jint p1);
int j2n_smsRecv(JNIEnv *env, jobject self, jstring content, jstring num);

void j2n_destroy(JNIEnv * env, jobject self);
int j2n_handleMessage(JNIEnv * env, jobject self, jint what, jint p0, jint p1);

void j2n_setIntOptions(JNIEnv * env, jobject self, jstring key, jint value);
jstring j2n_getStringOptions(JNIEnv * env, jobject self, jstring key);
void j2n_setStringOptions(JNIEnv * env, jobject self, jstring key, jstring value);
void j2n_callback(JNIEnv * env, jobject self, int what, int param);
jstring j2n_getAppName(JNIEnv * env, jobject self, jstring path);
void j2n_getMemoryInfo(JNIEnv * env, jobject self);

void j2n_screenRest(JNIEnv * env, jobject self, jobject cacheBitmap, jobject realBitmap, jint width, jint height);
void j2n_lockBitmap(JNIEnv * env, jobject self);
void j2n_unLockBitmap(JNIEnv * env, jobject self);

extern void mr_getMemoryInfo(uint32 *total, uint32 *free, uint32 *top);

JNIEnv *emu_attachJniEnv();
void emu_detachJniEnv();
void setJniEnv(JNIEnv *p);
JNIEnv *getJniEnv();

////////// 平台相关个方法 N->J //////////////////////////////////
void emu_bitmapToscreen(uint16* data, int x, int y, int w, int h);
void emu_drawChar(uint16 ch, int x, int y, uint16 color);
int32 emu_timerStart(uint16 t);
int32 emu_timerStop(void);
void emu_measureChar(uint16 ch, int *w, int *h);

int32 emu_showEdit(const char * title, const char * text, int type, int max_size);
const char* emu_getEditInputContent(int32 editHd);
void emu_releaseEdit(int32 editHd);

void emu_finish();
void emu_showDlg(const char *msg);
int emu_getIntSysinfo(const char * name);
const char * emu_getStringSysinfo(const char * name);
void emu_palySound(const char *path, int loop);
void emu_stopSound(int id);
void emu_startShake(int ms);
void emu_stopShake();
void emu_musicLoadFile(const char *path);
int emu_musicCMD(int cmd, int arg0, int arg1);
void emu_requestCallback(int what, int param);
int emu_sendSms(char *pNumber, char *pContent, int32 flags);
void emu_getHostByName(const char * name);
void emu_setStringOptions(const char *key, const char *value);

void N2J_readTsfFont(uint8 **outbuf, int32 *outlen);

void N2J_callVoidMethodString(int argc, const char** argv);

/*send handler message to mrp thread*/
void emu_sendHandlerMessage(int what, int p0, int p1, int delay);

void emu_getImageSize(const char *path, int *w, int *h);
void emu_drawImage(const char *path, int x, int y, int w, int h);

void SetDsmPath(const char *path);

extern int32 mr_start_dsmC(char* start_file, const char* entry);

#define isAddrValid(p) \
    (p >= gEmuEnv.vm_mem_base && p < gEmuEnv.vm_mem_end)

#endif
