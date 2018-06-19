#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <signal.h>

#include "emulator.h"
#include "vm.h"


#ifndef NELEM
#define NELEM(x) ((int)(sizeof(x) / sizeof((x)[0])))
#endif


static const char *s_class_path_name = "com/mrpoid/core/Emulator";

//#define

static JNINativeMethod s_methods[] = {
	{"native_init", "(Lcom/mrpoid/core/EmuScreen;Lcom/mrpoid/core/EmuAudio;)V", (void*) j2n_create},
	{"native_pause", "()V", (void*) j2n_pause},
	{"native_resume", "()V", (void*) j2n_resume},
	{"native_destroy", "()V", (void*) j2n_destroy},
	{"native_stop", "()V", (void*) j2n_stop},
	{"native_startMrp", "(Ljava/lang/String;)I", (void*) j2n_startMrp},
	{"native_event", "(III)V", (void*) j2n_event},
	{"native_smsRecv", "(Ljava/lang/String;Ljava/lang/String;)I", (void*) j2n_smsRecv},

	{"native_setIntOptions", "(Ljava/lang/String;I)V", (void*) j2n_setIntOptions},
	{"native_getStringOptions", "(Ljava/lang/String;)Ljava/lang/String;", (void*) j2n_getStringOptions},
	{"native_setStringOptions", "(Ljava/lang/String;Ljava/lang/String;)V", (void*) j2n_setStringOptions},
	{"native_callback", "(II)V", (void*) j2n_callback},
	{"native_getMemoryInfo", "()V", (void*) j2n_getMemoryInfo},
	{"native_handleMessage", "(III)I", (void*) j2n_handleMessage},

	{"native_screenReset", "(Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;II)V", (void*) j2n_screenRest},
	{"native_unLockBitmap", "()V", (void*) j2n_unLockBitmap},
	{"native_lockBitmap", "()V", (void*) j2n_lockBitmap},

//	{"vm_newSIMInd", "(I[B)I", (void*) vm_newSIMInd},
//	{"vm_registerAPP", "([BII)I", (void*) vm_registerAPP},

	{"native_getAppName", "(Ljava/lang/String;)Ljava/lang/String;", (void*)j2n_getAppName},
};


static int register_native_methods(JNIEnv* env,
		const char* class_name,
		JNINativeMethod* methods,
		int num_methods)
{
	jclass clazz;

	clazz = (*env)->FindClass(env, class_name);
	if (clazz == NULL) {
		fprintf(stderr, "Native registration unable to find class '%s'\n",
				class_name);
		return JNI_FALSE;
	}
	if ((*env)->RegisterNatives(env, clazz, methods, num_methods) < 0) {
		fprintf(stderr, "RegisterNatives failed for '%s'\n", class_name);
		return JNI_FALSE;
	}

	return JNI_TRUE;
}

static int register_natives(JNIEnv *env)
{
	if(!register_native_methods(env, s_class_path_name, s_methods, NELEM(s_methods)))
		return JNI_FALSE;

	return JNI_TRUE;
}

jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	gJVM = vm;

	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
		fprintf(stderr, "ERROR: GetEnv failed\n");
		goto bail;
	}
	assert(env != NULL);

	if (register_natives(env) < 0) {
		fprintf(stderr, "ERROR: Exif native registration failed\n");
		goto bail;
	}

	/* success -- return valid version number */
	result = JNI_VERSION_1_6;

bail:
	return result;
}

