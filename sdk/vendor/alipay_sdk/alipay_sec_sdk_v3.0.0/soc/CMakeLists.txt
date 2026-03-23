set(COMPONENT_NAME "alipay_soc")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
${CMAKE_CURRENT_SOURCE_DIR}/se/hsec/hsc32i1
${CMAKE_CURRENT_SOURCE_DIR}/softse
${CMAKE_CURRENT_SOURCE_DIR}/include
${CMAKE_CURRENT_SOURCE_DIR}/test
${CMAKE_CURRENT_SOURCE_DIR}


)

set(SOURCES
${CMAKE_CURRENT_SOURCE_DIR}/se/hsec/hsc32i1/hs_private.c
${CMAKE_CURRENT_SOURCE_DIR}/se/hsec/vendor_crypto.c
#${CMAKE_CURRENT_SOURCE_DIR}/se/hsec/vendor_product.c
${CMAKE_CURRENT_SOURCE_DIR}/se/hsec/vendor_rot.c
${CMAKE_CURRENT_SOURCE_DIR}/se/hsec/vendor_ssp.c


${CMAKE_CURRENT_SOURCE_DIR}/softse/aes.c
${CMAKE_CURRENT_SOURCE_DIR}/softse/kdf.c
${CMAKE_CURRENT_SOURCE_DIR}/softse/sha256.c
${CMAKE_CURRENT_SOURCE_DIR}/softse/utils.c

${CMAKE_CURRENT_SOURCE_DIR}/test/yunit.c
${CMAKE_CURRENT_SOURCE_DIR}/test//test_api.c
${CMAKE_CURRENT_SOURCE_DIR}/test/test_crypto_hal.c
${CMAKE_CURRENT_SOURCE_DIR}/test/test_ssp_hal.c
${CMAKE_CURRENT_SOURCE_DIR}/test/yunit_test_main.c

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

)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(AUTO_ADD_PATH
    false
)

set(PUBLIC_DEFINES

)

set(PRIVATE_DEFINES
__FILE__=""
SUPPORT_ALIPAY_RESERVE
)

build_component()

