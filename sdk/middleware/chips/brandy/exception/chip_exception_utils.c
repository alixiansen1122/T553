/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: utils
 * Author:
 * Create:
 */
#include "chip_exception_utils.h"
#include <stdbool.h>
#include "hal_reboot.h"
#include "chip_io.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "platform_core.h"
#if defined(BUILD_APPLICATION_STANDARD) && !defined(BUILD_RECOVERY_IMAGE)
#include "diag_log.h"
#include "diag_filter.h"
#endif

#define STATUS_ADDRESS  0x5702C1E8
#define VBUS_INSERT_POS 4
#define PWREN_PRESS_POS 3
#define B_IS_WAKE_UP_REG 0x57000500
#define B_MAINTENANCE_1 0x59008500
#define B_MAINTENANCE_2 0x570007a4
#define B_MAINTENANCE_3 0x570007a0
#define B_MAINTENANCE_PC_L 0x570007a8
#define B_MAINTENANCE_PC_H 0x570007ac

bool is_first_power_on(void)
{
    return hal_reboot_get_first_power_on_flag();
}

bool is_vbus_insert(void)
{
    if (reg32_getbit(STATUS_ADDRESS, VBUS_INSERT_POS)) {
        return true;
    }
    return false;
}

bool is_pwren_press(void)
{
    if (reg32_getbit(STATUS_ADDRESS, PWREN_PRESS_POS)) {
        return true;
    }
    return false;
}

static int32_t mkdir_all_path(const char *path)
{
    int path_len = (int)strlen(path);
    if (path_len <= 0) {
        return -1;
    }

    char *str_path = (char *)malloc((uint32_t)path_len + 1);
    if (str_path == NULL) {
        return -1;
    }
    (void)memset_s(str_path, (uint32_t)path_len + 1, 0, (uint32_t)path_len + 1);
    if (strcpy_s(str_path, (uint32_t)path_len + 1, path) != 0) {
        free(str_path);
        return -1;
    }

    for (int i = 0; i < path_len; i++) {
        if (i > 0 && str_path[i] == '/') {
            str_path[i] = '\0';
            if (access(str_path, 0) == 0) {
                str_path[i] = '/';
                continue;
            }
            if (mkdir(str_path, S_IREAD | S_IWRITE) != 0) {
                free(str_path);
                return -1;
            }
            str_path[i] = '/';
        }
    }
    free(str_path);
    return 0;
}

int32_t write_fileinfo(const char *path, uint32_t offset, const uint8_t *buf, uint32_t size)
{
    int fd;
    int ret;
    ssize_t len;
    fd = open(path, O_RDWR | O_CREAT, 0);
    if (fd < 0) {
        if (mkdir_all_path(path) != 0) {
            return -1;
        }
        fd = open(path, O_RDWR | O_CREAT, 0);
        if (fd < 0) {
            return -1;
        }
    }
    int pos = (int)lseek(fd, offset, SEEK_SET);
    if (pos < 0) {
        return -1;
    }
    len = write(fd, buf, size);
    if (len < 0) {
        return -1;
    }
    ret = close(fd);
    if (ret < 0) {
        return -1;
    }
    return len;
}

void config_maintenance_b_function(void)
{
#if defined(BUILD_APPLICATION_STANDARD) && !defined(BUILD_RECOVERY_IMAGE)
    diag_highest_level_proc(DIAG_LOG_LEVEL_ERROR);
    while (readw(B_IS_WAKE_UP_REG) != 0) {
        diag_highest_level_proc(DIAG_LOG_LEVEL_ERROR);
    }
    writew(B_MAINTENANCE_1, 1);
    uint8_t before = readb(B_MAINTENANCE_2);
    before = before | 0x1;
    writeb(B_MAINTENANCE_2, before);
    printf("0x59008500 %x \r\n", readw(B_MAINTENANCE_1));
    printf("0x570007a4 %x \r\n", readw(B_MAINTENANCE_2));
    printf("0x570007a0 %x \r\n", readw(B_MAINTENANCE_3));
    printf("0x570007a8 =%x,0x570007ac =%x \r\n", readw(0x570007a8), readw(0x570007ac));
#endif
}

__attribute__((noinline)) void test_msdio(void)
{
    uint16_t value = readw(SPI_HOST_REG_FOR_HADR_DOG_TEST);  // SDIO HOST地址，访问会造成硬件狗超时
    writew(BUS_TEST_REG, value);
    writew(BUS_TEST_REG, 1);
}

// 5702c014这个寄存器是不掉电的寄存器，当芯片复位这个寄存器的值还会在的，所以用来记录程序跑到了哪里
// 通过访问各个总线下面的寄存器来判断总线时钟是否正常，然后用5702c014记录时钟访问情况，然后在芯片复位后在ssb里面打印这个值
__attribute__((noinline)) void test_uart_h0(void)
{
    uint16_t value = readw(UART1_BASE + 0x24); // uart h0 的整数分频系数配置寄存器
    writew(BUS_TEST_REG, value);
    writew(BUS_TEST_REG, 2); // test value 2
}

__attribute__((noinline)) void test_timer0(void)
{
    uint16_t value = readw(TIMER_0_BASE_ADDR); // timer0的计数值的初始值
    writew(BUS_TEST_REG, value);
    writew(BUS_TEST_REG, 3); // test value 3
}

__attribute__((noinline)) void test_mdma(void)
{
    uint16_t value = readw(DMA_BASE_ADDR); // mdma的通道0的源地址寄存器
    writew(BUS_TEST_REG, value);
    writew(BUS_TEST_REG, 4); // test value 4
}

__attribute__((noinline)) void test_i2c(void)
{
    uint16_t value = readw(I2C_BUS_0_BASE_ADDR);  // i2c0的控制寄存器，可以将基地址配置实际使用IP的基地址
    writew(BUS_TEST_REG, value);
    writew(BUS_TEST_REG, 5); //  test value 5
}

__attribute__((noinline)) void test_qspi(void)
{
    uint16_t value = readw(SPI_BUS_5_BASE_ADDR);  // qspi2的控制寄存器
    writew(BUS_TEST_REG, value);
    writew(BUS_TEST_REG, 6); // test value 6
}


__attribute__((noinline)) void test_spi0(void)
{
    uint16_t value = readw(SPI_BUS_0_BASE_ADDR);  // spi0的控制寄存器，可以将基地址配置实际使用IP的基地址
    writew(BUS_TEST_REG, value);
    writew(BUS_TEST_REG, 7); // test value 7
}

__attribute__((noinline)) void test_nandflash(void)
{
    uint16_t value = readw(NAND_QSPI_BASE_ADDR);  // qspi to nand的控制寄存器
    writew(BUS_TEST_REG, value);
    writew(BUS_TEST_REG, 8); //  test value 8
}

__attribute__((noinline)) void test_main_ahb_bus(void)
{
#if defined(BUILD_APPLICATION_STANDARD)
    test_uart_h0();
    test_timer0();
    test_mdma();
   // test_i2c(); 使用时候需要先开启时钟，时钟在clocks_config_pri.c里面配置
   // test_qspi(); 使用时候需要先开启时钟，时钟在clocks_config_pri.c里面配置
   // test_spi0(); 使用时候需要先开启时钟，时钟在clocks_config_pri.c里面配置
    test_nandflash();
#endif
}

void test_mcu_bus(void)
{
    test_main_ahb_bus();
}