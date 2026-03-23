#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag. 2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ims")

if(NOT "ims" IN_LIST TARGET_COMPONENT)
    return()
endif()

if (${CHIP} STREQUAL "3322")
include_directories(${ROOT_DIR}/ohos/third_party/lwip/lwip_adapter/liteos/src/include/lwip)
include_directories(${ROOT_DIR}/ohos/third_party/lwip/lwip_v2.1.3/src/include)
include_directories(${ROOT_DIR}/ohos/third_party/lwip/lwip_adapter/liteos/src/include)
include_directories(${ROOT_DIR}/ohos/third_party/lwip/lwip_adapter/liteos/src/include/public)
else()
include_directories(${ROOT_DIR}/kernel/liteos/liteos_v207.1.0/Huawei_LiteOS/platform/libsec/include)
include_directories(${ROOT_DIR}/kernel/liteos/liteos_v207.1.0/Huawei_LiteOS/open_source/CMSIS/CMSIS/RTOS2/Include)
include_directories(${ROOT_DIR}/kernel/liteos/liteos_v207.1.0/Huawei_LiteOS/compat/cmsis)
include_directories(${ROOT_DIR}/kernel/freertos/utils/securec/src)
include_directories(${ROOT_DIR}/kernel/osal/include)
include_directories(${ROOT_DIR}/openharmony/third_party/lwip/lwip_v2.1.3/src/include)
include_directories(${ROOT_DIR}/openharmony/third_party/lwip/lwip_adapter/liteos/src/include)
include_directories(${ROOT_DIR}/openharmony/third_party/lwip/lwip_adapter/liteos/src/include/public)
include_directories(${ROOT_DIR}/include)
include_directories(${ROOT_DIR}/application/brandy/simulator/hal/time)
endif()

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/zos/zos*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/zos/os/rtosck/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/util/zcfg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/util/zchr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/util/zbase64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/util/zmd5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/util/zsha1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/util/zrandom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/util/zuuid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/utpt/*.c

    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_abnf_decode/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_abnf_encode/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_cfg/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_core_ua/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_dlg_invite/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_msg/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_tpt/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/rcs_protocol/rtp/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/rtp/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/ipsec/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/sdp/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/sdp/sdp_abnf_decode/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/sdp/sdp_abnf_encode/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/xml/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/xml/eax/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/xml/eax/ea/*.c

    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mtf/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mtf/mtf_db_ui/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mtf/mtf_msess/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mtf/mtf_msg_sip/*.c

    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mtc_login/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/msf/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mrf/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mmf/*.c

    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/media/mvc*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/media/mvc/cae/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/media/mvc/qoe/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/abnf/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/src/cms/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/src/crs/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/src/csf/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/src/cts/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rtosck/sa_latest/src/csm/*.c

    ${CMAKE_CURRENT_SOURCE_DIR}/rtosck/sys_res/src/sys_res.c

    ${CMAKE_CURRENT_SOURCE_DIR}/common/imsa/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/imsa_pip/src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/common/ril/src/*.c
)

file(GLOB_RECURSE IMS_FUZZ
    ${CMAKE_CURRENT_SOURCE_DIR}/../auto_test/fuzz_test/testcase/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../auto_test/fuzz_test/peach/*.c
)
if("SUPPORT_IMS_FUZZ_TEST" IN_LIST DEFINES)
set(SOURCES
    ${SOURCES}
    ${IMS_FUZZ}
)
else()
set(SOURCES
    ${SOURCES}
)
endif()

set(PRIVATE_HEADER
#rtosck下的csm相关
    ${CMAKE_CURRENT_SOURCE_DIR}/rtosck/sa_latest/include/csm
    ${CMAKE_CURRENT_SOURCE_DIR}/rtosck/sa_latest/include/imsa

#rtosck下的sys_res相关
    ${CMAKE_CURRENT_SOURCE_DIR}/rtosck/sys_res/include

#zos
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/zos
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/zos/os
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/zos/os/rtosck
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/zos/os/rtosck
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/zos
#utpt
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/utpt
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/utpt
#util
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/util
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/util
#sip
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_abnf_decode
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_abnf_encode
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_core_ua
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_dlg_invite
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_cfg
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_msg
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/sip/volte_sip/sip_tpt
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/sip
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/act_rtosck
#protocol

    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/dns
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/rtp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/sdp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/ipsec
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/tls
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/xcapc
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/xml
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/xml/eax
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/protocol/xml/eax/ea
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/dns
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/sdp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/xml
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/protocol/xml/eax
#mutf
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mutf
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mutf
#mtf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mtf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mtf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mme/mvc
#mtc_ut
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mtc_ut
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mtc_ut
#mtc_login
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mtc_login
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mtc_login
#msf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/msf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/msf
#mrf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mrf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mrf
#mmf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/mmf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mmf
#media
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/media/mvc
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/media/mvc/cae
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/media/mvc/qoe
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mme/mvc
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mme/mvc/cae
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mme/mvc/qoe
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mme/mvd
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/mme/mvd/mvdv
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/imsa
#fusion_call
    # ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/fusion_call/*.h
#abnf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/src/abnf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sdk_latest/include/abnf

#sc_latest
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/include/crs
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/include/csf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/include/cms
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/include/cts
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/src/crs
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/src/csf
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/src/cms
    ${CMAKE_CURRENT_SOURCE_DIR}/common/sc_latest/src/cts
#imsa_ril
if("SUPPORT_IMS_FUZZ_TEST" IN_LIST DEFINES)
    ${CMAKE_CURRENT_SOURCE_DIR}/../auto_test/fuzz_test/peach
    ${CMAKE_CURRENT_SOURCE_DIR}/../auto_test/fuzz_test/testcase
endif()

#imsa
    ${CMAKE_CURRENT_SOURCE_DIR}/common/imsa/include
    ${CMAKE_CURRENT_SOURCE_DIR}/common/imsa_pip/include
#audio
    ${ROOT_DIR}/middleware/services/media/foundation/audiomanager/frameworks/audiostream/include
    ${ROOT_DIR}/middleware/services/media/foundation/audiomanager/interfaces/kits/audiocommon
    ${ROOT_DIR}/middleware/services/media/foundation/sample/wrapper
#security
    ${ROOT_DIR}/include/driver/security_unified
    ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/include/mbedtls
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

set(PRIVATE_DEFINES
    ZOS_SUPT_UNUSED
    ZPLATFORM=ZPLATFORM_RTOSCK
    HW_VOLTE
    EXCLUDE_BFCP
    EXCLUDE_EA
    EXCLUDE_DB
    EXCLUDE_SRTP
    SUPPORT_MULTI_INSTANCE_1
    EXCLUDE_OPT
    LIBAGENT_TINY
)

set(PUBLIC_DEFINES
)

set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-strict-prototypes
	-Wno-incompatible-pointer-types-discards-qualifiers
	-Wno-pointer-to-int-cast
    -Wno-string-concatenation
)

if (${CHIP} STREQUAL "3322")
set(COMPONENT_CCFLAGS
    -Wno-sign-compare
    -Wno-pointer-compare
    -Wno-pointer-integer-compare
    -Wno-int-conversion
    -Wno-strict-prototypes
    -Wno-unused-but-set-parameter
    -Wno-unused-but-set-variable
    -Wno-string-concatenation
)
endif()

set(XFLAGS
    "-Wno-unused-parameter"
    "-Wno-address"
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${_CHIP}/libs/volte/${BOARD}_${APPLICATION})
#add for close component
set(LIB ${LIBRARY_OUTPUT_PATH})
install_sdk(${ROOT_DIR}/libs_url/${_CHIP}/cmake "ims.cmake")
install_sdk(${BIN_DIR}/${_CHIP}/libs/volte/${BOARD}_${APPLICATION} "*")

build_component()
