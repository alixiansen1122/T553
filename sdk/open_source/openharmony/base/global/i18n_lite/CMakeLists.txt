set(COMPONENT_NAME "i18n")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/include
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/kits/i18n/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/data_resource.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/date_time_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/date_time_format.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/date_time_format_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/locale_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/measure_format.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/measure_format_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/number_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/number_format.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/number_format_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/plural_format.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/plural_format_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/plural_rules.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/str_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/frameworks/i18n/src/week_info.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
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
    -Wno-c++11-narrowing
    -Wno-error=unused-function
    -Wno-sign-compare
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
