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


pthread_t 	gvm_therad_id = 0;

static char 		runMrpPath[DSM_MAX_FILE_LEN + 1];
static int 			b_native_thread = 1;
static QUEUE 		mQueue;
static int 			b_timer_started = 0;
static int 			b_thread_running = 0;


static void vm_loop();
static void vm_thread_exit();


//加载 MRP
jint vm_loadMrp(JNIEnv * env, jobject self, jstring path)
{
	const char *str = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
	if(str){
		LOGD("vm_loadMrp entry:%s", str);
		UTF8ToGBString(str, runMrpPath, sizeof(runMrpPath));

		b_native_thread = FALSE;
		gEmulatorCfg.b_nativeThread = 0;
//		if(!gEmulatorCfg.b_tsfInited)
//			gEmulatorCfg.androidDrawChar = 1;
		showApiLog = 1;
		gApiLogSw.showMrPrintf = 1;

		gMainJniEnv = env;

		dsm_init();

		gEmulatorCfg.b_vm_running = 1;

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

//----------- native thread -------------------------------
#define MSG_DEL(pmsg) \
	if(pmsg->expand) free(pmsg->expand); free(pmsg);

#define MSG_NEW \
	(PT_MSG)malloc(sizeof(T_MSG))

inline static void delMsg(ELEMENT *e)
{
	PT_MSG msg = (PT_MSG)e;
	MSG_DEL(msg)
}

inline void vm_sendMsgDelay(int what, int arg0, int arg1, int arg2, void *expand, long ms)
{
	if(!b_thread_running)
		return;

	PT_MSG msg = MSG_NEW;

	msg->what = what;
	msg->arg0 = arg0;
	msg->arg1 = arg1;
	msg->arg2 = arg2;
	msg->expand = expand;

	enqueue(mQueue, (ELEMENT) msg, uptimems() + ms);
}

inline void vm_sendMsg(int what, int arg0, int arg1, int arg2, void *expand)
{
	vm_sendMsgDelay(what, arg0, arg1, arg2, expand, 0);
}

inline void vm_sendEmptyMsgDelay(int what, long ms)
{
	vm_sendMsgDelay(what, 0, 0, 0, NULL, ms);
}

inline void vm_sendEmptyMsg(int what)
{
	vm_sendMsgDelay(what, 0, 0, 0, NULL, 0);
}

//退出 mrp 线程
static void sig_handle(int signo)
{
	if (signo == SIGKILL)
	{
		LOGI("thread_exit from SIGKILL");
		vm_thread_exit();

//	 * when call kill, thread maby blocked!
//		so we call exit
		pthread_exit(NULL);
	}
}

static void * vm_thread_run(void *data)
{
	//捕获 SIGKILL 信号，用于强制退出
	signal(SIGKILL, sig_handle);

	//获取 jni 环境
	emu_attachJniEnv();

	vm_sendEmptyMsg(VMMSG_ID_START);

	//启动主循环
	vm_loop();

	//运行到这里说明  MRP 结束了
	vm_thread_exit();

	gvm_therad_id = 0;
}

static void vm_thread_exit()
{
	LOGD("vm_thread_exit");

	mr_stop();
	//here we can't call mr_exit
//	mr_exit();

	dsm_reset();
	emu_finish();
	clqueue(mQueue, delMsg);
	mQueue = NULL;

	//at last
	emu_detachJniEnv();

	gEmulatorCfg.b_vm_running = 0;
}

/**
 * 强制关闭 native 线程
 */
void vm_exit_foce(JNIEnv * env, jobject self)
{
	if (b_native_thread)
	{
		LOGD("native force exit call");
		int ret = pthread_kill(gvm_therad_id, SIGKILL);

		if(ret == ESRCH)
			LOGD("the specified thread did not exists or already quit\n");
		else if(ret == EINVAL)
			LOGD("signal is invalid\n");
		else
			LOGD("the specified thread is alive\n");

//		pthread_join(gvm_therad_id, NULL); //等待 native thread 结束释放资源
	}
}

jint vm_loadMrp_thread(JNIEnv * env, jobject self, jstring path)
{
	const char *str;

	str = (*env)->GetStringUTFChars(env, path, JNI_FALSE);
	if (str)
	{
		LOGD("vm_loadMrp entry:%s", str);
		UTF8ToGBString(str, runMrpPath, sizeof(runMrpPath));

		b_native_thread = 1;
		gEmulatorCfg.b_nativeThread = 1;
//		if (!gEmulatorCfg.b_tsfInited)
//			gEmulatorCfg.androidDrawChar = 1;
		showApiLog = 1;
		gApiLogSw.showMrPrintf = 1;

		gMainJniEnv = env;
		dsm_init();

		gEmulatorCfg.b_vm_running = 1;

		//创建消息处理器
		mQueue = new_queue();

		//set can sendMsg flag
		b_thread_running = 1;

		//启动线程
		int ret = pthread_create(&gvm_therad_id, NULL, (void *)vm_thread_run, "Hello");
		if(ret != 0){
			jniThrowException(env, RUNTIME_EXCEPTION, "native create pthread FAIL!");
		}

		return 1;
	}

	return -1;
}

//暂停MRP
void vm_pause(JNIEnv * env, jobject self)
{
	if(gApiLogSw.showFW) LOGI("mr_pauseApp");

	if(b_native_thread)
		vm_sendEmptyMsg(VMMSG_ID_PAUSE);
	else
		mr_pauseApp();
}

//恢复MRP
void vm_resume(JNIEnv * env, jobject self)
{
	if(gApiLogSw.showFW) LOGI("mr_resumeApp");

	if(b_native_thread)
		vm_sendEmptyMsg(VMMSG_ID_RESUME);
	else
		mr_resumeApp();
}

int32 mr_exit(void)
{
	LOGD("mr_exit() called by mythroad!");

	if(b_native_thread) {
		vm_sendEmptyMsg(VMMSG_ID_STOP);
		b_thread_running = 0;

		//maby need join
//		pthread_join(gvm_therad_id, NULL);
	} else {
		dsm_reset();
		emu_finish();
		gEmulatorCfg.b_vm_running = 0;
	}

	return MR_SUCCESS;
}

//退出MRP
void vm_exit(JNIEnv * env, jobject self)
{
	if(gApiLogSw.showFW) LOGI("mr_stop");

	LOGD("vm_exit() called by user!");

	if(b_native_thread) {
		vm_sendEmptyMsg(VMMSG_ID_STOP);
		b_thread_running = 0;

//		pthread_join(gvm_therad_id, NULL);
	} else {
		//仅仅是通知调用 mrc_exit()
		mr_stop();
		//最后执行
		mr_exit();
	}
}

void vm_timeOut(JNIEnv * env, jobject self)
{
	if(gApiLogSw.showTimerLog) LOGI("timeOut");

	if(b_native_thread)
		vm_sendEmptyMsg(VMMSG_ID_TIMER_OUT);
	else
		mr_timer();
}

void vm_event(JNIEnv * env, jobject self, jint code, jint p0, jint p1)
{
	if(gApiLogSw.showFW) LOGI("mr_event(%d, %d, %d)", code, p0, p1);

	if(code == MR_SMS_GET_SC){ //获取短信中心
		p0 = (jint)dsmSmsCenter; //如果 java 层实现了，应该从java层读取信息
		p1 = 0;
	}

	if(b_native_thread)
		vm_sendMsg(VMMSG_ID_EVENT, code, p0, p1, NULL);
	else
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

static void timer_handle(int signo)
{
	if(signo == SIGALRM) {
		vm_sendEmptyMsg(VMMSG_ID_TIMER_OUT);
	}
}

void vm_timerStart(long ms)
{
	b_timer_started = 1;
#if 0
	vm_sendEmptyMsgDelay(VMMSG_ID_TIMER_OUT, ms);
#else

	struct itimerval tick = { 0 };

	signal(SIGALRM, timer_handle);

	// 设定第一次执行发出signal所延迟的时间
	tick.it_value.tv_sec = ms / 1000;
	tick.it_value.tv_usec = ms * 1000 % 1000000;

	// ITIMER_REAL，表示以real-time方式减少timer，在timeout时会送出SIGALRM signal
	if (setitimer(ITIMER_REAL, &tick, NULL) == -1) {
		LOGE("setitimer err!");
	}
#endif
}

void vm_timerStop()
{
	b_timer_started = 0;
#if 0
#else
	struct itimerval tick = {0};
	setitimer(ITIMER_REAL, &tick, NULL);
#endif
}

static void vm_loop()
{
	LOGD("start mainLoop...");

	while(1)
	{
		T_MSG *msg = (T_MSG *)dequeue(mQueue);

//		LOGI("get msg{%d,%d,%d,%d}", msg->what, msg->arg0, msg->arg1, msg->arg2);

		switch(msg->what)
		{
		case VMMSG_ID_START:
#ifdef DSM_FULL
			mr_start_dsm(runMrpPath);
#else
			mr_start_dsmC("cfunction.ext", runMrpPath);
#endif
			break;

		case VMMSG_ID_TIMER_OUT:
			if(b_timer_started)
				mr_timer();
			break;

		case VMMSG_ID_PAUSE:
			mr_pauseApp();
			break;

		case VMMSG_ID_RESUME:
			mr_resumeApp();
			break;

		case VMMSG_ID_EVENT:
			mr_event(msg->arg0, msg->arg1, msg->arg2);
			break;

		case VMMSG_ID_GETHOST:
			((MR_GET_HOST_CB)mr_soc.callBack)(msg->arg0);
			break;

		case VMMSG_ID_CALLBACK:
			LOGD("callback addr=%p arg=%d", (void *)msg->arg1, msg->arg0);
			((MR_CALLBACK)msg->arg1)(msg->arg0);
			break;

		case VMMSG_ID_STOP:
		{
			MSG_DEL(msg);
			goto end;
		}
		}

		MSG_DEL(msg);
	}

end:
	LOGD("exit mainLoop...");
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
