LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := mythroad
#LOCAL_ARM_MODE  := arm

LOCAL_SRC_FILES := mythroad.c \
	mr_draw.c \
	mr_file.c \
	mr_mem.c \
	mr_smscfg.c \
	mr_utils.c

include $(BUILD_STATIC_LIBRARY)