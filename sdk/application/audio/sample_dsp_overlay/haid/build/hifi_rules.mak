AT ?= @
LSP_HIFI := $(BUILD_OUT_DIR)/lsp_haid
#================================ TOOLS ========================================
CC      := $(TOOLPATH)/xt-xcc
CXX     := $(TOOLPATH)/xt-xc++
STRIP   := $(TOOLPATH)/xt-strip
ISS     := $(TOOLPATH)/xt-run
OBJCOPY := $(TOOLPATH)/xt-objcopy
OBJDUMP := $(TOOLPATH)/xt-objdump
LD      := $(TOOLPATH)/xt-ld
AR      := $(TOOLPATH)/xt-ar
#============================== FUNCTIONS ======================================
LOWER_TO_UPPER = $(shell echo '$1' | tr '[:lower:]' '[:upper:]')
DEF_FILE_NAME_AND_ID = $(shell echo -DFILE_NAME=\\\"$(notdir $1)\\\" -DTHIS_FILE_ID=$(patsubst %.C,%_C,$(call LOWER_TO_UPPER, $(notdir $1))))
#================================ FLAGS ========================================
CFLAGS :=
CFLAGS += -fforce-mem
CFLAGS += -fshort-enums
CFLAGS += -fsigned-char -Wall -W -g
CFLAGS += -O2 -OPT:alias=restrict
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -mno-mul16 -mno-mul32 -mno-div32 -mlongcalls -mtext-section-literals
CFLAGS += -INLINE:requested -Wsign-compare -Wstrict-prototypes -Wdeclaration-after-statement

CFLAGS += $(INCLUDES)
CPPFLAGS := $(CFLAGS)
CFLAGS += -std=gnu99
LDFLAGS := -Wl,-Map=$(BUILD_OUT_DIR)/$(TARGET).map -Wl,-gc-sections
#===============================================================================
OBJ_DIR     ?= $(BUILD_OUT_DIR)/obj/$(TARGET)
OBJ_CPP_DIR ?= $(BUILD_OUT_DIR)/obj_cpp/$(TARGET)
#===============================================================================
ELF_DIR := $(BUILD_OUT_DIR)/elf
DAT_DIR := $(BUILD_OUT_DIR)/dat
BIN_DIR := $(BUILD_OUT_DIR)/bin
LIB_DIR := $(BUILD_OUT_DIR)/lib
#===============================================================================
OBJDUMP_FLAGS := --source --all-headers --demangle --file-headers --line-numbers --wide
STRIP_SECTION := -R .comment -R .xtensa.info -R .xt.prop -R .xt.lit
#================================ BUILD ========================================
# try to remove sdk dir in source files
SRCFILE := $(subst $(SDK_TOP_DIR)/,,$(SRCFILE))
# tell gcc where to find source files
vpath %.c $(SDK_TOP_DIR)
vpath %.cpp $(SDK_TOP_DIR)
#===============================================================================
SRCFILE_C   := $(filter %.c,$(SRCFILE))
SRCFILE_CPP := $(filter %.cpp,$(SRCFILE))
OBJS_C   := $(SRCFILE_C:%.c=%.o)
OBJS_C   := $(addprefix $(OBJ_DIR)/,$(OBJS_C))
OBJS_CPP := $(SRCFILE_CPP:%.cpp=%.o)
OBJS_CPP := $(addprefix $(OBJ_CPP_DIR)/,$(OBJS_CPP))
ALL_OBJS := $(OBJS_C) $(OBJS_CPP)
#===============================================================================
$(OBJ_DIR)/%.o: %.c
	$(AT)test -d $(dir $@) || mkdir -p $(dir $@)
	$(AT)$(CC) $(CFLAGS) $(call DEF_FILE_NAME_AND_ID, $^) -o $@ -c $^
#===============================================================================
$(OBJ_CPP_DIR)/%.o: %.cpp
	$(AT)test -d $(dir $@) || mkdir -p $(dir $@)
	$(AT)$(CXX) $(CPPFLAGS) $(call DEF_FILE_NAME_AND_ID, $^) -o $@ -c $^
#===============================================================================
LSP_OBJS := $(patsubst %.S,%.o,$(LSPSRCFILE))
$(LSP_OBJS): %.o: %.S
	$(AT)$(CC) -c $(CFLAGS) -I$(XTOS_INCLUE) -mlsp=$(LSP_HIFI) -mtext-section-literals -o $@ $^
#===============================================================================
.PHONY: all clean
#===============================================================================
all: hifi_lsp elf
clean: elf_clean
#===============================================================================
.PHONY: hifi_lsp hifi_lsp_clean
#===============================================================================
hifi_lsp: $(LSP_OBJS)
hifi_lsp_clean:
#===============================================================================
.PHONY: elf_build elf_install elf_bin
#===============================================================================
elf_build: hifi_lsp $(ALL_OBJS)
	$(AT)$(CC) -o $(BUILD_OUT_DIR)/$(TARGET) $(ALL_OBJS) -mlsp=$(LSP_HIFI) $(LDFLAGS)
	$(AT)$(OBJDUMP) -d $(BUILD_OUT_DIR)/$(TARGET) > $(BUILD_OUT_DIR)/$(TARGET).asm
	$(AT)$(OBJDUMP) $(OBJDUMP_FLAGS) $(BUILD_OUT_DIR)/$(TARGET) > $(BUILD_OUT_DIR)/$(TARGET).lst
	$(AT)$(STRIP) -s $(STRIP_SECTION) $(BUILD_OUT_DIR)/$(TARGET)
	$(AT)$(STRIP) --strip-unneeded $(BUILD_OUT_DIR)/$(TARGET)

elf_install: elf_build
	$(AT)test -d $(ELF_DIR) || mkdir -p $(ELF_DIR)
	$(AT)cp -rf $(BUILD_OUT_DIR)/$(TARGET) $(ELF_DIR)

elf_bin: elf_build
	$(AT)test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
ifeq ($(TARGET), haid)
	$(AT)$(XT_ELF_OBJCOPY) $(OBJCOPY) $(XTENSA_CORE) $(BUILD_OUT_DIR)/$(TARGET) $(BIN_DIR)/$(TARGET).bin
else
	$(AT)$(XT_ELF_CLIP) $(BUILD_OUT_DIR)/$(TARGET) $(BIN_DIR)/$(TARGET).bin
endif
#===============================================================================
.PHONY: elf elf_clean
#===============================================================================
elf: elf_install elf_bin

elf_clean:
	$(AT)rm -rf $(LSP_OBJS) $(ALL_OBJS)
	$(AT)rm -rf $(BUILD_OUT_DIR)/$(TARGET)*
	$(AT)rm -rf $(ELF_DIR)/$(TARGET)
	$(AT)rm -rf $(BIN_DIR)/*
#===============================================================================
