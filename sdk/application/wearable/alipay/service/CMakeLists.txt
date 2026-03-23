set(COMPONENT_NAME "alipay_service")
set(MODULE_NAME "app")
set(AUTO_DEF_FILE_ID TRUE)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/app/setting/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/app/aid_code/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/app/bind_ble/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/app/pay_code/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/app/pay_result/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/app/trans_code/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/share/common/inc
    ${ALIPAY_SDK_DIR}/lib/rv70_llvm/hal/file/inc
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/alipay_feature.c
    ${CMAKE_CURRENT_SOURCE_DIR}/alipay_bluetooth.c
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
