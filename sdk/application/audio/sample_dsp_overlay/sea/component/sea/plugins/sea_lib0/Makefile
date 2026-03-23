CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../../../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include $(IDP_BUILD_DIR)/xtensa_cfg.mak
#===============================================================================
ifdef BUILD_LIB
TARGET := libsea_lib0

CFLAGS_EXT := 
INCLUDES := -Iinclude -I$(IDP_TOP_DIR)/include
LIB_INSTALL_DIR := lib

SRCFILE := $(wildcard src/*.c)

#===============================================================================
else
TARGET := sea_lib0

CFLAGS_EXT :=
INCLUDES := -Iinclude -I$(IDP_TOP_DIR)/include
LIBS := -l:lib/libsea_lib0.a

SRCFILE := xasea_lib0_comp.c

endif
#===============================================================================

#===============================================================================
include ../rules.mak
#===============================================================================
