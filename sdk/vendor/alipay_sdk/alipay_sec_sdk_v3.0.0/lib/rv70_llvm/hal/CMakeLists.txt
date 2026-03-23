set(COMPONENT_NAME "alipay_hal")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
${CMAKE_CURRENT_SOURCE_DIR}/os/inc
${CMAKE_CURRENT_SOURCE_DIR}/../share/common/inc
)

set(SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/os/sample/vendor_os.c


)

set(COMPONENT_PUBLIC_CCFLAGS

)

set(COMPONENT_CCFLAGS
-Wno-pointer-arith
-Wno-strict-prototypes
-Wno-implicit-function-declaration
-Wno-unused-function
-Wno-int-conversion
-Wno-unused-parameter
-Wno-sign-compare
-Wno-builtin-macro-redefined
-Wno-error

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
__FILE__=""
__TIME__=""
SUPPORT_ALIPAY_RESERVE
)

build_component()

