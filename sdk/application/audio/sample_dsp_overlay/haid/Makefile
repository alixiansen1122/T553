ifeq ($(SDK_TOP_DIR),)
SDK_TOP_DIR := $(realpath $(PWD))
endif
export SDK_TOP_DIR
#===============================================================================
AT ?= @
#===============================================================================
objects := build
objects_clean := $(addsuffix _clean,$(objects))
#===============================================================================
.PHONY: all clean
#===============================================================================
all: $(objects)
clean: $(objects_clean)

dist_clean: clean
	$(AT)rm -rf firmware out build/tool/__pycache__
#===============================================================================
.PHONY: $(objects) $(objects_clean)
#===============================================================================
$(objects):
	$(AT)make -C $@

$(objects_clean):
	$(AT)make -C $(patsubst %_clean,%,$@) clean
