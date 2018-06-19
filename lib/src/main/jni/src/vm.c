#include "vm.h"

#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>

#include <lib/TimeUtils.h>
#include <lib/JniUtils.h>
#include <msgqueue.h>
#include <message.h>
#include <mr_plat.h>
#include <mrporting.h>
#include "encode.h"

static char 		runMrpPath[DSM_MAX_FILE_LEN + 1];
static QUEUE 		mQueue;
static int 			b_timer_started = 0;



//加载 MRP
jint vm_loadMrp(JNIEnv * env, jobject self, jstring path)
{
	const char *str = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
	if(str){
		LOGD("vm_loadMrp entry:%s", str);
		UTF8ToGBString(str, runMrpPath, sizeof(runMrpPath));

		gEmuEnv.b_nativeThread = 0;
//		if(!gEmuEnv.b_tsfInited)
//			gEmuEnv.androidDrawChar = 1;
		showApiLog = 1;
		gEmuEnv.showMrPrintf = 1;

		gMainJniEnv = env;

		dsm_init();

		gEmuEnv.b_vm_running = 1;

#ifdef DSM_FULL
		mr_start_dsm(runMrpPath);
#else
		mr_start_dsmC("cfunction.ext", runMrpPath);
#endif

		(*env)->ReleaseStringUTFChars(env, path, str);

		return 1;
	}

	return -1;
}


//退出 mrp 线程
static void sig_handle(int signo)
{
	if (signo == SIGKILL)
	{
		LOGI("thread_exit from SIGKILL");
	}
}


//暂停MRP
void vm_pause()
{
	if(gEmuEnv.showFW) LOGI("mr_pauseApp");

	mr_pauseApp();
}

//恢复MRP
void vm_resume()
{
	if(gEmuEnv.showFW) LOGI("mr_resumeApp");

	mr_resumeApp();
}



//退出MRP
void vm_stop()
{
	if(gEmuEnv.showFW) LOGI("mr_stop");

	LOGD("vm_exit() called by user!");

	//仅仅是通知调用 mrc_exit()
	mr_stop();
	//最后执行
	mr_exit();
}

void vm_timeOut(JNIEnv * env, jobject self)
{
	if(gEmuEnv.showTimerLog) LOGI("timeOut");

	mr_timer();
}

void vm_event(JNIEnv * env, jobject self, jint code, jint p0, jint p1)
{
	if(gEmuEnv.showFW) LOGI("mr_event(%d, %d, %d)", code, p0, p1);

	if(code == MR_SMS_GET_SC){ //获取短信中心
		p0 = (jint)dsmSmsCenter; //如果 java 层实现了，应该从java层读取信息
		p1 = 0;
	}

	mr_event(code, p0, p1);
}

/**
 * 短信到达通知
 *
 * 2013-3-26 14:51:56
 */
jint vm_smsIndiaction(JNIEnv * env, jobject self, jstring content, jstring number)
{
	int32 ret = MR_IGNORE;
	const char *numStr, *contentStr;

	if(showApiLog) LOGD("vm_smsIndiaction");

	numStr = (*env)->GetStringUTFChars(env, number, JNI_FALSE);
	if (numStr) {
		uint8 buf[64];

		UTF8ToGBString((uint8 *)numStr, buf, sizeof(buf));

		contentStr = (*env)->GetStringUTFChars(env, content, JNI_FALSE);
		if(contentStr){
			uint8 buf2[1024];

			UTF8ToGBString((uint8 *)contentStr, buf2, sizeof(buf2));

			ret = mr_smsIndiaction(buf2, strlen(buf2), buf, MR_ENCODE_ASCII);

			(*env)->ReleaseStringUTFChars(env, content, contentStr);
		}

		(*env)->ReleaseStringUTFChars(env, number, numStr);
	}

	return ret;
}

jint vm_newSIMInd(JNIEnv * env, jobject self,
		jint type, jbyteArray old_IMSI)
{

	return MR_SUCCESS;
}

jint vm_registerAPP(JNIEnv * env, jobject self,
		jbyteArray jba, jint len, jint index)
{
	if(!jba || len <= 0)
		return MR_FAILED;

	jbyte* buf = malloc(len);
	(*env)->GetByteArrayRegion(env, jba, 0, len, buf);

	return mr_registerAPP((uint8 *)buf, (int32)len, (int32)index);
}

extern int32 mr_getHostByName_block(const char *ptr);

int vm_handle_emu_msg(int what, int p0, int p1)
{
	switch(what)
	{
	case EMU_MSG_GET_HSOT:
	{
		vm_sendMsg(VMMSG_ID_CALLBACK,
				mr_getHostByName_block((const char *)p0),
				p1, 0, NULL);
		break;
	}

	default:
		return 0;
	}

	return 1;
}
