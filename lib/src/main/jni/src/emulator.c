#include "emulator.h"

#include <jni.h>

#include <android/log.h>
#include <malloc.h>
#include <string.h>
#include <fcntl.h>
#include <asm-generic/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "encode.h"
#include "font/font_sky16_2.h"

#define USE_JBITMAP 1

#ifdef USE_JBITMAP
#include <android/bitmap.h>
#endif

//---------------------------------
T_EMUENV			gEmuEnv; //API LOG 控制
T_PROCINFO			g_procInfo;

JavaVM				*gJVM;
JNIEnv				*gMainJniEnv;
JNIEnv				*gVmJniEnv;

uint16				*cacheScreenBuffer;	//缓冲屏幕地址
uint16		        *realScreenBuffer;
int 				SCNW = 240;
int 				SCNH = 320;
int					showApiLog = TRUE;


//---------------------------------
static int   	g_bAttatedT = 0;
static char		m_bSuspendDraw;
static pid_t	m_initTid;

static jfieldID		fid_charW, fid_charH, fid_editInputContent;
static jfieldID		fid_memLen, fid_memTop, fid_memLeft;

static jmethodID	id_flush; //刷新画布函数 ID
static jmethodID	id_finish; //结束activity 
static jmethodID	id_timerStart, id_timerStop; 
static jmethodID	id_showEdit, id_showDlg;
static jmethodID	id_getIntSysinfo;
static jmethodID	id_getStringSysinfo;
static jmethodID	id_requestCallback;
static jmethodID	id_sendSms;
static jmethodID	id_getHostByName;
static jmethodID	id_setStringOptions;
static jmethodID	id_playSound, id_stopSound, id_musicLoadFile, id_musicCMD, id_startShake, id_stopShake;
static jmethodID 	id_drawChar, id_measureChar;
static jmethodID 	id_readTsfFont;
static jmethodID 	id_callVoidMethod;
static jmethodID 	id_sendMessage;
static jmethodID 	id_getBitmap;
static jmethodID 	id_drawImage;


//static jbyteArray 	jba_screenBuf;
static jobject		obj_emulator;
static jobject		obj_mrpScreen;
static jobject		obj_emuAudio;
static jobject		obj_realBitmap, obj_cacheBitmap;

static char 		runMrpPath[DSM_MAX_FILE_LEN + 1];


inline JNIEnv *getJniEnv()
{
//	LOGI("getJniEnv from %d", gettid());
	return (gettid() == m_initTid)? gVmJniEnv : gMainJniEnv;
}

int _gettid()
{
//    return syscall(_NR)
    return gettid();
}

JNIEnv * _attachJniEnv()
{
	int ret;
	JNIEnv * env = NULL;

	LOGI("_attachJniEnv tid=%d", gettid());

	ret = (*gJVM)->GetEnv(gJVM, (void **)&env, JNI_VERSION_1_6);
	if(ret == JNI_OK)
	{
		JavaVMAttachArgs arg = {JNI_VERSION_1_6, NULL, NULL};
		ret = (*gJVM)->AttachCurrentThread(gJVM, &env, &arg);
		if(ret == JNI_OK)
		{
			LOGI("_attachJniEnv suc env=%p, tid=%d", env, gettid());
			return env;
		}
	}
    LOGE("_attachJniEnv fail");

	return NULL;
}

void _detachJniEnv(JNIEnv * env)
{
	LOGI("_detachJniEnv tid=%d", gettid());

    (*gJVM)->DetachCurrentThread(gJVM);
}

