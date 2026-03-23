# Install script for directory: /work/shishimao/T553_code/software/code/sdk/drivers/drivers/driver

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/work/shishimao/T553_code/software/code/sdk/tools/bin/compiler/linx/linx_170/linx-llvm-binary-debug/linx-llvm-binary-release-musl/bin/riscv32/riscv32-linux-musl-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/adc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/cpu_trace/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/dma/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/i2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/ipc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/lcd/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/lpc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/memory_core/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/memory_monitor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/mipi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/mmc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/nand_flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/otp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/pinmux/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/pmp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/psram/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/pwm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/qspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/security/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/security_unified/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/security_unified/drv_hitls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/spi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/systick/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/tcxo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/touch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/uart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/ulp_aon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/watchdog/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/calendar/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/qspi_display/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/mtd/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/button/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/rtc_unified/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/efuse/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-diting-r/driver/audio/cmake_install.cmake")
endif()

