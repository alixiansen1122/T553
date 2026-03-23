CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR))

include $(IDP_TOP_DIR)/sap_idp.mak

#===============================================================================
.PHONY: all clean
#===============================================================================
all:
	make -C $(IDP_BUILD_DIR)

clean:
	make -C $(IDP_BUILD_DIR) clean
#===============================================================================
.PHONY: lib lib_clean
#===============================================================================
lib:
	make -C $(IDP_BUILD_DIR) lib

lib_clean:
	make -C $(IDP_BUILD_DIR) lib_clean
