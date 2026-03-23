set(COMPONENT_NAME "permission")

set(PUBLIC_HEADER
	${CMAKE_CURRENT_SOURCE_DIR}/adaptor
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/openharmony/third_party/cJSON
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/frameworks/resmgr_lite/include
    ${ROOT_DIR}/open_source/openharmony/base/global/resmgr_lite/interfaces/innerkits/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/adaptor/perm_operate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/adaptor/pms_impl.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -nostdinc
    -nostdlib
    -Wno-error=unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

build_component()
