/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include "lwip_hooks.h"
#include "lwip_public.h"
#include "lwip_private.h"
#include "lwip/dns.h"
#include "errcode.h"
#include "ril_interface.h"
#include "cat1_net_interface.h"
#include "modem_cmd.h"
#include "soc_osal.h"

#define RRC_RELEASE_TIME 1000
uint8_t g_ipv4_status = 0;
static struct netif *g_volte_netif = NULL;
static net_connection_callback g_volte_callback = NULL;
static sys_sem_t g_ip_fetch_sem = {0};
static uint8_t g_cat1_net_state = CONNECTION_UNKNOWN;

static char rrc_timer_started = 0;
osal_timer rrc_release_timer={0};
void rrc_release_timer_handler(void)
{
    LWIP_DEBUG_LOG(("rrc_release_timer_handler\n"));
    rrc_timer_started = 0;
    RRC_release();
}

static err_t lwip_netif_output_ip4(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
    if (netif == NULL || p == NULL || ipaddr == NULL) {
        return ERR_MEM;
    }
    return netif->linkoutput(netif, p);
}

static err_t lwip_netif_volte_linkout(struct netif *netif, struct pbuf *p)
{
    int ret;
    (void)netif;
    if (p == NULL || p->payload == NULL) {
        return ERR_ARG;
    }
    uint8_t *payload = NULL;
    struct pbuf *p_clone = NULL;
    if (p->tot_len > p->len && p->next != NULL) {
        p_clone = pbuf_clone(PBUF_RAW, PBUF_RAM, p); /* packet queue copy to one packet */
        if (p_clone == NULL) {
            return ERR_MEM;
        }
        payload = p_clone->payload;
    } else {
        payload = p->payload;
    }

    ret = cat1_network_service_write_data((uint8_t *)payload, p->tot_len);
    if (ret != ERRCODE_CAT1_SUCCESS) {
        LWIP_DEBUG_LOG(("cat1 network write fail,ret=0x%x\n", ret));
        ret = ERR_CONN;
        goto END;
    }
    ret = ERR_OK;
END:
    if (p_clone != NULL) {
        pbuf_free(p_clone);
    }
    return ret;
}

#if LWIP_IPV6
static err_t lwip_netif_output_ip6(struct netif *netif, struct pbuf *p, const ip6_addr_t *ipaddr)
{
    if (netif == NULL || p == NULL || ipaddr == NULL) {
        return ERR_MEM;
    }

    return netif->linkoutput(netif, p);
}
#endif

static err_t cat1_driver_init_fn(struct netif *netif_p)
{
    if (netif_p == NULL) {
        return ERR_MEM;
    }
    netif_state *state = (netif_state *)netif_p->state;
    if (state == NULL) {
        return ERR_ARG;
    }
#if LWIP_NETIF_HOSTNAME
    /* Initialise interface hostname */
    netif_p->hostname = state->hostname;
#endif /* LWIP_NETIF_HOSTNAME */
    netif_p->hwaddr_len = 0;
#if LWIP_IPV4
    netif_p->output = lwip_netif_output_ip4;
#endif
#if LWIP_IPV6
    netif_p->output_ip6 = lwip_netif_output_ip6;
    netif_p->mtu6 = 1500; /* 1500: max transmit unit */
#endif
    netif_p->linkoutput = lwip_netif_volte_linkout;
    netif_p->flags |= (NETIF_FLAG_BROADCAST);
    netif_p->mtu = 1500; /* 1500: max transmit unit */
    netifapi_netif_set_default(netif_p);

	rrc_release_timer.handler = rrc_release_timer_handler;
    rrc_release_timer.interval = RRC_RELEASE_TIME;
	int ret=osal_timer_init(&rrc_release_timer);
    if(ret!=0)
    {
        LWIP_DEBUG_LOG(("osal_timer_init failed\n"));
    }
    return ERR_OK;
}

