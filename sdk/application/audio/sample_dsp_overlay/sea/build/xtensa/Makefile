CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include xtensa_cfg.mak

AT ?= @
#===============================================================================
FILTER := $(IDP_OUT_DIR)/bin/dsp_overlay.bin

#===============================================================================
component := sea
component_clean := $(addsuffix _clean,$(component))
#===============================================================================
depends := lsp
depends_clean := $(addsuffix _clean,$(depends))
#===============================================================================
.PHONY: all clean
#===============================================================================
all: overlay
clean: overlay_clean
#===============================================================================
.PHONY: $(objects) $(objects_clean)
#===============================================================================
$(objects): $(depends)
	$(AT)$(XTENSA_MAKE) -f hifi_$@.mak all

$(objects_clean): $(depends_clean)
	$(AT)$(XTENSA_MAKE) -f hifi_$(patsubst %_clean,%,$@).mak clean
#===============================================================================
.PHONY: $(depends) $(depends_clean)
#===============================================================================
$(depends):
	$(AT)$(XTENSA_MAKE) -f hifi_$@.mak

$(depends_clean):
	$(AT)$(XTENSA_MAKE) -f hifi_$(patsubst %_clean,%,$@).mak clean
#===============================================================================
#                   lib and component lib
#===============================================================================
.PHONY: lib lib_clean
#===============================================================================
lib: $(component)

lib_clean: $(component_clean)
#===============================================================================
.PHONY: $(component) $(component_clean)
#===============================================================================
$(component):
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/$@ lib

$(component_clean):
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/$(patsubst %_clean,%,$@) lib_clean
#===============================================================================
.PHONY: sea_bin sea_bin_clean
#===============================================================================
sea_bin: $(depends)
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/sea/plugins

sea_bin_clean:
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/sea/plugins clean
#===============================================================================
.PHONY: overlay overlay_clean
#===============================================================================
overlay: sea_bin
	$(AT)$(XT_ELF_PACK) $(IDP_OUT_DIR)/bin/dsp_$@.bin $(filter-out $(FILTER), $(wildcard $(IDP_OUT_DIR)/bin/*.bin))

overlay_clean: sea_bin_clean
	$(AT)rm -rf $(IDP_OUT_DIR)/bin/dsp_$@.bin
