LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

MY_LOCAL_PATH := $(LOCAL_PATH)

include $(MY_LOCAL_PATH)/src/mr/Android.mk
#include $(LOCAL_PATH)/src/mr_pre/Android.mk

include $(MY_LOCAL_PATH)/src/Android.mk