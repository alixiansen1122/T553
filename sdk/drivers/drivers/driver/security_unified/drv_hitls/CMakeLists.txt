#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "drv_hitls")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/crypto_drv_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal/crypto_osal_lib.c

    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/hash/drv_hkdf_soft.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drv_code/symc/drv_symc_aead_gcm_soft.c

    # service layer
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/cipher_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/cipher_aead.c
    ${CMAKE_CURRENT_SOURCE_DIR}/service_layer/hkdf.c
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer/sl_common.c

    # HiTLS
    ${ROOT_DIR}/open_source/HiTLS/bsl/sal/src/sal_mem.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/log/src/log.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/err/src/err.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/sal/src/sal_threadlock.c
    ${ROOT_DIR}/open_source/HiTLS/bsl/err/src/avl.c

    # HiTLS HKDF
    ${ROOT_DIR}/open_source/HiTLS/crypto/hkdf/src/hkdf.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/hmac/src/hmac.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha2/src/noasm_sha256.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha2/src/sha2_256.c

    # HiTLS GCM
    ${ROOT_DIR}/open_source/HiTLS/crypto/modes/src/modes_gcm.c
    ${ROOT_DIR}/open_source/HiTLS/crypto/modes/src/noasm_ghash.c

    ${ROOT_DIR}/open_source/HiTLS/crypto/aes/src/crypt_aes.c
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include/drv_include
    ${ROOT_DIR}/include/driver/security_unified

    ${CMAKE_CURRENT_SOURCE_DIR}/crypto_osal
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${ROOT_DIR}/drivers/drivers/driver/security_unified/service_layer
    ${CMAKE_CURRENT_SOURCE_DIR}/include/common_include

    # HiTLS
    ${ROOT_DIR}/open_source/HiTLS/include/bsl
    ${ROOT_DIR}/open_source/HiTLS/include/crypto
    ${ROOT_DIR}/open_source/HiTLS/crypto/include
    ${ROOT_DIR}/open_source/HiTLS/bsl/include
    ${ROOT_DIR}/open_source/HiTLS/bsl/err/include
    ${ROOT_DIR}/open_source/HiTLS/bsl/sal/include
    ${ROOT_DIR}/open_source/HiTLS/bsl/log/include

    # HiTLS HKDF
    ${ROOT_DIR}/open_source/HiTLS/crypto/hkdf/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/hmac/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/sha2/include

    # HiTLS GCM
    ${ROOT_DIR}/open_source/HiTLS/crypto/modes/include
    ${ROOT_DIR}/open_source/HiTLS/crypto/aes/include
)

set(PUBLIC_HEADER

)

set(PRIVATE_DEFINES
    __FILENAME__=""
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)
build_component()
