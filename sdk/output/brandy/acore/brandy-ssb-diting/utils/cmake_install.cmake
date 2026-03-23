# Install script for directory: /work/shishimao/T553_code/software/code/sdk/middleware/utils

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
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/algorithm/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/app_version/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/at/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/bootloader/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/build_version/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/codeloader/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/codeloader_ssb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/common_headers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/connectivity/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/cpu_load/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/dfx/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/error_code/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/fs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/lib_utils/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/nv/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/partition/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/sec_boot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/work/shishimao/T553_code/software/code/sdk/output/brandy/acore/brandy-ssb-diting/utils/update/cmake_install.cmake")
endif()

