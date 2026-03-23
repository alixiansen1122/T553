LOCAL_DIR := $(shell pwd)

MEDIA_BASE_DIR := $(LOCAL_DIR)/../../..
include $(MEDIA_BASE_DIR)/base.mak

#===============================================================================
# self deps
#===============================================================================
AUDIO_MANAGER_INC_PATH := $(LOCAL_DIR)/../../interfaces/audiomanager

AUDIO_COMMON_INC_PATH := $(LOCAL_DIR)/../../interfaces/audiocommon

AUDIO_UTILS_INC_PATH := $(LOCAL_DIR)/../audioutils/include

AUDIO_SERVICE_INC_PATH := $(LOCAL_DIR)/../audioservice/include

SRC_DEPS_INC_PATH := -I$(AUDIO_MANAGER_INC_PATH)
SRC_DEPS_INC_PATH += -I$(SECUREC_INC_PATH)\
                     -I$(AUDIO_UTILS_INC_PATH) \
                     -I$(AUDIO_COMMON_INC_PATH) \
					 -I$(AUDIO_SERVICE_INC_PATH) 


SRC_ROOT    := $(LOCAL_DIR)
OBJ_ROOT    := $(SRC_ROOT)/obj
TRAGET_ROOT := $(SRC_ROOT)/lib

SRCS := $(shell find $(SRC_ROOT) -maxdepth 2 -type f -name '*.cpp' -print)
SRCS        := $(sort $(SRCS))
OBJS  := $(SRCS:%.cpp=%.o)
OBJS        := $(sort $(OBJS))
SRC_DEPS    := $(OBJS:%.o=%.d)

# create obj dir $$ target dir
CreateResult :=
dummy := $(call CreateDir, $(OBJ_ROOT))
dummy += $(call CreateDir, $(TRAGET_ROOT))
dummy += $(foreach dir, $(OBJ_DIR), CreateResult += $(call CreateDir, $(dir)))
ifneq ($(strip $(CreateResult)),)
    $(error $(CreateResult))
endif

CFLAGS += -fdata-sections -fno-omit-frame-pointer

LDFLAGS_SO := $(LDFLAGS)

AUDIO_SERVICE_LIB_PATH := $(LOCAL_DIR)/../audioservice/lib

SRC_DEPS_LIB +=	-lsecurec -lpthread -ldl -lm -laudio_service
SRC_DEPS_LIB_PATH += -L$(AUDIO_SERVICE_LIB_PATH)\
                     -L$(SECUREC_LIB_PATH)

LIB_SO := $(TRAGET_ROOT)/libaudio_manager.so
LIB_A := $(TRAGET_ROOT)/libaudio_manager.a

.PHONY : clean all

all: $(LIB_SO) $(LIB_A)

$(LIB_A): $(OBJS)
	$(AR) -rcv $@ $^


$(LIB_SO): $(OBJS)
	$(CC) $(LDFLAGS_SO) $(CFLAGS) $(LFLAGS) -fPIC -shared -o $@ $^ $(SRC_DEPS_LIB_PATH) $(SRC_DEPS_LIB)
	$(STRIP) $(LIB_SO)


$(OBJS): %.o : %.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(SRC_DEPS_INC_PATH) -c $< -o $@

clean:
	@-rm -f $(OBJS) $(LIB_SO) $(LIB_A)
	@-rm -r $(TRAGET_ROOT) $(OBJ_ROOT)
