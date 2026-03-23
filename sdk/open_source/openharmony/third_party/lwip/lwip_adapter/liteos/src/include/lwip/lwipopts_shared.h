/*
 * Copyright (c) CompanyNameMagicTag. 2024-2025. All rights reserved.
 * Description: lwip shared opts
 */
#ifndef LWIPOPTS_SHARED_H_
#define LWIPOPTS_SHARED_H_

#define LWIP_IPV4                       1
#define LWIP_IPV6                       1

#ifdef LIBAGENT_TINY
#define LWIP_DNS                        1
#endif

#define SO_REUSE                        1

#define LWIP_NOASSERT                   1

/**
 * LWIP_IPV6_SEND_ROUTER_SOLICIT==0: DO NOT send router solicitation messages during network startup
 * (GSMA TS.34 TS.34_5.3_REQ_001)
 */
#define LWIP_IPV6_SEND_ROUTER_SOLICIT   0
/**
 * LWIP_IPV6_SEND_NEIGHBOR_SOLICIT==0: DO NOT send neighbor solicitation messages during network startup
 * (GSMA TS.34 TS.34_5.3_REQ_001)
 */
#define LWIP_IPV6_SEND_NEIGHBOR_SOLICIT 0
#define LWIP_IPV6_DUP_DETECT_ATTEMPTS   0

#define LWIP_IPV6_ND6_CUSTOMIZE         0

/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#define LWIP_RAW                        1

/**
 * LWIP_NETIF_HOSTNAME==1: use DHCP_OPTION_HOSTNAME with netif's hostname
 * field.
 */
#define LWIP_NETIF_HOSTNAME             1

/**
 * LWIP_NETIF_API==1: Support netif api (in netifapi.c)
 */
#define LWIP_NETIF_API                  1

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT            1

/**
 * PBUF_LINK_ENCAPSULATION_HLEN: the number of bytes that should be allocated
 * for an additional encapsulation header before ethernet headers (e.g. 802.11)
 * pre alloc space for AS encode, pre alloc space equels IP_NONIP_DATA_HLEN
 * Attention, this must be same with IP_NONIP_DATA_HLEN, ESM:5, ROHC:10: EMM: 12
 */
#define PBUF_LINK_ENCAPSULATION_HLEN    27


/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#define LWIP_AUTOIP                     0

/**
 * LWIP_DHCP_AUTOIP_COOP==1: Allow DHCP and AUTOIP to be both enabled on
 * the same interface at the same time.
 */
#define LWIP_DHCP_AUTOIP_COOP           0

/**
 * LWIP_DHCP_AUTOIP_COOP_TRIES: Set to the number of DHCP DISCOVER probes
 * that should be sent before falling back on AUTOIP. This can be set
 * as low as 1 to get an AutoIP address very quickly, but you should
 * be prepared to handle a changing IP address when DHCP overrides
 * AutoIP.
 */
#define LWIP_DHCP_AUTOIP_COOP_TRIES     2

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                   4

/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#if defined (LWIPERF_ENABLE)
#define LWIP_IGMP                       1
#else
#define LWIP_IGMP                       0
#endif /* LWIPERF_ENABLE */

/**
 * LWIP_NETBUF_RECVINFO==1: append destination addr and port to every netbuf.
 */
#define LWIP_NETBUF_RECVINFO            1

/**
 * TCPIP_THREAD_NAME: The name assigned to the main tcpip thread.
 */
#define TCPIP_THREAD_NAME              "TCPIP Stack"

/**
 * TCPIP_THREAD_STACKSIZE: The stack size used by the main tcpip thread.
 * The stack size value itself is platform-dependent, but is passed to
 * sys_thread_new() when the thread is created.
 */
#define TCPIP_THREAD_STACKSIZE          0x1000

/**
 * LWIP_NETIF_STATUS_CALLBACK==1: Support a callback function whenever an interface
 * changes its up/down status (i.e., due to DHCP IP acquistion)
 */
#define LWIP_NETIF_STATUS_CALLBACK      1

/**
 * LWIP_NETIF_LINK_CALLBACK==1: Support a callback function from an interface
 * whenever the link changes (i.e., link down)
 */
#define LWIP_NETIF_LINK_CALLBACK        1

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 *
 * Needs to be at least TCP_SND_QUEUELEN.
 */
#define MEMP_NUM_TCP_SEG                64

/**
 * MEM_LIBC_MALLOC==1: Use malloc/free/realloc provided by your C-library
 * instead of the lwip internal allocator. Can save code size if you
 * already use it.
 */
