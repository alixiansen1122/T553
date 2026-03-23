# GNSS Makefile
ifeq ($(CONFIG_DEV_GNSS), y)
tiot-gnss-objs += gnss_device_info.o gnss_service.o

ifeq ($(CONFIG_FILE_BY_ARRAY), y)
ifneq (,$(wildcard $(TIOT_DRIVER_PATH)/device/gnss/firmware/gnss_fw_file_array.c))
tiot-gnss-objs += firmware/gnss_fw_file_array.o
TIOT_HEADER_LIST += -I$(TIOT_DRIVER_PATH)/device/gnss/firmware
endif
endif

tiot-gnss-objs := $(addprefix device/gnss/,$(tiot-gnss-objs))
tiot_driver-objs += $(tiot-gnss-objs)

TIOT_HEADER_LIST += -I$(TIOT_DRIVER_PATH)/device/gnss
endif
