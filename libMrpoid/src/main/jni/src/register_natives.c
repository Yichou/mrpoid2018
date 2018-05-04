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
	{"native_create", "(Lcom/mrpoid/core/MrpScreen;Lcom/mrpoid/core/EmuAudio;)V", (void*) native_create},
	{"native_pause", "()V", (void*) native_pause},
	{"native_resume", "()V", (void*) native_resume},
	{"native_destroy", "()V", (void*) native_destroy},

	{"native_setIntOptions", "(Ljava/lang/String;I)V", (void*) native_setIntOptions},
	{"native_getStringOptions", "(Ljava/lang/String;)Ljava/lang/String;", (void*) native_getStringOptions},
	{"native_setStringOptions", "(Ljava/lang/String;Ljava/lang/String;)V", (void*) native_setStringOptions},
	{"native_callback", "(II)V", (void*) native_callback},
	{"native_getMemoryInfo", "()V", (void*) native_getMemoryInfo},
	{"native_handleMessage", "(III)I", (void*) native_handleMessage},

	{"vm_loadMrp", "(Ljava/lang/String;)I", (void*) vm_loadMrp},
	{"vm_loadMrp_thread", "(Ljava/lang/String;)I", (void*) vm_loadMrp_thread},
	{"vm_pause", "()V", (void*) vm_pause},
	{"vm_resume", "()V", (void*) vm_resume},
	{"vm_exit", "()V", (void*) vm_exit},
	{"vm_exit_foce", "()V", (void*) vm_exit_foce},
	{"vm_timeOut", "()V", (void*) vm_timeOut},
	{"vm_event", "(III)V", (void*) vm_event},
	{"vm_smsIndiaction", "(Ljava/lang/String;Ljava/lang/String;)I", (void*) vm_smsIndiaction},
	{"vm_newSIMInd", "(I[B)I", (void*) vm_newSIMInd},
	{"vm_registerAPP", "([BII)I", (void*) vm_registerAPP},

	{"native_getAppName", "(Ljava/lang/String;)Ljava/lang/String;", (void*)native_getAppName},
};

static const char *s_class_path_name2 = "com/mrpoid/core/MrpScreen";
static JNINativeMethod s_methods2[] = {
	{"native_reset", "(Landroid/graphics/Bitmap;Landroid/graphics/Bitmap;II)V", (void*) native_mrpScreenRest},
	{"native_unLockBitmap", "()V", (void*) native_mrpScreen_unLockBitmap},
	{"native_lockBitmap", "()V", (void*) native_mrpScreen_lockBitmap},
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

	if(!register_native_methods(env, s_class_path_name2, s_methods2, NELEM(s_methods2)))
		return JNI_FALSE;

	return JNI_TRUE;
}

jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	gs_JavaVM = vm;

	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		fprintf(stderr, "ERROR: GetEnv failed\n");
		goto bail;
	}
	assert(env != NULL);

	if (register_natives(env) < 0) {
		fprintf(stderr, "ERROR: Exif native registration failed\n");
		goto bail;
	}

	/* success -- return valid version number */
	result = JNI_VERSION_1_4;

bail:
	return result;
}