void lwip_cat1_set_ip(void)
{
    struct netif *netif_p = g_volte_netif;
    if (netif_p == NULL) {
        LWIP_DEBUG_LOG(("netif is null\r\n"));
        return;
    }
    ril_ip_addr_info ip_addr_info = {0};
    sys_sem_new(&g_ip_fetch_sem, 0);

    while (true) {
        netif_p = g_volte_netif;
        if (netif_p == NULL || !netif_is_up(netif_p)) {
            goto WAITING;
        }
        if (ril_get_ue_addr(&ip_addr_info) != 0 || ip_addr_info.ip_num == 0) {
            sleep(1);
            continue;
        }

#if LWIP_IPV4 && LWIP_IPV6
        for (int i = 0; i < ip_addr_info.ip_num; i++) {
            if (ip_addr_info.ip_addr[i].type == IPADDR_TYPE_V6) {
                netif_ip6_addr_set(netif_p, 0, ip_2_ip6(&ip_addr_info.ip_addr[i]));
                LWIP_DEBUG_LOG(("Set ipv6\r\n"));
            } else if (ip_addr_info.ip_addr[i].type == IPADDR_TYPE_V4) {
                ip_addr_t gw;
                ip_addr_t netmask;
                IP_ADDR4(&netmask, 255, 255, 255, 0);
                IP_ADDR4(&gw, 255, 255, 255, 255);
                LWIP_DEBUG_LOG(("Set ipv4 OK\r\n"));
                netifapi_netif_set_addr(netif_p, ip_2_ip4(&ip_addr_info.ip_addr[i]), ip_2_ip4(&netmask), ip_2_ip4(&gw));
                g_ipv4_status = 1;
            } else {
                LWIP_DEBUG_LOG(("Set ip fail\r\n"));
            }
        }
    WAITING:
        sys_arch_sem_wait(&g_ip_fetch_sem, 0);
        LWIP_DEBUG_LOG(("lwip now check cat1 ip addr...\r\n"));
#else
        (void)netif_p;
        sys_sem_free(&g_ip_fetch_sem);
        LWIP_DEBUG_LOG(("set ip fail, lwip not support ipv4 or ipv6, check config\r\n"));
        break;
#endif
    }
}

static struct netif *lwip_netif_4g_create(char *hostname)
{
    /** The netif structure that this is looking after. */
    struct netif *netif_p = NULL;
    netif_state *state = NULL;
#if LWIP_IPV4
    ip4_addr_t ipaddr, netmask, gw;

    IP4_ADDR(&gw, 127, 0, 0, 1);
    IP4_ADDR(&ipaddr, 127, 0, 0, 1);
    IP4_ADDR(&netmask, 255, 0, 0, 0);
#endif
    state = malloc(sizeof(netif_state));
    if (state == NULL) {
        return NULL;
    }

#if LWIP_NETIF_HOSTNAME
    state->hostname = hostname;
#else
    unused(hostname);
#endif
    state->start_stack_init = true;

    netif_p = (struct netif *)malloc(sizeof(struct netif));
    if (netif_p == NULL) {
        free(state);
        return NULL;
    }
    memset_s(netif_p, sizeof(struct netif), 0, sizeof(struct netif));
    netifapi_netif_add(netif_p,
#if LWIP_IPV4
        &ipaddr,
        &netmask,
        &gw,
#endif
        (void *)state,
        cat1_driver_init_fn,
        tcpip_input);

#if LWIP_IPV6
    IP_ADDR6_HOST(netif_p->ip6_addr, 0, 0, 0, 0x00000001UL);
    netif_p->ip6_addr_state[0] = IP6_ADDR_VALID;
#endif /* LWIP_IPV6 */
    return (netif_p);
}

static void cat1_netif_set_up(struct netif *netif_p)
{
    netifapi_netif_set_link_up(netif_p);
    netifapi_netif_set_up(netif_p);
    netifapi_netif_set_default(netif_p);
}

