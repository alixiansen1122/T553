ifeq ($(SDK_TOP_DIR),)
SDK_TOP_DIR := $(realpath $(PWD)/..)
endif
export SDK_TOP_DIR

include $(SDK_TOP_DIR)/build/xtensa_cfg.mak
#===============================================================================
TARGET := haid
INCLUDES := \
	-Idummy \
	-I$(SDK_TOP_DIR)/overlay \
	-I$(SDK_TOP_DIR)/include

SRCFILE := \
	haid_component.c \
	dummy/hearing_aid_dummy.c
#===============================================================================
SRCFILE += $(SDK_TOP_DIR)/overlay/core_overlay.c
#===============================================================================
include $(SDK_TOP_DIR)/build/hifi_rules.mak
