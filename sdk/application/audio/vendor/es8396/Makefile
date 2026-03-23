LOCAL_TARGET := es8396

#=========================== variables ==================================
CUR_DIR = $(shell pwd)

SAP_TOP_DIR = $(realpath $(CUR_DIR)/../../..)
DRV_DIR = $(CUR_DIR)/../..

include $(SAP_TOP_DIR)/sap_base.mak

PARAM_FILE:=$(SAP_TOP_DIR)/build/Makefile.param
include $(PARAM_FILE)

OBJOUT := $(OBJS_DIR)$(dir $(subst $(realpath $(SAP_SDK_DIR)),,$(CUR_DIR)))$(LOCAL_TARGET)

LOCAL_CFLAGS += $(SAP_FEATURE_CONFIGS)
#======================== common CFLAGS =========================================
LOCAL_CFLAGS += \
	-I$(CUR_DIR) \
	-I$(SAP_CORE_DIR)/common/include \
	-I$(SAP_SDK_DIR)/include/native \
	-I$(SAP_SDK_DIR)/device/soc/vendor/soc_driver/include \
	-I$(SAP_SDK_DIR)/include/native/osa \
	-I$(SAP_SDK_DIR)/utils/osal/include

#======================== SRCS ====================================
LOCAL_SRCS := $(wildcard ./*.c)

#===================== os diff ===========================================
include $(DRV_DIR)/$(CFG_SAP_CPU_OS).mak