static int cat1_netif_open(void)
{
    struct netif *netif_p = NULL;
    if (g_volte_netif != NULL) {
        cat1_netif_set_up(g_volte_netif);
        return 0;
    }
    netif_p = lwip_netif_4g_create("4G cat1");
    if (netif_p == NULL) {
        LWIP_DEBUG_LOG(("netif for 4g create fail\n"));
        return -1;
    }
    g_volte_netif = netif_p;
    return 0;
}

static void cat1_netif_state_callback(uint8_t net_state)
{
    LWIP_DEBUG_LOG(("cat1 net_state=%u\n", net_state));
    struct netif *netif_p = g_volte_netif;
    if (netif_p == NULL) {
        return;
    }

    if (net_state == CAT1_NET_STATE_ON) {
        netifapi_netif_set_link_up(netif_p);
        netifapi_netif_set_up(netif_p);
        sys_sem_signal(&g_ip_fetch_sem);
    } else {
        netifapi_netif_set_link_down(netif_p);
        netifapi_netif_set_down(netif_p);
#if LWIP_IPV4
        ip_addr_set_zero_ip4(&netif_p->ip_addr);
        ip_addr_set_zero_ip4(&netif_p->netmask);
        ip_addr_set_zero_ip4(&netif_p->gw);
#endif
    }
}

static void cat1_ethernet_data_callback(uint8_t *data, uint16_t length)
{
    struct netif *netif_p = g_volte_netif;
    struct pbuf *p = NULL;

    if (netif_p == NULL || data == NULL) {
        return;
    }
    p = pbuf_alloc(PBUF_RAW, (length + ETH_PAD_SIZE), PBUF_RAM);
    if (p == NULL) {
        return;
    }
    if (memcpy_s(p->payload, length, data, length) != EOK) {
        return;
    }
    p->len = length;
    netif_p->input(p, netif_p);
    // if(rrc_timer_started)
    // {
    //     LWIP_DEBUG_LOG(("rrc_timer_mod\n"));
    //     osal_timer_mod(&rrc_release_timer,RRC_RELEASE_TIME);
    // }
    // else
    // {
    //     LWIP_DEBUG_LOG(("rrc_timer_start\n"));
    //     osal_timer_start(&rrc_release_timer);
    //     rrc_timer_started = 1;
    // }
}

static void cat1_netif_call_state_callback(uint8_t call_state)
{
    LWIP_DEBUG_LOG(("cat1 call_state=%u\n", call_state));
    struct netif *netif_p = g_volte_netif;
    if (netif_p == NULL) {
        return;
    }
    if (call_state == CAT1_CALL_STATE_ON && cat1_network_service_net_state_get() == CAT1_NET_STATE_ON) {
        netifapi_netif_set_link_down(netif_p);
        netifapi_netif_set_down(netif_p);
#if LWIP_IPV4
        ip_addr_set_zero_ip4(&netif_p->ip_addr);
        ip_addr_set_zero_ip4(&netif_p->netmask);
        ip_addr_set_zero_ip4(&netif_p->gw);
#endif
    }
    if (call_state == CAT1_CALL_STATE_OFF && cat1_network_service_net_state_get() == CAT1_NET_STATE_ON) {
        netifapi_netif_set_link_up(netif_p);
        netifapi_netif_set_up(netif_p);
        sys_sem_signal(&g_ip_fetch_sem);
    }
}

static cat1_net_callbacks_t g_cat1_net_callback = {
    cat1_netif_state_callback, cat1_ethernet_data_callback, cat1_netif_call_state_callback};

void cat1_close_inner(void)
{
    struct netif *netif_p = g_volte_netif;
    if (netif_p == NULL) {
        return;
    }
    cat1_net_deregister_callbacks();

    netifapi_netif_set_link_down(netif_p);
    netifapi_netif_set_down(netif_p);
#if LWIP_IPV4
    ip_addr_set_zero_ip4(&netif_p->ip_addr);
    ip_addr_set_zero_ip4(&netif_p->netmask);
    ip_addr_set_zero_ip4(&netif_p->gw);
#endif
    g_cat1_net_state = CONNECTION_DISCONNECTED;
}

