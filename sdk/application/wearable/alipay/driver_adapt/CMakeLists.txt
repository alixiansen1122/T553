set(COMPONENT_NAME "alipay_soc_adapt")

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/hal/csi/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/hal/file/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/share/common/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/hal/network/inc
    ${ROOT_DIR}/application/wearable/service/alipay_msg
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/soc_adapt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor_product.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor_file.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor_net.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fenv_adapt.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
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
