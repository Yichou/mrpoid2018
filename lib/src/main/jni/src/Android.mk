LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)


#编译精简虚拟机
LOCAL_MODULE := mrpoid

LOCAL_STATIC_LIBRARIES := mr_vm_mini
#LOCAL_STATIC_LIBRARIES := vm_mini_prebuilt
LOCAL_LDLIBS := -lm -llog -lc -lz -ljnigraphics
LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib
LOCAL_CFLAGS := -DDSM_MINI

LOCAL_SRC_FILES := emulator.c \
	dsm.c \
	network.c \
	utils.c \
	cport.c \
	register_natives.c \
	encode.c \
	font/font_sky16_2.c \
	lib/TimeUtils.c \
	lib/JniUtils.c
#	font/tsffont.c \

include $(BUILD_SHARED_LIBRARY)


#编译完整虚拟机###############
#include $(CLEAR_VARS)
#
#LOCAL_MODULE := mrpoid2
#
#LOCAL_STATIC_LIBRARIES := mr_vm_full
##LOCAL_STATIC_LIBRARIES := vm_full_prebuilt
#LOCAL_LDLIBS := -lm -llog -lc -lz -ljnigraphics
#LOCAL_C_INCLUDES := $(LOCAL_PATH)/lib
#LOCAL_CFLAGS := -DDSM_FULL
#
#LOCAL_SRC_FILES := emulator.c \
#	dsm.c \
#	network.c \
#	utils.c \
#	vm.c \
#	msgqueue.c \
#	register_natives.c \
#	encode.c \
#	lib/TimeUtils.c \
#	font/font_sky16_2.c \
#	lib/JniUtils.c
#
#include $(BUILD_SHARED_LIBRARY)
