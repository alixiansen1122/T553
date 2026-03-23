set(COMPONENT_NAME "xiaodu_navi_vector")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ohos_deps
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits/gfx_utils
    ${ROOT_DIR}/include/middleware/services/bts/br
    ${ROOT_DIR}/include/middleware/services/bts/common
    ${ROOT_DIR}/open_source/openharmony/foundation/ace/ace_engine_lite/modules/baidumapcomponent
    ${ROOT_DIR}/application/wearable/service/xiaodu_navi_vector/libs
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/xiaodu_navi_vector_msg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/xiaodu_navi_vector.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-writable-strings
)
set(WHOLE_LINK
    true
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    APP_PLATFORM_WATCHGT
    CONFIG_ACELITE_ENABLE
)

build_component()


set(COMPONENT_NAME "baidumapsdk")
set(LIBS ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib${COMPONENT_NAME}.a)
set(PUBLIC_DEFINES
    SUPPORT_BAIDU
)

set(WHOLE_LINK
    true
)
build_component()