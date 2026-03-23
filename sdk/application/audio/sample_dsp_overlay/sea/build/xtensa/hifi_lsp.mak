CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include xtensa_cfg.mak
AT ?= @
#===============================================================================
keep_sections := .start .component
#===============================================================================
objects := lsp_sea
objects_clean := $(addsuffix _clean,$(objects))
#===============================================================================
.PHONY: all clean
#===============================================================================
all: $(objects)
clean: $(objects_clean)
#===============================================================================
.PHONY: $(objects) $(objects_clean)
#===============================================================================
$(objects):
	$(AT)mkdir -p $(IDP_OUT_DIR)
	$(AT)cp -r $(IDP_LSP_DIR)/$@ $(IDP_OUT_DIR)
	$(AT)xt-genldscripts -b $(IDP_OUT_DIR)/$@

	$(AT)$(foreach section, $(keep_sections), \
		sed -i "/*($(section))/a\    KEEP(*($(section)))" $(IDP_OUT_DIR)/$@/ldscripts/*; \
	)

$(objects_clean):
	$(AT)rm -rf $(IDP_OUT_DIR)/$(patsubst %_clean,%,$@)
#===============================================================================
