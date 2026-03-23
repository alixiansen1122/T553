CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include $(IDP_BUILD_DIR)/xtensa_cfg.mak
AT ?= @
#===============================================================================
aef_libx := aef_lib0
aef_libx_clean := $(addsuffix _clean,$(aef_libx))

# aef_libx pre-build lib filter
aef_libx_lib_filter :=
#===============================================================================
.PHONY: all clean lib lib_clean
#===============================================================================
all: $(aef_libx)
clean: $(aef_libx_clean)
#===============================================================================
.PHONY: $(aef_libx) $(aef_libx_clean)
#===============================================================================
$(aef_libx):
	$(AT)$(XTENSA_MAKE) -C $@ all XTENSA_LSP_FILE=lsp_aef

$(aef_libx_clean):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_clean,%,$@) clean XTENSA_LSP_FILE=lsp_aef
#===============================================================================
lib := $(addsuffix _lib,$(filter-out $(aef_libx_lib_filter),$(aef_libx)))
lib_clean := $(addsuffix _clean,$(lib))
#===============================================================================
.PHONY: $(lib) $(lib_clean)
#===============================================================================
$(lib):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_lib,%,$@) BUILD_LIB=y lib

$(lib_clean):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_lib_clean,%,$@) BUILD_LIB=y lib_clean
#===============================================================================
.PHONY: lib lib_clean
#===============================================================================
lib: $(lib)
lib_clean: $(lib_clean)
#===============================================================================
