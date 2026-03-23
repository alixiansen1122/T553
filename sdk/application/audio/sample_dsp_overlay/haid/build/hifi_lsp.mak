include xtensa_cfg.mak
AT ?= @
#===============================================================================
keep_sections := .start .component
#===============================================================================
objects := lsp_haid
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
	$(AT)mkdir -p $(BUILD_OUT_DIR)
	$(AT)cp -r $(LSP_DIR)/$@ $(BUILD_OUT_DIR)
	$(AT)xt-genldscripts -b $(BUILD_OUT_DIR)/$@

	$(AT)$(foreach section, $(keep_sections), \
		sed -i "/*($(section))/a\    KEEP(*($(section)))" $(BUILD_OUT_DIR)/$@/ldscripts/*; \
	)

$(objects_clean):
	$(AT)rm -rf $(BUILD_OUT_DIR)/$(patsubst %_clean,%,$@)