void cat1_open_inner(void)
{
    int ret = cat1_netif_open();
    if (ret != 0) {
        lwip_cat1_close();
        return;
    }

    /* initialize default DNS server address */
    ip_addr_t dns_server = {0};
    ip4_addr_set_u32(ip_2_ip4(&dns_server), ipaddr_addr("223.5.5.5")); /* resolver1.opendns.com */
    dns_setserver(0, &dns_server);

    ret = cat1_net_register_callbacks(&g_cat1_net_callback);
    if (ret != ERRCODE_CAT1_SUCCESS) {
        LWIP_DEBUG_LOG(("cat1 register fail, ret=0x%x\n", ret));
        lwip_cat1_close();
    } else {
        LWIP_DEBUG_LOG(("cat1_net_register_callbacks ok\n"));
    }

    if (cat1_network_service_net_state_get() == CAT1_NET_STATE_ON) {
        sys_sem_signal(&g_ip_fetch_sem);
    }
}

uint8_t cat1_get_net_state(void)
{
    return g_cat1_net_state;
}

#if LWIP_NETIF_EXT_STATUS_CALLBACK
static netif_ext_callback_t g_netif_ext_callback;

static void cat1_net_callback(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args)
{
    if (netif != g_volte_netif) {
        return;
    }

    if ((reason & LWIP_NSC_IPV4_ADDRESS_CHANGED || reason & LWIP_NSC_IPV6_SET) &&
        g_cat1_net_state != CONNECTION_CONNECTED) {
        g_cat1_net_state = CONNECTION_CONNECTED;
        if (g_volte_callback != NULL) {
            LWIP_DEBUG_LOG(("cat1 CONNECTION_CONNECTED\n"));
            g_volte_callback(LWIP_NETIF_VOLTE, CONNECTION_CONNECTED);
        }
        return;
    }

    if ((reason & LWIP_NSC_STATUS_CHANGED) && (args->status_changed.state == 0) &&
        g_cat1_net_state != CONNECTION_DISCONNECTED) {
        g_cat1_net_state = CONNECTION_DISCONNECTED;
        if (g_volte_callback != NULL) {
            LWIP_DEBUG_LOG(("cat1 CONNECTION_DISCONNECTED\n"));
            g_volte_callback(LWIP_NETIF_VOLTE, CONNECTION_DISCONNECTED);
        }
    }
}

void cat1_register_net_callback(net_connection_callback callback)
{
    if (g_volte_callback != NULL) {
        return;
    }
    g_volte_callback = callback;
    netif_add_ext_callback(&g_netif_ext_callback, cat1_net_callback);
}

void cat1_unregister_net_callback(void)
{
    g_volte_callback = NULL;
    netif_remove_ext_callback(&g_netif_ext_callback);
}
#else
void cat1_register_net_callback(net_connection_callback callback)
{
    (void)callback;
}

void cat1_unregister_net_callback(void)
{}
#endif

/* Based on ip4_route, but use src as opposed to dest. */
struct netif *sys_hooks_ip4_route_src(const ip4_addr_t *dest, const ip4_addr_t *src)
{
    (void)(dest);
    struct netif *net_if = NULL;
    if (src == NULL) {
        return NULL;
    }

    for (net_if = netif_list; net_if != NULL; net_if = net_if->next) {
        if (netif_is_link_up(net_if) && netif_is_up(net_if) && !ip4_addr_isany_val(*netif_ip4_addr(net_if))) {
            if (ip4_addr_netcmp(src, netif_ip4_addr(net_if), netif_ip4_netmask(net_if))) {
                return net_if;
            }
            if (ip4_addr_cmp(src, netif_ip4_gw(net_if)) && ((net_if->flags & NETIF_FLAG_BROADCAST) == 0)) {
                return net_if;
            }
        }
    }

    return NULL;
}
