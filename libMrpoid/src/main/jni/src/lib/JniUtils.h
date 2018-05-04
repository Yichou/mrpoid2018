/*
 * Jni.h
 *
 *  Created on: 2013年9月6日
 *      Author: Yichou
 */

#ifndef _LIBS_UTILS_JNI_H_
#define _LIBS_UTILS_JNI_H_

#include <jni.h>

//namespace mrpoid {

#define RUNTIME_EXCEPTION 	"java/lang/RuntimeException"
#define OUTOFMEMORY_ERROR 	"java/lang/OutOfMemoryError"
#define ILLEGALSTATE_EXCEPTION "java/lang/IllegalStateException"


void jniThrowException(JNIEnv *env, const char *throwClass, const char *msg);

const char * jniJstr2cstrBuf(JNIEnv *env, jstring js, char *buf);

const char *jniJstr2cstr(JNIEnv *env, jstring js);

//}

#endif /* JNI_H_ */
