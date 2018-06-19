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


void vm_timerStart(long ms);
void vm_timerStop();

int vm_handle_emu_msg(int what, int p0, int p1);


jint vm_loadMrp(JNIEnv * env, jobject self, jstring path);
void vm_timeOut(JNIEnv * env, jobject self);
jint vm_newSIMInd(JNIEnv * env, jobject self, jint type, jbyteArray old_IMSI);
jint vm_registerAPP(JNIEnv * env, jobject self, jbyteArray p, jint len, jint index);
int vm_smsIndiaction(JNIEnv * env, jobject self, jstring pContent, jstring pNum);
void vm_event(JNIEnv * env, jobject self, jint code, jint p0, jint p1);

void vm_pause();
void vm_resume();
void vm_stop();


#endif
