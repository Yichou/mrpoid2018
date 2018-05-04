/*
 * Jni.cpp
 *
 *  Created on: 2013年9月6日
 *      Author: Yichou
 */


#include <lib/JniUtils.h>
#include <mr_types.h>
#include <malloc.h>
#include <string.h>


//namespace mrpoid {

#ifndef LOG_UTILS

#include <android/log.h>

#define LOG_UTILS
#define LOG_TAG "Mrpoid"

#define LOGI(...) \
	((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) \
	((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) \
	((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#define LOGD(...) \
	((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

#endif


void jniThrowException(JNIEnv *env, const char *throwClass, const char *msg)
{
	jclass clazz = (*env)->FindClass(env, throwClass);

	if ((*env)->ExceptionCheck(env)) {
		/* TODO: consider creating the new exception with this as "cause" */
//		char buf[256];

		jthrowable excep = (*env)->ExceptionOccurred(env);
		(*env)->ExceptionClear(env);
//		getExceptionSummary(env, excep, buf, sizeof(buf));
//		LOGW("Discarding pending exception (%s) to throw %s\n", buf, className);
	}


	if(clazz == NULL) {
		LOGE("jniThrowException: can't findClass %s", throwClass);
		return;
	}

	LOGE("%s", msg);

	if ((*env)->ThrowNew(env, clazz, msg) != JNI_OK) {
		LOGE("Failed throwing '%s' '%s'\n", throwClass, msg);
		/* an exception, most likely OOM, will now be pending */
		return ;
	}
}

const char * jniJstr2cstrBuf(JNIEnv *env, jstring js, char *buf)
{
	if(js == NULL ||buf == NULL)
		return (char *)NULL;

	jsize l = (*env)->GetStringUTFLength(env, js);
	(*env)->GetStringUTFRegion(env, js, 0, l, buf);
	buf[l] = '\0';

	return buf;
}

const char *jniJstr2cstr(JNIEnv *env, jstring js)
{
	char *buf = (char *)NULL;

	if (js != NULL)
	{
		jsize l = (*env)->GetStringUTFLength(env, js);

		if (l > 0)
		{
			buf = (char *)malloc(l + 1);
			(*env)->GetStringUTFRegion(env, js, 0, l, buf);
			buf[l] = '\0';
		}
	}

	return buf;
}



//}
