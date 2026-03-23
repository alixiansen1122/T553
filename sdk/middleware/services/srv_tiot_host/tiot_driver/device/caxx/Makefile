# CAXX Makefile
ifeq ($(CONFIG_DEV_CAXX), y)
tiot-caxx-objs += caxx_device_info.o caxx_service.o

ifeq ($(CONFIG_FILE_BY_ARRAY), y)
ifneq (,$(wildcard $(TIOT_DRIVER_PATH)/device/caxx/firmware/caxx_fw_file_array.c))
tiot-caxx-objs += firmware/caxx_fw_file_array.o
TIOT_HEADER_LIST += -I$(TIOT_DRIVER_PATH)/device/caxx/firmware
endif
endif

tiot-caxx-objs := $(addprefix device/caxx/,$(tiot-caxx-objs))
tiot_driver-objs += $(tiot-caxx-objs)

TIOT_HEADER_LIST += -I$(TIOT_DRIVER_PATH)/device/caxx
endif