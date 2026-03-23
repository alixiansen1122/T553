/*
 * Copyright (c) CompanyNameMagicTag. 2019-2020. All rights reserved.
 * Description: implementation for mbox, ticks and memory adaptor
 * Author: none
 * Create: 2020
 */

#include "main.h"
#include "arch/sys_arch.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/tcpip.h"
#include "lwip/timeouts.h"
#include "lwip/ethip6.h"
#include "lwip/ethernetif.h"
#include "netif/etharp.h"
#include "netif/ethernet.h"
#include <string.h>

#define CONAME0 'h'
#define CONAME1 's'

#define ETHARP_FLAG_TRY_HARD  1

#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 1
#define IP_ADDR3 121

struct ethernetif {
	struct eth_addr *ethaddr;
};

sys_sem_t tx_sem;
sys_mbox_t eth_tx_mb = NULL;

int g_rev_pkt_cnt = 0;

static void arp_timer(void *arg);

void ethernetif_input(struct pbuf *p, struct netif *netif);

char eth_frame[] = {
    0x00, 0x00,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55,               // dst_mac
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25,               // src_mac
    0x08, 0x00,                                       // type
    0x45, 0x00, 0x00, 0x2c,
    0x00, 0x42, 0x00, 0x00, 0x40, 0x11, 0xf6, 0x3b,
    0xc0, 0xa8, 0x01, 0x79,                           // src_addr
    0xc0, 0xa8, 0x01, 0x7a,                           // dst_addr
    0x13, 0x8a,                                       // src_port
    0x13, 0x89,                                       // dst_port
    0x00, 0x18, 0x98, 0x23,
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff    // data
};

static void low_level_init(struct netif *netif)
{
    netif->flags |= NETIF_FLAG_LINK_UP;
#if LWIP_ARP || LWIP_ETHERNET

    netif->hwaddr_len = ETH_HWADDR_LEN;

    netif->hwaddr[0] =  0x00;
    netif->hwaddr[1] =  0x11;
    netif->hwaddr[2] =  0x22;
    netif->hwaddr[3] =  0x33;
    netif->hwaddr[4] =  0x44;
    netif->hwaddr[5] =  0x55;

    netif->mtu = NETIF_MTU;

    #if LWIP_ARP
      netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    #else
      netif->flags |= NETIF_FLAG_BROADCAST;
    #endif

    if(sys_sem_new(&tx_sem , 0) == ERR_OK) {
        printf("sys_sem_new ok\n");
    }

    if(sys_mbox_new(&eth_tx_mb , 50) == ERR_OK) {
        printf("sys_mbox_new ok\n");
    }
#endif
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    err_t errval = ERR_OK;
    struct pbuf *q_send = NULL;
    q_send = pbuf_alloc(PBUF_TRANSPORT, 100, PBUF_RAM);
    if (q_send == NULL) {
        printf("pbuf_alloc failed\n");
        return ERR_MEM;
    }

    q_send->if_idx = p->if_idx;
    q_send->len = 0x3c;
    q_send->tot_len = 0x3c;
    memcpy(q_send->payload, eth_frame, 60);

    if (g_rev_pkt_cnt) {
        errval = netif->input(q_send, netif);
        if (errval != ERR_OK) {
            printf("send report from hard to socket failed\n");
        }
    }
    g_rev_pkt_cnt++;

    return errval;
}

static struct pbuf * low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL;
    return p;
}

void ethernetif_output(void *pParams)
{
    void *msg;
    struct pbuf *p;
    struct netif *netif;
    netif = (struct netif*) pParams;
    while(1)
    {
      if(sys_arch_mbox_fetch(&eth_tx_mb,&msg,0)==ERR_OK)
      {
          printf("sys_arch_mbox_fetch ok\n");
          p = (struct pbuf *)msg;

          if(p!=NULL) {
              if(low_level_output(netif,p) == ERR_OK)
              {
                  printf("low_level_output ok\n");
              }
          }
      }
    }

}

void ethernetif_input(struct pbuf *p, struct netif *netif) {
    unused(p);
    unused(netif);
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
}


#if !LWIP_ARP
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, const ip4_addr_t *ipaddr)
{
    err_t errval;
    errval = ERR_OK;

    return errval;
}
#endif /* LWIP_ARP */

err_t ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    ethernetif = mem_malloc(sizeof(struct ethernetif));

    if (ethernetif == NULL) {
      printf("ethernetif_init: out of memory\n");
      return ERR_MEM;
    }

    LWIP_ASSERT("netif != NULL", (netif != NULL));
#if LWIP_NETIF_HOSTNAME
    netif->hostname = "lwip";
#endif
    netif->state = ethernetif;
    netif->name[0] = CONAME0;
    netif->name[1] = CONAME1;

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
    netif->output = etharp_output;
#else
    netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);
    ethernetif->ethaddr = (struct eth_addr *) &(netif->hwaddr[0]);
    return ERR_OK;
}

static void arp_timer(void *arg)
{
    etharp_tmr();
    sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}
