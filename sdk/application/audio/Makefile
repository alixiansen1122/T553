LOCAL_TARGET := audio_sample

CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

SAP_TOP_DIR := $(realpath $(CUR_DIR)/../../../utils/audio)

include $(SAP_TOP_DIR)/sap_base.mak

ifeq ($(PARAM_FILE),)
PARAM_FILE := $(SAP_TOP_DIR)/build/Makefile.param
include $(PARAM_FILE)
endif

OBJOUT := $(OBJS_DIR)$(dir $(subst $(realpath $(SAP_SDK_DIR)),,$(shell pwd)))$(LOCAL_TARGET)

SRCFILES := \
	sample_audio.c

ifeq ($(PLATFORM), apps)
SRCFILES += \
	sample_audio_utils.c \
	sample_ai/sample_ai_aenc.c \
	sample_ai/sample_ai.c \
	sample_ao/sample_ao.c \
	sample_decode/sample_decode.c \
	sample_encode/sample_cast_aenc_play.c \
	sample_encode/sample_encode.c \
	sample_phone/sample_phone_apps.c \
	sample_gui_aef/sample_gui_aef.c \
	sample_sea/sample_sea.c \
	sample_lpwk/sample_lpwk.c \
	sample_dsp_overlay/sea/sample_lsp_sea.c \
	tools/sample_dump.c \
	tools/sample_proc.c \
	tools/sample_data.c
endif

ifeq ($(CFG_SAP_CPU_OS), liteos)
SRCFILES += thread/athread_cmsis.c
else ifeq ($(CFG_SAP_CPU_OS), linux)
SRCFILES += thread/athread_linux.c
endif

ifeq ($(PLATFORM), prot)
SRCFILES += sample_phone/sample_phone_prot.c
endif

include sample.mak
