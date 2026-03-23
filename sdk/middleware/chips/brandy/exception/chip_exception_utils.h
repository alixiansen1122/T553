/**
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: utils
 * Author:
 * Create:
 */
#ifndef CHIP_EXCEPTION_UTILS_H
#define CHIP_EXCEPTION_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define BUS_TEST_REG 0x5702C014
#define SPI_HOST_REG_FOR_HADR_DOG_TEST 0x52061000
#define SPI_HOST_CLOCK_REG 0x52000124
/**
 * @brief Check if this is the first power-on since the device was battery-powered.
 * @return true if this is the first power-on, false otherwise.
 */
bool is_first_power_on(void);

/**
 * @brief Check if the VBUS (power supply) is inserted.
 * @return true if VBUS is inserted, false otherwise.
 */
bool is_vbus_insert(void);

/**
 * @brief Check if the power button (PWREN) is being pressed.
 * @return true if PWREN is pressed, false otherwise.
 */
bool is_pwren_press(void);

/**
 * @brief create a file and write info
 * @return write length
 */
int32_t write_fileinfo(const char *path, uint32_t offset, const uint8_t *buf, uint32_t size);

/**
 * @brief Configure B-core maintenance function
 * @return null
 */
void config_maintenance_b_function(void);

void test_mcu_bus(void);
#endif
