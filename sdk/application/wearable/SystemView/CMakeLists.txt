#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "systemview")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SEGGER/SEGGER_RTT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SEGGER/SEGGER_SYSVIEW.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Sample/LiteOS/SEGGER_SYSVIEW_LiteOS.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Sample/LiteOS/Config/SEGGER_SYSVIEW_Config_LiteOS.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/SEGGER
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Sample/LiteOS
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
	CONFIG_SYSTEM_VIEW
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
-Wno-pointer-arith
-Wno-strict-prototypes
-Wno-implicit-function-declaration
-Wno-unused-function
-Wno-int-conversion
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()