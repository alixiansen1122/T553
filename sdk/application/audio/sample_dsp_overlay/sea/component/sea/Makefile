CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include $(IDP_BUILD_DIR)/xtensa_cfg.mak
AT ?= @
#===============================================================================
objects := plugins
#===============================================================================
objects_clean := $(addsuffix _clean,$(objects))
#===============================================================================
.PHONY: all clean lib lib_clean
#===============================================================================
all: $(objects)
clean: $(objects_clean)
lib: all
lib_clean: clean
#===============================================================================
.PHONY: $(objects) $(objects_clean)
#===============================================================================
$(objects):
	$(AT)$(XTENSA_MAKE) -C $@ lib

$(objects_clean):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_clean,%,$@) lib_clean
#===============================================================================
