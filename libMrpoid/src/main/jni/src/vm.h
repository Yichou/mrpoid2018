#ifndef _VM_H
#define _VM_H

#include "emulator.h"

#define VM_NATIVE_THREAD 0

//extern int vm_is_running;


typedef enum {
	VMMSG_ID_TIMER_OUT = 1001,
	VMMSG_ID_START,
	VMMSG_ID_PAUSE,
	VMMSG_ID_RESUME,
	VMMSG_ID_EVENT,
	VMMSG_ID_STOP,
	VMMSG_ID_GETHOST,//网络异步回调
	VMMSG_ID_CALLBACK,

	VMMSG_ID_MAX
}E_VMMSG_ID;


void vm_sendMsgDelay(int what, int arg0, int arg1, int arg2, void *expand, long ms);
void vm_sendMsg(int what, int arg0, int arg1, int arg2, void *expand);
void vm_sendEmptyMsgDelay(int what, long ms);
void vm_sendEmptyMsg(int what);

void vm_timerStart(long ms);
void vm_timerStop();

int vm_handle_emu_msg(int what, int p0, int p1);


//---------------------------------------
jint vm_loadMrp(JNIEnv * env, jobject self, jstring path);
jint vm_loadMrp_thread(JNIEnv * env, jobject self, jstring path);
void vm_pause(JNIEnv * env, jobject self);
void vm_resume(JNIEnv * env, jobject self);
void vm_exit(JNIEnv * env, jobject self);
void vm_timeOut(JNIEnv * env, jobject self);
void vm_event(JNIEnv * env, jobject self, jint code, jint p0, jint p1);
int vm_smsIndiaction(JNIEnv * env, jobject self, jstring pContent, jstring pNum);

jint vm_newSIMInd(JNIEnv * env, jobject self,
		jint type, jbyteArray old_IMSI);

jint vm_registerAPP(JNIEnv * env, jobject self,
		jbyteArray p, jint len, jint index);

/*
 * 强制退出 native 启动的 thread
 */
void vm_exit_foce(JNIEnv * env, jobject self);

#endif
