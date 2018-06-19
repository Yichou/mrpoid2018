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



include $(CLEAR_VARS)

LOCAL_MODULE 	:= mr_vm_full

#LOCAL_ARM_MODE  := arm

LOCAL_CFLAGS := -c -littleend -O2 -DTARGET_MOD -DMTK_MOD -DMR_VERSION=$(VM_VERSION) -DMR_PLAT_DRAWTEXT -DMR_PLAT_U2C

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
					$(LOCAL_PATH)/src \
					$(LOCAL_PATH)/tomr/include

LOCAL_SRC_FILES := src/mr_api.c \
                  src/mr_debug.c  \
                  src/mr_do.c     \
                  src/mr_dump.c   \
                  src/mr_func.c     \
                  src/mr_gc.c       \
                  src/mr_mem.c      \
                  src/mr_opcodes.c  \
                  src/mr_object.c  \
                  src/mr_state.c    \
                  src/mr_string.c   \
                  src/mr_table.c    \
                  src/mr_tm.c       \
                  src/mr_undump.c   \
                  src/mr_vm.c       \
                  src/mr_zio.c      \
                  src/mr_noparser.c  
#                 src/lparser.c   \
#                 src/mr_cect.c   \
#                 src/llex.c      \
#                 src/lcode.c   \

LOCAL_SRC_FILES += src/lib/mr_auxlib.c \
                  src/lib/mr_auxiliar.c\
                  src/lib/mr_baselib.c\
                  src/lib/mr_tablib.c   \
                  src/lib/mr_strlib.c     \
                  src/lib/mr_socket_target.c     \
                  src/lib/mr_tcp_target.c     \
                  src/lib/mr_iolib_target.c    
#                  src/loadlib.c     \
#                  src/ldblib.c        \

LOCAL_SRC_FILES += mythroad.c \
                    mr_gb2312.c     \
                    mr_pluto.c	\
                    mr_unzip.c	\
                    mr_util.c	\
                    mr_encode.c	\
		    		mr_graphics.c \
                    mr_inflate.c	
#                    src/mr_maketo.c	
#                    src/MySprite.c   \
#                    src/pluto.c	\
#                    src/Mylib.c

LOCAL_SRC_FILES += tomr/src/tomr_event.c \
                tomr/src/tomr_is.c   \
                tomr/src/tomr_map.c        \
                tomr/src/tomr_push.c     \
                tomr/src/tomr_to.c

include $(BUILD_STATIC_LIBRARY)