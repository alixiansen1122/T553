#===============================================================================
# export variable
#===============================================================================
LOCAL_DIR := $(shell pwd)
MEDIA_BASE_DIR := $(LOCAL_DIR)/../../../..
MEDIA_BUILD_DIR := $(MEDIA_BASE_DIR)/build
ifeq ($(wildcard $(MEDIA_BUILD_DIR)/.*),)
    # MEDIA_BUILD_DIR is does not exist
    MEDIA_BASE_DIR := $(LOCAL_DIR)/../../../../..
    MEDIA_BUILD_DIR := $(MEDIA_BASE_DIR)/build
endif
include $(MEDIA_BUILD_DIR)/base.mak

#===============================================================================
# component deps
#===============================================================================
MEDIA_COMPONENT_COMM_INC_DIR := $(MEDIA_COMPONENT_COMMON_DIR)/include

#===============================================================================
# local variable
#===============================================================================
#mod dir
MOD_ROOT    := $(LOCAL_DIR)/../..
OBJ_ROOT    := $(LOCAL_DIR)/obj
TRAGET_ROOT := $(MOD_ROOT)/lib
SRC_ROOT    := $(LOCAL_DIR)
HAL_FORMAT_DIR := $(LOCAL_DIR)/../..
HAL_COMMON_DIR := $(LOCAL_DIR)/../../../common

#src files
SRCS        := $(shell find $(SRC_ROOT)  -type f -name '*.c' -print)
SRCS        := $(sort $(SRCS))

#obj files
OBJS_C      := $(SRCS:$(SRC_ROOT)%.c=$(OBJ_ROOT)/%.o)

OBJS        := $(OBJS_C)
OBJS        := $(sort $(OBJS))
OBJ_DIR     := $(dir $(OBJS_C))

SRC_DEPS    := $(OBJS:%.o=%.d)

HMF_DEMUXER_PATH := $(MEDIA_COMPONENT)/fileformat/hmf_demuxer
MEDIA_INCLUDE_PATH :=
MEDIA_INCLUDE_PATH += -I$(HMF_DEMUXER_PATH)/include
MEDIA_INCLUDE_PATH += -I$(HMF_DEMUXER_PATH)/src/common/utils/include
MEDIA_INCLUDE_PATH += -I$(HMF_DEMUXER_PATH)/src/protocol_file
MEDIA_INCLUDE_PATH += -I$(MEDIA_COMPONENT_COMM_INC_DIR)
MEDIA_INCLUDE_PATH += -I$(SECUREC_INC_PATH)
MEDIA_INCLUDE_PATH += -I$(HAL_FORMAT_DIR)/include
MEDIA_INCLUDE_PATH += -I$(HAL_COMMON_DIR)/include

# create obj dir $$ target dir
CreateResult :=
dummy := $(call CreateDir, $(OBJ_ROOT))
dummy += $(call CreateDir, $(TRAGET_ROOT))
dummy += $(foreach dir, $(OBJ_DIR), CreateResult += $(call CreateDir, $(dir)))
ifneq ($(strip $(CreateResult)),)
    $(error $(CreateResult))
endif
HMF_CONFIG_MARC := -DHMF_MINI_MEDIA_ENABLE -DHMF_AAC_PARSER_ENABLE \
                   -DHMF_MP4_TO_ANNEXB_ENABLE -DHMF_MOV_DEMUXER_ENABLE \
                   -DHMF_AVC_PARSER_ENABLE -DHMF_HEVC_PARSER_ENABLE \
                   -DHMF_TS_DEMUXER_ENABLE -DHMF_TS_PMT_CRC_ENABLE -DHMF_VIDEO_MEDIA_ENABLE

LFLAGS_CUR := $(LFLAGS) $(HMF_CONFIG_MARC)

#target
TARGET_A := $(TRAGET_ROOT)/libplugin_demuxer_hmf.a
TARGET_SO := $(TRAGET_ROOT)/libplugin_demuxer_hmf.so

#cflags
ifeq ($(CFG_SUPPORT_FORMAT_DEMUXER),y)
ifeq ($(CFG_SUPPORT_DYNAMIC_LOADING),y)
ifeq ($(CFG_SUPPORT_DYNAMIC_LOADING_DEMUXER_PLUGIN),y)
CFLAGS += -DENABLE_DL_DEMUXER_PLUGIN
endif
endif
endif

DEPS_LIB += -Wl,-Bstatic -L$(HMF_DEMUXER_PATH)/lib -lhmf_demuxer -Wl,-Bdynamic

DEPS_LIB += -L$(HAL_COMMON_DIR)/lib -lmedia_hal_common -L$(SECUREC_LIB_PATH) -lsecurec -lm -lpthread -lc

#===============================================================================
# rules
#===============================================================================
COMPILE    = $(CC) $(CFLAGS) -c "$<" -o "$@" $(MEDIA_INCLUDE_PATH)
COMPILEDEP = $(CC) -MM "$<" $(CFLAGS) $(MEDIA_INCLUDE_PATH)
LINK       = $(CC) $(CFLAGS)

.PHONY: all clean install uninstall

all: $(TARGET_A) $(TARGET_SO)
	@-rm -rf $(OBJ_ROOT)

$(TARGET_A): $(OBJS)
	$(AT)$(AR) -rcv $@ $^

$(TARGET_SO): $(OBJS)
	$(AT)$(CC) $(CFLAGS) $(LFLAGS_CUR) -shared -o $@ $^ $(DEPS_LIB)
	$(STRIP) $(TARGET_SO)

$(OBJS_C): $(OBJ_ROOT)/%.o :$(SRC_ROOT)/%.c
	$(AT)$(COMPILE) $(HMF_CONFIG_MARC)
#	pushd $(LOCAL_DIR);cd $(LOCAL_DIR)/lib_src ;$(MAKE) $(MFLAGS); popd ;

$(SRC_DEPS): $(OBJ_ROOT)/%.d : $(SRC_ROOT)/%.c
	$(AT)set -e;rm -f $@;$(COMPILEDEP) > $@.$$$$; \
        sed 's,.*\.o[ :]*,$(@:%.d=%.o) $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$

#-include $(SRC_DEPS)

install: all
	$(AT)mkdir -p $(OUT_LIB_DIR)
#	$(AT)mkdir -p $(OUT_HEADER_DIR)
	$(AT)cp -af $(TARGET_A) $(OUT_LIB_DIR)/
	$(AT)cp -af $(TARGET_SO) $(OUT_LIB_DIR)/

uninstall:
	$(AT)rm -rf $(OUT_LIB_DIR)/$(notdir $(TARGET_A))
	$(AT)rm -rf $(OUT_LIB_DIR)/$(notdir $(TARGET_SO))

clean:
	$(AT)rm -rf $(SRC_DEPS)
	$(AT)rm -rf $(OBJS)
	$(AT)rm -rf $(OBJ_ROOT)
	$(AT)rm -rf $(TARGET_A)
	$(AT)rm -rf $(TARGET_SO)
