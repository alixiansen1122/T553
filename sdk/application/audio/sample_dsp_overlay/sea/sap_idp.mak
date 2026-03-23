#===================================================================================
PROJECT_ROOT := $(realpath $(IDP_TOP_DIR)/../../../../)
IDP_BUILD_DIR := $(IDP_TOP_DIR)/build/xtensa
IDP_LSP_DIR := $(IDP_BUILD_DIR)/arch/lsp
IDP_OUT_DIR := $(IDP_TOP_DIR)/out
IDP_OBJ_DIR := $(IDP_OUT_DIR)/obj

#===================================================================================
CFG_SAP_SEA_LIB0_SUPPORT=y
ifeq ($(CFG_SAP_SEA_LIB0_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_LIB0_SUPPORT
endif
