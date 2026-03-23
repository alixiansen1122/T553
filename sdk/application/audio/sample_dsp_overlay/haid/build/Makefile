ifeq ($(SDK_TOP_DIR),)
SDK_TOP_DIR := $(realpath $(PWD)/..)
endif
export SDK_TOP_DIR

include xtensa_cfg.mak

AT ?= @
FIRMWARE_PREFIX := dsp
DSP_LIB := $(BUILD_OUT_DIR)/bin/haid.bin
#===============================================================================
depends := lsp
depends_clean := $(addsuffix _clean,$(depends))
#====================================================================================
.PHONY: all clean dsp_lib dsp_lib_clean
#===============================================================================
all: dsp_lib
clean: dsp_lib_clean
#===============================================================================
.PHONY: haid haid_clean
#===============================================================================
haid: $(depends)
	$(AT)make -C $(SDK_TOP_DIR)/component
haid_clean: $(depends_clean)
	$(AT)make -C $(SDK_TOP_DIR)/component clean
#===============================================================================
# lsp
#===============================================================================
.PHONY: $(depends) $(depends_clean)
#===============================================================================
$(depends):
	$(AT)$(XTENSA_MAKE) -f hifi_$@.mak

$(depends_clean):
	$(AT)$(XTENSA_MAKE) -f hifi_$(patsubst %_clean,%,$@).mak clean
#===============================================================================
.PHONY: dsp_lib dsp_lib_clean
#===============================================================================
dsp_lib: haid
	$(AT)mkdir -p $(SAP_FIRMWARE_DIR)
ifneq ($(DSP_LIB),)
	$(AT)cp -rf $(BUILD_OUT_DIR)/bin/haid.bin $(BUILD_OUT_DIR)/bin/$(FIRMWARE_PREFIX)_lib.bin
	$(AT)cp -rf $(BUILD_OUT_DIR)/bin/$(FIRMWARE_PREFIX)_lib.bin $(SAP_FIRMWARE_DIR)/
endif

dsp_lib_clean: haid_clean
	$(AT)rm -rf $(BUILD_OUT_DIR)/bin/$(FIRMWARE_PREFIX)_lib.bin
#===============================================================================
