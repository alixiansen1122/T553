#===============================================================================
# dsp environment variables
#===============================================================================
XTENSA_VERSION := RG-2017.5-linux
XTENSA_CORE := H_3Z_ides

export XTENSA_HOME=/opt/xtensa/XtDevTools
# export XTENSAD_LICENSE_FILE=
#===============================================================================
XTENSA_TOOLS := $(XTENSA_HOME)/install/tools/$(XTENSA_VERSION)/XtensaTools
XTENSA_SYS := $(XTENSA_HOME)/install/builds/$(XTENSA_VERSION)/$(XTENSA_CORE)/config
XTENSA_MAKE := $(XTENSA_TOOLS)/bin/xt-make --xtensa-core=$(XTENSA_CORE) --xtensa-system=$(XTENSA_SYS)
XTOS_INCLUE := $(XTENSA_TOOLS)/xtensa-elf/src/xtos
TOOLPATH := $(XTENSA_TOOLS)/bin
#===============================================================================
# Define the object output directory
SAP_FIRMWARE_DIR ?= $(SDK_TOP_DIR)/firmware
BUILD_OUT_DIR ?= $(SDK_TOP_DIR)/out
LSP_DIR ?= $(SDK_TOP_DIR)/build/arch/socmn1
#===============================================================================
# elf scripts
XT_ELF_CLIP := $(SDK_TOP_DIR)/build/tool/elf_clip.sh
XT_ELF_PACK := $(SDK_TOP_DIR)/build/tool/elf_pack.sh
XT_ELF_OBJCOPY := $(SDK_TOP_DIR)/build/tool/elf_objcopy.sh
#===============================================================================
# hifi_rules.mak
BUILD_RULES := $(SDK_TOP_DIR)/build/hifi_rules.mak
