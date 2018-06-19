LOCAL_PATH := $(call my-dir)

MINI_VM_VERSION := 2011
VM_VERSION := 1968
MINI_TARGETDIR :=


include $(CLEAR_VARS)

LOCAL_MODULE 	:= mr_vm_mini

#LOCAL_ARM_MODE  := arm

LOCAL_CFLAGS := -c -littleend -O2 -DTARGET_MOD -DMTK_MOD -DMR_VERSION=$(MINI_VM_VERSION) -DMR_PLAT_DRAWTEXT -DMR_PLAT_U2C

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES := mythroad_mini.c \
                    mr_gb2312.c     \
                    mr_unzip.c	\
                    mr_util.c	\
                    mr_encode.c	\
                    mr_inflate.c \
                    mr_graphics.c

include $(BUILD_STATIC_LIBRARY)


