#===============================================================================
# dsp environment variables
#===============================================================================
XTENSA_VERSION := RG-2017.5-linux
XTENSA_CORE := H_3Z_ides

export XTENSA_HOME := $(PROJECT_ROOT)/tools/bin/compiler/xtensa/$(XTENSA_VERSION)
# export XTENSAD_LICENSE_FILE=

#===============================================================================
XTENSA_TOOLS := $(XTENSA_HOME)/install/tools/$(XTENSA_VERSION)/XtensaTools
XTENSA_SYS := $(XTENSA_HOME)/install/builds/$(XTENSA_VERSION)/$(XTENSA_CORE)/config
XTENSA_MAKE := $(XTENSA_TOOLS)/bin/xt-make --xtensa-core=$(XTENSA_CORE) --xtensa-system=$(XTENSA_SYS)
XTOS_INCLUE := $(XTENSA_TOOLS)/xtensa-elf/src/xtos
TOOLPATH := $(XTENSA_TOOLS)/bin

#===============================================================================
# elf scripts
XT_ELF_BIN := $(IDP_BUILD_DIR)/tool/clipped
XT_ELF_CLIP := $(IDP_BUILD_DIR)/tool/elf_clip.sh
XT_ELF_PACK := $(IDP_BUILD_DIR)/tool/elf_pack.sh
XT_ELF_DUMP := $(IDP_BUILD_DIR)/tool/elf_to_dat.sh

#===============================================================================
# hifi_rules.mak
BUILD_RULES := $(IDP_BUILD_DIR)/hifi_rules.mak
