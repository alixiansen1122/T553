#============================== INCLUDES PATH ==================================
INCLUDES += \
	-I$(IDP_TOP_DIR)/overlay
#================================ LSP PATH =====================================
LSP_HIFI := $(IDP_OUT_DIR)/$(XTENSA_LSP_FILE)
LSPSRCFILE :=

ifndef BUILD_LIB
CFLAGS_EXT += -Werror
SRCFILE += core_overlay.c
vpath %.c $(IDP_TOP_DIR)/overlay
endif
#===============================================================================
include $(IDP_BUILD_DIR)/hifi_rules.mak
#===============================================================================
