# Install script for directory: /work/shishimao/T553_code/software/code/sdk/drivers/drivers/hal

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
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/adc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/cpu_core/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/cpu_trace/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/dma/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/gpio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/i2c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/ipc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/lpc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/l2ram/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/memory_monitor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/mipi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/mips/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/otp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/pinmux/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/pmp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/pwm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/qspi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/reboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/reg_config/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/sectors/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/security/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/spi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/systick/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/tcxo/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/timer/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/uart/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/watchdog/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/xip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/calendar/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/hal/rtc_unified/cmake_install.cmake")
endif()