static void initJniId(JNIEnv * env)
{
	//----------- Emulator.java -------------------------------------
	jclass cls = (*env)->GetObjectClass(env, obj_emulator);

	fid_charW = (*env)->GetFieldID(env, cls, "N2J_charW", "I");
	fid_charH = (*env)->GetFieldID(env, cls, "N2J_charH", "I");
	fid_editInputContent = (*env)->GetFieldID(env, cls, "N2J_editInputContent", "Ljava/lang/String;");
	fid_memTop = (*env)->GetFieldID(env, cls, "N2J_memTop", "I");
	fid_memLen = (*env)->GetFieldID(env, cls, "N2J_memLen", "I");
	fid_memLeft = (*env)->GetFieldID(env, cls, "N2J_memLeft", "I");

	id_flush = (*env)->GetMethodID(env, cls, "N2J_flush", "()V");
	id_requestCallback = (*env)->GetMethodID(env, cls, "N2J_requestCallback", "(II)V");
	id_timerStart = (*env)->GetMethodID(env, cls, "N2J_timerStart", "(I)V");
	id_timerStop = (*env)->GetMethodID(env, cls, "N2J_timerStop", "()V");
	id_showEdit = (*env)->GetMethodID(env, cls, "N2J_showEdit", "(Ljava/lang/String;Ljava/lang/String;II)V");
	id_showDlg = (*env)->GetMethodID(env, cls, "N2J_showDlg", "(Ljava/lang/String;)V");
	id_finish = (*env)->GetMethodID(env, cls, "N2J_finish", "()V");
	id_sendSms = (*env)->GetMethodID(env, cls, "N2J_sendSms", "(Ljava/lang/String;Ljava/lang/String;ZZ)I");
	id_getHostByName = (*env)->GetMethodID(env, cls, "N2J_getHostByName", "(Ljava/lang/String;)V");
	id_setStringOptions = (*env)->GetMethodID(env, cls, "N2J_setOptions", "(Ljava/lang/String;Ljava/lang/String;)V");

	id_getIntSysinfo = (*env)->GetMethodID(env, cls, "N2J_getIntSysinfo", "(Ljava/lang/String;)I");
	id_getStringSysinfo = (*env)->GetMethodID(env, cls, "N2J_getStringSysinfo", "(Ljava/lang/String;)Ljava/lang/String;");
	id_readTsfFont = (*env)->GetMethodID(env, cls, "N2J_readTsfFont", "()[B");

	id_callVoidMethod = (*env)->GetMethodID(env, cls, "N2J_callVoidMethod", "([Ljava/lang/String;)V");
	id_sendMessage = (*env)->GetMethodID(env, cls, "N2J_sendHandlerMessage", "(IIII)V");
	id_getBitmap = (*env)->GetMethodID(env, cls, "N2J_getBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
	id_drawImage = (*env)->GetMethodID(env, cls, "N2J_drawImage", "(Ljava/lang/String;IIII)V");

	(*env)->DeleteLocalRef(env, cls);

	//----------- EmuAudio.java -------------------------------------
	cls = (*env)->GetObjectClass(env, obj_emuAudio);

	id_playSound = (*env)->GetMethodID(env, cls, "N2J_playSound", "(Ljava/lang/String;I)V");
	id_stopSound = (*env)->GetMethodID(env, cls, "N2J_stopSound", "()V");
	id_musicLoadFile = (*env)->GetMethodID(env, cls, "N2J_musicLoadFile", "(Ljava/lang/String;)V");
	id_musicCMD = (*env)->GetMethodID(env, cls, "N2J_musicCMD", "(III)I");
	id_startShake = (*env)->GetMethodID(env, cls, "N2J_startShake", "(I)V");
	id_stopShake = (*env)->GetMethodID(env, cls, "N2J_stopShake", "()V");

	(*env)->DeleteLocalRef(env, cls);

	//----------- EmuScreen.java -------------------------------------
	cls = (*env)->GetObjectClass(env, obj_mrpScreen);

	id_drawChar = (*env)->GetMethodID(env, cls, "N2J_drawChar", "(IIII)V");
	id_measureChar = (*env)->GetMethodID(env, cls, "N2J_measureChar", "(I)V");

	(*env)->DeleteLocalRef(env, cls);

	LOGI("initJniId ok, id_timerStart=%p ...", id_timerStart);
}

static void freeJniId()
{
}

//------------------------
void sevg_handler(int signo)
{
	char *argv[2] = {
			"crash",
			"不小心又崩溃了%>_<%"
	};

	N2J_callVoidMethodString(2, (const char **)argv);

	sleep(2);

	exit(0);
}

//初始化模拟器  唯一实例
void j2n_create(JNIEnv *env, jobject self, jobject mrpScreen, jobject emuAudio)
{
#if 0
	signal(SIGSEGV, sevg_handler);
#endif
    LOGD("j2n_create tid=%d", gettid());

	gMainJniEnv = _attachJniEnv();
	gVmJniEnv = gMainJniEnv;
	m_initTid = gettid();
    env = gMainJniEnv;

	LOGI("j2n_create gMainJniEnv=%p, m_initTid=%d", gMainJniEnv, m_initTid);

	//直接保存 obj 到 DLL 中的全局变量是不行的,应该调用以下函数:
	obj_emulator = (*env)->NewGlobalRef(env, self);
	obj_emuAudio = (*env)->NewGlobalRef(env, emuAudio);
	obj_mrpScreen = (*env)->NewGlobalRef(env, mrpScreen);

	initJniId(env);

	gEmuEnv.showFile = TRUE;
	gEmuEnv.showInputLog = FALSE;
	gEmuEnv.showTimerLog = FALSE;
	gEmuEnv.showNet = TRUE;
	gEmuEnv.showMrPlat = TRUE;
	gEmuEnv.showFW = FALSE;

	gEmuEnv.androidDrawChar = FALSE;
	gEmuEnv.useSysScreenbuf = FALSE;
	gEmuEnv.memSize = 10; //默认4M
    gEmuEnv.enableExram = TRUE;

//	tsf_init();
	xl_font_sky16_init();

	//初始化 DSM启动时间
	gettimeofday(&gEmuEnv.dsmStartTime, NULL);

	LOGI("j2n_create self=%p,scn=%p,aud=%p  gself=%p,gscn=%p,gaud=%p",
         self, mrpScreen,emuAudio, obj_emulator, obj_mrpScreen, obj_emuAudio);
}

int j2n_startMrp(JNIEnv * env, jobject self, jstring path)
{
	const char *str = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
	if(str)
	{
		LOGD("vm_loadMrp entry:%s", str);
		UTF8ToGBString((uint8 *)str, (uint8 *)runMrpPath, sizeof(runMrpPath));

        (*env)->ReleaseStringUTFChars(env, path, str);
        (*env)->ExceptionClear(env);

//		if(!gEmuEnv.b_tsfInited)
//			gEmuEnv.androidDrawChar = 1;
		showApiLog = 1;
		gEmuEnv.showMrPrintf = 1;
        gEmuEnv.b_vm_running = 1;

        dsm_init();


#ifdef DSM_FULL
		mr_start_dsm(runMrpPath);
#else
		mr_start_dsmC("cfunction.ext", runMrpPath);
#endif
		return 1;
	}

	return -1;
}

void j2n_pause(JNIEnv *env, jobject self)
{
	LOGD("native pause!");
	m_bSuspendDraw = 1;

	if(gEmuEnv.showFW) LOGI("mr_pauseApp");

	mr_pauseApp();
}

void j2n_resume(JNIEnv *env, jobject self)
{
	LOGD("native resume!");
	m_bSuspendDraw = 0;

	if(gEmuEnv.showFW) LOGI("mr_resumeApp");

	mr_resumeApp();

	if(gEmuEnv.b_vm_running)
		emu_bitmapToscreen(cacheScreenBuffer, 0, 0, screenW, screenH);
}

void j2n_stop(JNIEnv *env, jobject self)
{
	LOGD("native stop!");

	if(gEmuEnv.showFW) LOGI("mr_stop");


	//仅仅是通知调用 mrc_exit()
	mr_stop();
	//最后执行
	mr_exit();
}

void j2n_event(JNIEnv *env, jobject self, jint code, jint p0, jint p1)
{
	if(gEmuEnv.showFW) LOGI("mr_event(%d, %d, %d)", code, p0, p1);

	if(code == MR_SMS_GET_SC) { //获取短信中心
		p0 = (jint)dsmSmsCenter; //如果 java 层实现了，应该从java层读取信息
		p1 = 0;
	} else if(code == MR_EMU_ON_TIMER) {
		LOGI("call mr_timer");
		mr_timer();
		return;
	}

	mr_event(code, p0, p1);
}

int j2n_smsRecv(JNIEnv *env, jobject self, jstring content, jstring num)
{
	if(showApiLog) LOGD("vm_smsIndiaction");

	int32 ret = MR_IGNORE;
	const char *numStr, *contentStr;

	numStr = (*env)->GetStringUTFChars(env, num, JNI_FALSE);
	if (numStr) {
		uint8 buf[64];

		UTF8ToGBString((uint8 *)numStr, buf, sizeof(buf));

		contentStr = (*env)->GetStringUTFChars(env, content, JNI_FALSE);
		if(contentStr){
			uint8 buf2[1024];

			UTF8ToGBString((uint8 *)contentStr, buf2, sizeof(buf2));

			ret = mr_smsIndiaction((uint8 *)buf2, strlen(buf2), (uint8 *)buf, MR_ENCODE_ASCII);

			(*env)->ReleaseStringUTFChars(env, content, contentStr);
		}

		(*env)->ReleaseStringUTFChars(env, num, numStr);
	}
	return ret;
}

void j2n_destroy(JNIEnv * env, jobject self)
{
	(*env)->DeleteGlobalRef(env, obj_emulator);
	(*env)->DeleteGlobalRef(env, obj_emuAudio);
	(*env)->DeleteGlobalRef(env, obj_mrpScreen);
	(*env)->DeleteGlobalRef(env, obj_realBitmap);

//	(*env)->ReleaseByteArrayElements(env, jba_screenBuf, jScreenBuf, 0);

	gJVM = NULL;
	obj_emulator = NULL;
//	tsf_dispose();
	xl_font_sky16_close();

	LOGI("native destroy");
}

/**
 * native 与 java 的回调中介
 */
void j2n_callback(JNIEnv * env, jobject self, int what, int param)
{
	if(!gEmuEnv.b_vm_running) {
		LOGW("native_callback vm exited!");
		return;
	}

	LOGD("native_callback %d,%d", what, param);

	switch(what)
	{
	case CALLBACK_PLAYMUSIC:
		if(dsmMediaPlay.cb)
			dsmMediaPlay.cb(param);
		break;

	case CALLBACK_GETHOSTBYNAME:
		LOGI("getHost callback ip:%p", (void*)param);
		((MR_GET_HOST_CB)mr_soc.callBack)(param);
		break;

	case CALLBACK_TIMER_OUT:
		mr_timer();
		break;
	}
}

void j2n_screenRest(JNIEnv * env, jobject self, jobject cacheBitmap, jobject realBitmap, jint width, jint height)
{
    LOGD("j2n_screenRest cache=%p real=%p %dx%d", cacheBitmap, realBitmap, width, height);

	if (!cacheBitmap || !realBitmap || width <= 0 || height <= 0) {
		LOGE("j2n_screenRest error params!");
		return;
	}

	obj_realBitmap = (*env)->NewGlobalRef(env, realBitmap);
	obj_cacheBitmap = (*env)->NewGlobalRef(env, cacheBitmap);

	SCNW = width;
	SCNH = height;

#if USE_JBITMAP
	AndroidBitmapInfo info;
	void* pixels;
	int ret;

	if ((ret = AndroidBitmap_lockPixels(env, cacheBitmap, &pixels)) >= 0) {
		cacheScreenBuffer = (uint16 *)pixels;
		AndroidBitmap_unlockPixels(env, cacheBitmap);
	} else {
		LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
		return;
	}

	if ((ret = AndroidBitmap_lockPixels(env, realBitmap, &pixels)) >= 0) {
		realScreenBuffer = (uint16 *)pixels;
		AndroidBitmap_unlockPixels(env, realBitmap);
	} else {
		LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
		return;
	}
#else
	cacheScreenBuffer = (uint16 *)malloc(SCNW * SCNH * 2);
#endif

    screenBuf = cacheScreenBuffer;
}

jstring j2n_getStringOptions(JNIEnv * env, jobject self, jstring key)
{
	const char *str, *str2;
	jstring ret = NULL;


	str = (*env)->GetStringUTFChars(env, key, JNI_FALSE);
	if(str)
	{
		if (strcasecmp(str, "memSize") == 0)
		{
			char buf[24];
			sprintf(buf, "%d", gEmuEnv.memSize);
			ret = (*env)->NewStringUTF(env, buf);
		}

		(*env)->ReleaseStringUTFChars(env, key, str);
	}

	return ret;
}

void j2n_setStringOptions(JNIEnv * env, jobject self, jstring key, jstring value)
{
	const char *str, *str2;
	char buf[128];

	str = (*env)->GetStringUTFChars(env, key, JNI_FALSE);
	str2 = (*env)->GetStringUTFChars(env, value, JNI_FALSE);

	if(str && str2)
	{
		if (strcasecmp(str, "sdpath") == 0) {
			SetDsmSDPath(str2);
		} else if (strcasecmp(str, "mythroadPath") == 0) {
			//mythroad 路径
			SetDsmPath(str2);
		} else if (strcasecmp(str, "dsmFactory") == 0) {
			strncpy(dsmFactory, str2, 7);
		} else if (strcasecmp(str, "dsmType") == 0) {
			strncpy(dsmType, str2, 7);
		} else if (strcasecmp(str, "smsCenter") == 0) {
			strncpy(dsmSmsCenter, str2, sizeof(dsmSmsCenter));
		} else if (strcasecmp(str, "smsCenter") == 0) {
			strncpy(dsmSmsCenter, str2, sizeof(dsmSmsCenter));
		} else if (strcasecmp(str, "procName") == 0) {
			LOGD("mrp running process = %s", str2);
			strncpy(g_procInfo.name, str2, sizeof(g_procInfo.name));
		}
		/*else if (strcasecmp(str, "platscreenbuf") == 0)
		{

		}*/
	}

	if(str) (*env)->ReleaseStringUTFChars(env, key, str);
	if(str2) (*env)->ReleaseStringUTFChars(env, value, str2);
}

void j2n_setIntOptions(JNIEnv * env, jobject self, jstring key, jint value)
{
	const char *str;

	str = (*env)->GetStringUTFChars(env, key, JNI_FALSE);
	if(str)
	{
//		if(showApiLog)
//			LOGI("setOperation(%s, %d)", str, value);
		if(strcasecmp(str, "platdrawchar") == 0){
			gEmuEnv.androidDrawChar = 0;//(value);
		}else if(strcasecmp(str, "enableApilog") == 0){
			showApiLog = value;
			if(!showApiLog){
				memset(&gEmuEnv, 0, sizeof(gEmuEnv));
			}
		}else if(strcasecmp(str, "enable_log_input") == 0){
			gEmuEnv.showInputLog = showApiLog && value;
		}else if(strcasecmp(str, "enable_log_file") == 0){
			gEmuEnv.showFile = showApiLog && value;
		}else if(strcasecmp(str, "enable_log_net") == 0){
			gEmuEnv.showNet = showApiLog && value;
		}else if(strcasecmp(str, "enable_log_mrplat") == 0){
			gEmuEnv.showMrPlat = showApiLog && value;
		}else if(strcasecmp(str, "enable_log_timer") == 0){
			gEmuEnv.showTimerLog = showApiLog && value;
		}else if(strcasecmp(str, "enable_log_fw") == 0){
			gEmuEnv.showFW = showApiLog && value;
		}else if(strcasecmp(str, "enable_log_mrprintf") == 0){
			gEmuEnv.showMrPrintf = showApiLog && value;
		}else if(strcasecmp(str, "enableExram") == 0){
			gEmuEnv.enableExram = value;
		}else if(strcasecmp(str, "platform") == 0){
			gEmuEnv.platform = value;
		}else if(strcasecmp(str, "memSize") == 0){
			gEmuEnv.memSize = value;
		}

		(*env)->ReleaseStringUTFChars(env, key, str);
	}
}

void j2n_getMemoryInfo(JNIEnv * env, jobject self)
{
	uint32 len, left, top;
	mr_getMemoryInfo(&len, &left, &top);

	(*env)->SetIntField(env, self, fid_memTop, (jint)top);
	(*env)->SetIntField(env, self, fid_memLen, (jint)len);
	(*env)->SetIntField(env, self, fid_memLeft, (jint)left);
}

jint j2n_handleMessage(JNIEnv * env, jobject self, jint what, jint p0, jint p1)
{
	LOGD("j2n_handleMessage %d,%d,%d", what, p0, p1);

    switch(what)
    {
        case EMU_MSG_GET_HSOT:
        {
            ((MR_GET_HOST_CB)mr_soc.callBack)((int32)p0);
            return 1;
        }
    }

    return 0;
}

jstring j2n_getAppName(JNIEnv * env, jobject self, jstring path)
{
	const char *str;

	str = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
	if(str){
		int fd;
		uint8 buf[128]={0};
		uint8 utf8[256]={0};
		jbyteArray ba;

		fd = open(str, O_RDONLY);
		(*env)->ReleaseStringUTFChars(env, path, str);
		if(fd >= 0){
			lseek(fd, 28, 0);
			read(fd, buf, 24);
			close(fd);
		}

		GBToUTF8String(buf, utf8, sizeof(utf8));
		return (*env)->NewStringUTF(env, (char *)utf8);
	}

	return NULL;
}

void j2n_lockBitmap(JNIEnv * env, jobject self)
{
	(*env)->MonitorEnter(env, obj_realBitmap);
}

void j2n_unLockBitmap(JNIEnv * env, jobject self)
{
	(*env)->MonitorExit(env, obj_realBitmap);
}

///////////// N->J /////////////////////////////////////////////
static void fix_x0y0x1y1(int *x0, int *y0, int *x1, int *y1)
{
	if(*x0 > *x1){ //交换
		int tmp = *x0;
		*x0 = *x1;
		*x1 = tmp;
	}

	if(*y0 > *y1){ //交换
		int tmp = *y0;
		*y0 = *y1;
		*y1 = tmp;
	}
}

static int clip_rect(int *x0, int *y0, int *x1, int *y1, int r, int b)
{
	fix_x0y0x1y1(x0, y0, x1, y1);

	//超出的情况
	if (*x0>r || *y0>b || *x1<0 || *y1<0)
		return 1;

	//根据Clip修正后的 x y r b 
	*x0 = MAX(*x0, 0);
	*y0 = MAX(*y0, 0);
	*x1 = MIN(*x1, r);
	*y1 = MIN(*y1, b);

	return 0;
}

void emu_bitmapToscreen(uint16* data, int x, int y, int w, int h)
{
	if(m_bSuspendDraw)
		return;

	int ret;
	int				x1, y1, r, b;
	int32			sw=SCNW, sh=SCNH;
	
	if(x>=sw || y>=sh || w<=0 || h<=0)
		return;

	r = sw-1, b = sh-1;
	x1 = x+w-1, y1 = y+h-1;
	clip_rect(&x, &y, &x1, &y1, r, b);
	w = x1-x+1, h = y1-y+1;
	
	JNIEnv *e = getJniEnv();
	void* pixels = realScreenBuffer;

    uint16 *p = (uint16 *)pixels;
    int i;
    for (i = y; i <= y1; i++)
        memcpy((p + (sw * i) + x), (data + (sw * i) + x), w * 2);

	(*e)->CallVoidMethod(e, obj_emulator, id_flush);

	(*e)->ExceptionClear(e);
}

int32 emu_timerStart(uint16 t)
{
	JNIEnv *e = getJniEnv();

    LOGI("emu_timerStart %d", t);
//	LOGI("emu_timerStart jni=%p,obj=%p,id=%x,t=%d,tid=%d", jniEnv, obj_emulator, id_timerStart, t, gettid());

	(*e)->CallVoidMethod(e, obj_emulator, id_timerStart, (int)t);

//	(*e)->ExceptionClear(e);

	return MR_SUCCESS;
}

int32 emu_timerStop()
{
	JNIEnv *e = getJniEnv();

	(*e)->CallVoidMethod(e, obj_emulator, id_timerStop);

    (*e)->ExceptionClear(e);

	return MR_SUCCESS;
}

/**
 * 多线程调用
 * @param what
 * @param p0
 * @param p1
 * @param delay
 */
void emu_sendHandlerMessage(int what, int p0, int p1, int delay)
{
	LOGI("emu_sendHandlerMessage %d %d,%d %d", what, p0, p1, delay);

    JNIEnv *env = NULL;

    if(gettid() == m_initTid) {
        env = getJniEnv();
        (*env)->CallVoidMethod(env, obj_emulator, id_sendMessage, what, p0, p1, delay);
    } else {
        int ret = (*gJVM)->GetEnv(gJVM, (void **) &env, JNI_VERSION_1_6);
        if (ret == JNI_OK) {
            ret = (*gJVM)->AttachCurrentThread(gJVM, &env, NULL);
            if (ret == JNI_OK) {
                (*env)->CallVoidMethod(env, obj_emulator, id_sendMessage, what, p0, p1, delay);
                (*env)->ExceptionClear(env);

                (*gJVM)->DetachCurrentThread(gJVM);

                LOGI(" suc. %d %d", what, p0);
            } else
                LOGE(" error: AttachCurrentThread failed");
        } else {
            LOGE(" error: GetEnv failed");
        }
    }
}

void emu_requestCallback(int what, int param)
{
    LOGI("emu_requestCallback(%d, %d)", what, param);

    if(!gEmuEnv.b_vm_running) {
        LOGW(" error: vm has exited!");
        return;
    }

    int status;
    JNIEnv *env ;

    status = (*gJVM)->GetEnv(gJVM, (void **) &env, JNI_VERSION_1_6);
    if (status < 0)
    {
        status = (*gJVM)->AttachCurrentThread(gJVM, &env, NULL);
        if (status < 0)
        {
            LOGE(" error: failed to attach current thread!");
        }else{
            (*env)->CallVoidMethod(env, obj_emulator, id_requestCallback, what, param);
            (*gJVM)->DetachCurrentThread(gJVM);

            LOGI(" suc. %d %d", what, param);
        }
    }
}

void N2J_callVoidMethodString(int argc, const char *argv[])
{
	JNIEnv *e = getJniEnv();

//	LOGE("N2J_callVoidMethodString %d,%s", argc, argv[0]);

	jclass clsString = (*e)->FindClass(e, "java/lang/String");
	jobjectArray arr = (*e)->NewObjectArray(e, argc, clsString, NULL);
	if(arr != NULL) {
		int i;
		for(i=0; i<argc; ++i){
			jstring str = (*e)->NewStringUTF(e, argv[i]);
			(*e)->SetObjectArrayElement(e, arr, i, str);
		}

		(*e)->CallVoidMethod(e, obj_emulator, id_callVoidMethod, arr);
	}
    (*e)->ExceptionClear(e);
}

void N2J_callVoidMethodStringJ(int argc, jstring argv[])
{
	JNIEnv *jniEnv = getJniEnv();

	jclass clsString = (*jniEnv)->FindClass(jniEnv, "java/lang/String");
	jobjectArray arr = (*jniEnv)->NewObjectArray(jniEnv, argc, clsString, NULL);
	if(arr != NULL){
		int i;
		for(i=0; i<argc; ++i){
			(*jniEnv)->SetObjectArrayElement(jniEnv, arr, i, argv[i]);
		}
		(*jniEnv)->CallVoidMethod(jniEnv, obj_emulator, id_callVoidMethod, arr);
	}
}

void emu_showDlg(const char *msg)
{
	if(!msg)
        return;

	jstring jstr = NULL;
	JNIEnv *e = getJniEnv();
    static char buf[256];

	GBToUTF8String((uint8 *)msg, (uint8 *)buf, sizeof(buf));
	jstr = (*e)->NewStringUTF(e, buf);

    (*e)->CallVoidMethod(e, obj_emulator, id_showDlg, jstr);
    (*e)->ExceptionClear(e);
}

typedef struct {
	char *buf_title;
	char *buf_text;
	char *buf_content;
}T_MR_EDIT, *PT_MR_EDIT;

int32 emu_showEdit(const char * title, const char * text, int type, int max_size)
{
	jstring stitle = NULL, stext = NULL;
	char *buf;
	int l;
	T_MR_EDIT *edit = malloc(sizeof(T_MR_EDIT));
	JNIEnv *jniEnv = getJniEnv();

	if(showApiLog)
		LOGI("emu_showEdit");

	memset(edit, 0, sizeof(T_MR_EDIT));
	if(title){
		l = UCS2_strlen(title);
		buf = malloc(l+2);
		memset(buf, 0, l+2);
		memcpy(buf, title, l);
		UCS2ByteRev(buf); //unicode大端转小端
		stitle = (*jniEnv)->NewString(jniEnv, (const jchar*)buf, l/2);

		edit->buf_title = buf;
	} else {
		stitle = (*jniEnv)->NewStringUTF(jniEnv, "\x0\x0");
	}
	
	if(text) {
		l = UCS2_strlen(text);
		buf = malloc(l+2);
		memset(buf, 0, l+2);
		memcpy(buf, text, l);
		UCS2ByteRev(buf);
		stext = (*jniEnv)->NewString(jniEnv, (const jchar*)buf, l/2);

		edit->buf_text = buf;
	} else {
		stext = (*jniEnv)->NewStringUTF(jniEnv, "\x0\x0");
	}

	(*jniEnv)->CallVoidMethod(jniEnv, obj_emulator, id_showEdit, stitle, stext, type, max_size);
    (*jniEnv)->ExceptionClear(jniEnv);

	return (int32)edit;
}

/**
 * 返回编辑框内容的起始地址(编辑框的内容指针，大端unicode编码)，如果失败返回NULL.
 */
const char* emu_getEditInputContent(int32 editHd)
{
	if(!editHd)
		return NULL;

	JNIEnv *jniEnv = getJniEnv();

	jstring text = (jstring)(*jniEnv)->GetObjectField(jniEnv, obj_emulator, fid_editInputContent);
	T_MR_EDIT *edit = (PT_MR_EDIT)editHd;

	if(text != NULL){
		int len = (*jniEnv)->GetStringLength(jniEnv, text); //返回 unicode 个数（字节数/2）
		if(len > 0){
			const jchar * str = (*jniEnv)->GetStringChars(jniEnv, text, JNI_FALSE); //返回 unicode 小端
			if(str != NULL){
				len *= 2;

				char *content = malloc(len + 2);
				memset(content, 0, len+2);
				memcpy(content, str, len);
				UCS2ByteRev(content); //转为大端

				edit->buf_content = content;

				(*jniEnv)->ReleaseStringChars(jniEnv, text, str);

				return content;
			}
		}
	} 
	
	//貌似直接返回空指针会挂
	edit->buf_content = malloc(8);
	memset(edit->buf_content, 0, 8);

	return edit->buf_content;
}

void emu_releaseEdit(int32 editHd)
{
	if(editHd){
		T_MR_EDIT *edit = (PT_MR_EDIT)editHd;

		FREE_SET_NULL(edit->buf_content);
		FREE_SET_NULL(edit->buf_text);
		FREE_SET_NULL(edit->buf_title);

		free(edit);
	}
}

void emu_finish()
{
	JNIEnv *e = getJniEnv();
	(*e)->CallVoidMethod(e, obj_emulator, id_finish);
    (*e)->ExceptionClear(e);
}

int emu_getIntSysinfo(const char * name)
{
	JNIEnv *e = getJniEnv();

	int i = (*e)->CallIntMethod(e, obj_emulator,
			id_getIntSysinfo,
			(*e)->NewStringUTF(e, name));
    (*e)->ExceptionClear(e);

	return i;
}

void emu_getHostByName(const char * name)
{
	if(!name)
		return;

	JNIEnv *e = getJniEnv();
	(*e)->CallVoidMethod(e, obj_emulator, id_getHostByName, (*e)->NewStringUTF(e, name));
    (*e)->ExceptionClear(e);
}

void emu_setStringOptions(const char *key, const char *value)
{
	if(!key || !value)
		return;

	JNIEnv *e = getJniEnv();

	(*e)->CallVoidMethod(e, obj_emulator, id_setStringOptions,
			(*e)->NewStringUTF(e, key),
			(*e)->NewStringUTF(e, value)
			);
    (*e)->ExceptionClear(e);
}

const char *emu_getStringSysinfo(const char * name)
{
	JNIEnv *e = getJniEnv();

	LOGI("emu_getStringSysinfo env=%p,tid=%d", e, gettid());

	jstring text = (jstring)(*e)->CallObjectMethod(e, obj_emulator, id_getStringSysinfo, (*e)->NewStringUTF(e, name));
    (*e)->ExceptionClear(e);

	if(text != NULL){
		const char *str = (*e)->GetStringUTFChars(e, text, JNI_FALSE);
		if(str != NULL){
			int l = strlen(str) + 1;
			char *content = malloc(l);
			memcpy(content, str, l);

			(*e)->ReleaseStringUTFChars(e, text, str);
            (*e)->ExceptionClear(e);

			return content;
		}
	}

	LOGE("emu_getStringSysinfo ret null");

	return NULL;
}



/**
 * 发送短信接口
 *
 * 2013-3-26 14:51:08
 */
int emu_sendSms(char *pNumber, char *pContent, int32 flags)
{
	if(!pNumber || !pContent)
		return MR_FAILED;

	LOGD("emu_sendSms, msg addr: %s", pNumber);

	jstring num, msg;
	JNIEnv *jniEnv = getJniEnv();

	num = (*jniEnv)->NewStringUTF(jniEnv, pNumber);

	if((flags&0x07) == MR_ENCODE_UNICODE){
		int l = UCS2_strlen(pContent);
		char *tmp = malloc(l + 2);

		memcpy(tmp, pContent, l);
		tmp[l] = 0;
		tmp[l+1] = 0;
		UCS2ByteRev(tmp);

		msg = (*jniEnv)->NewString(jniEnv, (const jchar*)tmp, l/2);

		free(tmp);
	}else{
		int l = strlen(pContent);
		int l2 = (l+1)*2;
		uint8 *buf = malloc(l2);

		GBToUTF8String((uint8 *)pContent, buf, l2);
		LOGD("  (gb)msg body: %s", buf);

		msg = (*jniEnv)->NewStringUTF(jniEnv, (char *)buf);
		free(buf);
	}

	return (*jniEnv)->CallIntMethod(jniEnv, obj_emulator, id_sendSms,
			num, msg, flags&0x08, flags&0x10);
}

/**
 * 读取字体
 *
 * 2013-4-19 9:24:35
 */
void N2J_readTsfFont(uint8 **outbuf, int32 *outlen)
{
	JNIEnv *jniEnv = getJniEnv();

	jbyte *buf = NULL;
	jint len = 0;

//	LOGI("N2J_readTsfFont %p %p", jniEnv, id_readTsfFont);

	jbyteArray jba = (*jniEnv)->CallObjectMethod(jniEnv, obj_emulator, id_readTsfFont);

	if(jba) {
		LOGI("N2J_readTsfFont array=%p", jba);

		len = (*jniEnv)->GetArrayLength(jniEnv, jba);
		if(len > 0) {
			buf = malloc(len);
			(*jniEnv)->GetByteArrayRegion(jniEnv, jba, 0, len, buf);

			LOGD("read tsf from assets suc! ");
		}

		(*jniEnv)->DeleteLocalRef(jniEnv, jba);
	}

	*outbuf = (uint8 *)buf;
	*outlen = len;
}

jobject N2J_getBitmap(const char *path)
{
	JNIEnv *jniEnv = getJniEnv();

	return (*jniEnv)->CallObjectMethod(jniEnv, obj_emulator, id_getBitmap,
			(*jniEnv)->NewStringUTF(jniEnv, path));
}

void N2J_drawImage(const char *path, int x, int y, int w, int h)
{
	JNIEnv *jniEnv = getJniEnv();

	(*jniEnv)->CallVoidMethod(jniEnv, obj_emulator, id_drawImage,
			(*jniEnv)->NewStringUTF(jniEnv, path), x, y, w, h);
}

void emu_getImageSize(const char *path, int *w, int *h)
{
	JNIEnv *jniEnv = getJniEnv();
	jobject jo = N2J_getBitmap(path);

	*w = 0;
	*h = 0;

	if(jo != NULL) {
		AndroidBitmapInfo t;

		if(ANDROID_BITMAP_RESULT_SUCCESS == AndroidBitmap_getInfo(jniEnv, jo, &t)) {
			*w = t.width;
			*h = t.height;
		}
	}
}

void emu_drawPix(uint16 *src, int w, int h, int x, int y)
{
	int ret;
	int x1, y1, r, b;
	int32 sw = SCNW, sh = SCNH;

	if (x >= sw || y >= sh || w <= 0 || h <= 0)
		return;

	r = sw - 1, b = sh - 1;
	x1 = x + w - 1, y1 = y + h - 1;
	clip_rect(&x, &y, &x1, &y1, r, b);
	w = x1 - x + 1, h = y1 - y + 1;

	uint16 *p = (uint16 *) cacheScreenBuffer;
	int i;
	for (i = y; i <= y1; i++)
		memcpy((p + (sw * i) + x), (src + (sw * i) + x), w * 2);
}

void emu_drawImage(const char *path, int x, int y, int w, int h)
{
	N2J_drawImage(path, x, y, w, h);
}

void emu_drawImage3(const char *path, int x, int y, int w, int h)
{
	JNIEnv *jniEnv = getJniEnv();
	jobject jo = N2J_getBitmap(path);

	if(jo != NULL) {
		AndroidBitmapInfo t;

		if(ANDROID_BITMAP_RESULT_SUCCESS == AndroidBitmap_getInfo(jniEnv, jo, &t)) {
			void *addr = NULL;
			w = t.width;
			h = t.height;

			if(ANDROID_BITMAP_RESULT_SUCCESS == AndroidBitmap_lockPixels(jniEnv, jo, &addr)) {
				emu_drawPix((uint16 *)addr, w, h, x, y);
			}
		}
	}
}

//-------------- Begin EmuScreen.java ------------------------------------------------
static int rgb565_to_rgb888(int p)
{
	return 0xff000000 | (p<<8 & 0xff0000) | (p<<5 & 0xff00) | (p<<3 & 0xff);
}

void emu_drawChar(uint16 ch, int x, int y, uint16 color)
{
	JNIEnv *jniEnv = getJniEnv();
	(*jniEnv)->CallVoidMethod(jniEnv, obj_mrpScreen, id_drawChar, (jint)ch, x, y, rgb565_to_rgb888((int)color));
}

void emu_measureChar(uint16 ch, int *w, int *h)
{
	*w = 20; *h = 20;
//	jint ret;
//	JNIEnv *jniEnv = getJniEnv();
//
//	(*jniEnv)->CallVoidMethod(jniEnv, obj_mrpScreen, id_measureChar, (jint)ch);
//	ret = (*jniEnv)->GetIntField(jniEnv, obj_emulator, fid_charW);
//	if(w != NULL)
//		*w = ret;
//	ret = (*jniEnv)->GetIntField(jniEnv, obj_emulator, fid_charH);
//	if(h != NULL)
//		*h = ret;
}
//-------------- End EmuScreen.java ------------------------------------------------


//-------------- Begin EmuAudio.java ------------------------------------------------
void emu_palySound(const char *path, int loop)
{
	JNIEnv *e = getJniEnv();
	LOGD("emu_palySound %s %p", path, e);

	char buf[128] = {0};
    strncpy(buf, path, 128);

	jstring jspath = (*e)->NewStringUTF(e, buf);
    (*e)->ExceptionClear(e);
    LOGE("------------1");

	(*e)->CallVoidMethod(e, obj_emuAudio, id_playSound, jspath, loop);
	(*e)->ExceptionClear(e);
}

void emu_stopSound(int id)
{
	JNIEnv *jniEnv = getJniEnv();

	(*jniEnv)->CallVoidMethod(jniEnv, obj_emuAudio,
						id_stopSound);
}

void emu_musicLoadFile(const char *path)
{
	JNIEnv *jniEnv = getJniEnv();

	(*jniEnv)->CallVoidMethod(jniEnv, obj_emuAudio,
							id_musicLoadFile,
							(*jniEnv)->NewStringUTF(jniEnv, path));
}

int emu_musicCMD(int cmd, int arg0, int arg1)
{
	JNIEnv *jniEnv = getJniEnv();

	return (*jniEnv)->CallIntMethod(jniEnv, obj_emuAudio,
								id_musicCMD,
								cmd, arg0, arg1);
}

void emu_startShake(int ms)
{
	JNIEnv *jniEnv = getJniEnv();

	(*jniEnv)->CallVoidMethod(jniEnv, obj_emuAudio, id_startShake, ms);
	(*jniEnv)->ExceptionClear(jniEnv);
}


void emu_stopShake()
{
	JNIEnv *jniEnv = getJniEnv();

	(*jniEnv)->CallVoidMethod(jniEnv, obj_emuAudio, id_stopShake);
	(*jniEnv)->ExceptionClear(jniEnv);
}