#define MEM_LIBC_MALLOC                 1

/**
 * MEMP_MEM_MALLOC==1: Use mem_malloc/mem_free instead of the lwip pool allocator.
 * Especially useful with MEM_LIBC_MALLOC but handle with care regarding execution
 * speed and usage from interrupts!
 */
#define MEMP_MEM_MALLOC                 1

/**
 * LWIP_COMPAT_SOCKETS==1: Enable BSD-style sockets functions names.
 * (only used if you use sockets.c)
 */
#define LWIP_COMPAT_SOCKETS             1

/**
 * LWIP_POSIX_SOCKETS_IO_NAMES==1: Enable POSIX-style sockets functions names.
 * Disable this option if you use a POSIX operating system that uses the same
 * names (read, write & close). (only used if you use sockets.c)
 */
#define LWIP_POSIX_SOCKETS_IO_NAMES     0


/**
 * LWIP_SO_RCVTIMEO==1: Enable SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO                1

/**
 * LWIP_TCP_KEEPALIVE==1: Enable TCP keepalive packets.
 */
#define LWIP_TCP_KEEPALIVE              1

/**
 * IP_SOF_BROADCAST=1: Use the SOF_BROADCAST field to enable broadcast
 * filter per pcb on udp and raw send operations. To enable broadcast filter
 * on recv operations, you also have to set IP_SOF_BROADCAST_RECV=1.
 */
#define IP_SOF_BROADCAST                1

/**
 * IP_SOF_BROADCAST_RECV (requires IP_SOF_BROADCAST=1) enable the broadcast
 * filter on recv operations.
 */
#define IP_SOF_BROADCAST_RECV           1

/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#define LWIP_STATS                      0

/**
 * LWIP_STATS_LARGE==1: Use u32_t for starts, otherwise use u16_t
 */
#define LWIP_STATS_LARGE                0

/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#define IP_FORWARD                      1

/**
 * IP_FORWARD_ALLOW_TX_ON_RX_NETIF==1: allow ip_forward() to send packets back
 * out on the netif where it was received. This should only be used for
 * wireless networks.
 * ATTENTION: When this is 1, make sure your netif driver correctly marks incoming
 * link-layer-broadcast/multicast packets as such using the corresponding pbuf flags!
 */
#define IP_FORWARD_ALLOW_TX_ON_RX_NETIF 1

/* override definitions to point to ours */
#define mem_clib_free       free
#define mem_clib_malloc     malloc
#define mem_clib_calloc     calloc

#ifndef LWIP_DHCP_BOOTP_FILE
#define LWIP_DHCP_BOOTP_FILE                    0
#endif

#if defined (ROBOT_TEST)
#define CHECKSUM_CHECK_IP               0
#endif

#define LWIP_IPV6_SCOPES            0

#define LWIP_ND6_RDNSS_MAX_DNS_SERVERS    0

#ifndef  TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN        ((8 * TCP_SND_BUF) / TCP_MSS)
#endif

/* Use SRC based routing for IPv4. */
#define LWIP_HOOK_FILENAME                      "lwip_hooks.h"
/* lwip 2.1.3 have update it in ip4_route_src */
#define LWIP_HOOK_IP4_ROUTE_SRC(src, dest)      sys_hooks_ip4_route_src((dest), (src))

#define LWIP_ND6_NUM_PREFIXES           11

/**
 * LWIP_ND6_NUM_NEIGHBORS: Number of entries in IPv6 neighbor cache
 */
#define LWIP_ND6_NUM_NEIGHBORS          2

/**
 * LWIP_ND6_NUM_DESTINATIONS: number of entries in IPv6 destination cache
 */
#define LWIP_ND6_NUM_DESTINATIONS       2

/**
 * LWIP_ND6_NUM_ROUTERS: number of entries in IPv6 default router cache
 */
#define LWIP_ND6_NUM_ROUTERS            2

/** DNS maximum host name length supported in the name table, contains the end of '\0'. */
#define DNS_MAX_NAME_LENGTH             65

/* set default retransmission timeout to 3 secs, same as open source */
#define LWIP_TCP_RTO_DEFAULT_SECS       3
/* set minimum configured keepidle time to 15 mins */
#define LWIP_TCP_MIN_IDLE_TIME_SECS     900
/* set maximum segment lifetime to 60 secs */
#define LWIP_TCP_MSL_DEFAULT_SECS       60

/* Disable support VJ header compression. */
#define VJ_SUPPORT                      0

#endif /* LWIPOPTS_SHARED_H_ */
