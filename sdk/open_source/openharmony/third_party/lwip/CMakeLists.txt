cmake_minimum_required(VERSION 3.7)


#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
if("BRANDY_PRODUCT_EVB4" IN_LIST DEFINES OR "BRANDY_PRODUCT_EVB" IN_LIST DEFINES)
set(COMPONENT_NAME "lwip")
else()
set(COMPONENT_NAME "ohos_lwip")
endif()

set(CMAKE_LWIP_SOURCE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip_v2.1.3)
set(LWIP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lwip_v2.1.3)

if("__LITEOS__" IN_LIST DEFINES)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lwip_adapter/liteos)
elseif("freertos" IN_LIST TARGET_COMPONENT)
    set(CMAKE_LWIP_ADAPTER_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/lwip_adapter/freertos)     
endif()

set(SOURCES
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/tcp_out.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/inet_chksum.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ip.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/udp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/etharp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/ip4.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/ip4_addr.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/ip4_frag.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/autoip.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/igmp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/icmp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv4/dhcp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/pbuf.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/lowpower.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/timeouts.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/tcp_in.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/altcp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/altcp_tcp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/def.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/dns.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/stats.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/inet6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/mld6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/icmp6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/dhcp6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/ip6_frag.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/ip6_addr.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/ethip6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/nd6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/ipv6/ip6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/init.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/sys.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/altcp_alloc.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/netif/lowpan6_common.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/netif/lowpan6.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/if_api.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/netifapi.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/sockets.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/err.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/mem.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/memp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/netif.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/netif/ethernet.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/raw.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/core/tcp.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/netbuf.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/tcpip.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/netdb.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/api_msg.c
    ${CMAKE_LWIP_SOURCE_DIR}/src/api/api_lib.c
    ${CMAKE_LWIP_ADAPTER_DIR}/src/arch/sys_arch.c
    ${CMAKE_LWIP_ADAPTER_DIR}/src/arch/ethernetif.c
)

if("__LITEOS__" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${ROOT_DIR}/open_source/mbedtls/mbedtls_v3.6.0/library/net_sockets.c
        ${CMAKE_LWIP_ADAPTER_DIR}/src/private/lwip_netif_manage.c
        ${CMAKE_LWIP_SOURCE_DIR}/src/api/nettool/ping.c
        ${CMAKE_LWIP_ADAPTER_DIR}/src/private/lwip_lowpower.c
    )
endif()

set(PUBLIC_HEADER
    ${CMAKE_LWIP_SOURCE_DIR}/src/include
    ${CMAKE_LWIP_ADAPTER_DIR}/src/include
    ${CMAKE_LWIP_ADAPTER_DIR}/src/include/public
)

set(PRIVATE_HEADER
    ${CMAKE_LWIP_SOURCE_DIR}/src/include
    ${CMAKE_LWIP_SOURCE_DIR}/src/include/lwip/nettool
    ${CMAKE_LWIP_ADAPTER_DIR}/src/include
    ${ROOT_DIR}/include/middleware/services/wifi
)

set(PRIVATE_DEFINES
    LOSCFG_BASE_CORE_USE_MULTI_LIST
    MBEDTLD_USE_LWIP
)

set(PUBLIC_DEFINES
    LWIP_CONFIG_FILE=\"lwip/lwipopts_default.h\"
)

if("ims" IN_LIST TARGET_COMPONENT)
    list(APPEND PUBLIC_DEFINES
        LWIP_SHARED_CONFIG_FILE=\"lwip/lwipopts_shared.h\"
    )
    list(APPEND SOURCES
        ${CMAKE_LWIP_ADAPTER_DIR}/src/private/lwip_volte_adapter.c
        ${CMAKE_LWIP_ADAPTER_DIR}/src/private/netif.c
    )
    list(APPEND PUBLIC_HEADER
        ${ROOT_DIR}/protocol/ims/include
    )
endif()

if("SUPPORT_BREDR" IN_LIST DEFINES AND "__LITEOS__" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_LWIP_ADAPTER_DIR}/src/private/lwip_bt_adapter.c
    )
endif()

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

if("BRANDY_PRODUCT_EVB4" IN_LIST DEFINES OR "BRANDY_PRODUCT_EVB" IN_LIST DEFINES)
set(COMPONENT_CCFLAGS
)
else()
set(COMPONENT_CCFLAGS
    -Wno-unused-but-set-variable
)
endif()

if("__ALIOS__" IN_LIST DEFINES)
    list(APPEND PRIVATE_HEADER
        ${ROOT_DIR}/kernel/alios/utils/libc/musl_1.2.3/include
        ${ROOT_DIR}/kernel/alios/utils/libc/adapt/include
        ${ROOT_DIR}/kernel/alios/utils/libc/adapt/musl_1.2.3/include
    )
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
