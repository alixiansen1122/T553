/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "lwip_private.h"
#include "lwip/tcpip.h"
#include "lwip/etharp.h"
#include "lwip/netifapi.h"
#include "lwip/arch.h"
#include "bts_pan.h"
#include "bts_br_gap.h"

#define PAN_LOW_POWER

static struct netif *g_bt_pan_netif = NULL;
static net_connection_callback g_bt_pan_callback = NULL;
static uint8_t g_pan_net_state = CONNECTION_UNKNOWN;

#ifndef PAN_LOW_POWER
typedef struct {
    uint8_t thread_run;
    uint8_t end_thread;
    sys_sem_t pan_sem;
    void *param;
} thread_ctl;

static thread_ctl g_pan_thread = {0};
#endif

#define BTPAN_THREAD_NAME "bt_pan_listen"
#define BTPAN_THREAD_STACKSIZE 0xc00
#define BTPAN_THREAD_PRIO 30
#define BTPAN_MAX_PAIRED_DEVICES 5
#define DEVICE_MAJOR_CLASS_PHONE 0x200 /* 手机设备COD：Major Device Class = phone */

static err_t lwip_driver_output(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr);

static err_t lwip_netif_linkout(struct netif *netif, struct pbuf *p)
{
    (void)netif;
    if (p == NULL || p->payload == NULL) {
        return ERR_ARG;
    }
    unsigned char *payload = (unsigned char *)p->payload;
    if (p->len > SIZEOF_ETH_HDR) { /* delete arp packet ethernet header */
        payload += SIZEOF_ETH_HDR;
        p->len -= SIZEOF_ETH_HDR;
    }
    pan_service_write_data(payload, p->len, PAN_NETWORK_PROTOCOL_ARP, PAN_PACKET_TYPE_DEST_ONLY_UNICAST);
    return ERR_OK;
}

static err_t lwip_driver_init_fn(struct netif *netif_p)
{
    if (netif_p == NULL) {
        return ERR_ARG;
    }
    netif_p->output = lwip_driver_output;
    netif_p->linkoutput = lwip_netif_linkout;

    netifapi_netif_set_up(netif_p);
    netifapi_netif_set_link_up(netif_p);
    netifapi_netif_set_default(netif_p);
    netif_p->mtu = 1000; /* 1000: mtu */

    return ERR_OK;
}

/* 网络设备初始化 驱动初始化时调用
   参数 name：网络设备名称 2bytes
 */
static struct netif *lwip_driver_init_netif(const char *name, const unsigned char *hw_addr)
{
    struct netif *netif = (struct netif *)malloc(sizeof(struct netif));

    if (netif == NULL) {
        return NULL;
    }
    memset_s(netif, sizeof(struct netif), 0, sizeof(struct netif));
    strncpy_s(netif->name, sizeof(netif->name), name, strlen(name));
    if (memcpy_s(netif->hwaddr, NETIF_MAX_HWADDR_LEN, hw_addr, NETIF_MAX_HWADDR_LEN) != EOK) {
        free(netif);
        return NULL;
    }
    netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;
    netifapi_netif_add(netif, NULL, NULL, NULL, NULL, lwip_driver_init_fn, tcpip_input);
    return netif;
}

/* 收包入口
   参数 data：报文数据区指针 以ip开始
        len：数据长度
        netif：网络设备指针
*/
static err_t lwip_driver_input(u8_t *data, u16_t len, struct netif *netif)
{
    struct pbuf *p = NULL;

    if ((data == NULL) || (netif == NULL)) {
        return ERR_ARG;
    }
    p = pbuf_alloc(PBUF_RAW, (len + ETH_PAD_SIZE), PBUF_RAM);
    if (p == NULL) {
        return ERR_BUF;
    }
    if (memcpy_s(p->payload, len, data, len) != EOK) {
        return ERR_MEM;
    }
    p->len = len;
    netif->input(p, netif);
    return ERR_OK;
}
/* 发包函数
   参数 netif：网络设备指针
        p：pbuf指针
        ipaddr：不使用
 */
static err_t lwip_driver_output(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr)
{
    if (netif == NULL || p == NULL || ipaddr == NULL) {
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
    if (ip4_addr_isbroadcast(ipaddr, netif)) {
        pan_service_write_data(payload, p->tot_len, PAN_NETWORK_PROTOCOL_IPV4, PAN_PACKET_TYPE_DEST_ONLY_BORADCAST);
    } else {
        pan_service_write_data(payload, p->tot_len, PAN_NETWORK_PROTOCOL_IPV4, PAN_PACKET_TYPE_DEST_ONLY_UNICAST);
    }
    if (p_clone != NULL) {
        pbuf_free(p_clone);
    }
    return ERR_OK;
}

static void bt_pan_net_state_callback(uint8_t net_state)
{
    struct netif *netif_p = g_bt_pan_netif;
    if (netif_p == NULL) {
        return;
    }
    if (net_state == 1) {
        netifapi_netif_set_link_up(netif_p);
        netifapi_netif_set_up(netif_p);
#if defined(LWIP_DHCP) && LWIP_DHCP
        netifapi_dhcp_start(netif_p);
#endif
    } else {
#if defined(LWIP_DHCP) && LWIP_DHCP
        netifapi_dhcp_release_and_stop(netif_p);
#endif
        netifapi_netif_set_link_down(netif_p);
        netifapi_netif_set_down(netif_p);
#if LWIP_IPV4
        ip_addr_set_zero_ip4(&netif_p->ip_addr);
        ip_addr_set_zero_ip4(&netif_p->netmask);
        ip_addr_set_zero_ip4(&netif_p->gw);
#endif
#ifndef PAN_LOW_POWER
        sys_sem_signal(&g_pan_thread.pan_sem);
#endif
    }
}

#if defined(LWIP_IPV4) && LWIP_IPV4 && defined(LWIP_ARP) && LWIP_ARP
static int lwip_input_arp(u8_t *data, u16_t len, struct netif *netif)
{
    struct pbuf *p = NULL;

    if ((data == NULL) || (netif == NULL)) {
        return ERR_ARG;
    }
    p = pbuf_alloc(PBUF_RAW, (len + ETH_PAD_SIZE), PBUF_RAM);
    if (p == NULL) {
        return ERR_BUF;
    }
    if (memcpy_s(p->payload, len, data, len) != EOK) {
        return ERR_MEM;
    }
    p->len = len;
    etharp_input(p, netif);
    return ERR_OK;
}
#endif

static void bt_pan_rpt_ethernet(uint8_t *packet, uint16_t packet_length, uint16_t network_protocol)
{
    int ret = ERR_OK;
    struct netif *netif_p = g_bt_pan_netif;
    if (netif_p == NULL) {
        return;
    }
    if (network_protocol == PAN_NETWORK_PROTOCOL_ARP) {
#if defined(LWIP_IPV4) && LWIP_IPV4 && defined(LWIP_ARP) && LWIP_ARP
        ret = lwip_input_arp(packet, packet_length, netif_p);
#endif
    } else if (network_protocol == PAN_NETWORK_PROTOCOL_IPV4) {
        ret = lwip_driver_input(packet, packet_length, netif_p);
    } else {
        return;
    }
    if (ret != 0) {
        LWIP_DEBUG_LOG(("pan receive packet fail, ret=0x%x\n", ret));
    }
}

static pan_callbacks_t g_bt_pan_callbacks_t = {bt_pan_net_state_callback, bt_pan_rpt_ethernet};

/* mac addr to big-endian */
static void mac_addr_hton(uint8_t *data)
{
    uint8_t temp;

    temp = data[0];
    data[0] = data[5]; /* 5 : byte index */
    data[5] = temp;    /* 5 : byte index */

    temp = data[1];    /* 1 : byte index */
    data[1] = data[4]; /* 1 4 : byte index */
    data[4] = temp;    /* 4 : byte index */

    temp = data[2];    /* 2 : byte index */
    data[2] = data[3]; /* 2 3 : byte index */
    data[3] = temp;    /* 3 : byte index */
}

static void init_register_bt_netif(void)
{
#if defined(BRANDY_PRODUCT_EVB4) || defined(BRANDY_PRODUCT_EVB)
    unsigned char local_addr[6];          /* 6: mac addr length */
    bluetooth_get_local_addr(local_addr, 6); /* 6: mac addr length */
    mac_addr_hton(local_addr);
#else
    bd_addr_t local_addr = {0};
    bluetooth_get_local_addr(&local_addr);
#endif
    if (g_bt_pan_netif != NULL) {
        return;
    }
#if defined(BRANDY_PRODUCT_EVB4) || defined(BRANDY_PRODUCT_EVB)
    g_bt_pan_netif = lwip_driver_init_netif("bt", local_addr);
#else
    g_bt_pan_netif = lwip_driver_init_netif("bt", local_addr.addr);
#endif
    if (g_bt_pan_netif == NULL) {
        LWIP_DEBUG_LOG(("bt pan init lwip fail\n"));
    }
}

static int bt_pan_get_paired_device(bd_addr_t *addr)
{
    int index = 0;
    int number = BTPAN_MAX_PAIRED_DEVICES;
#if defined(BRANDY_PRODUCT_EVB4) || defined(BRANDY_PRODUCT_EVB)
    gap_paired_device_info_t dev_info[BTPAN_MAX_PAIRED_DEVICES] = {0};
#else
    gap_remote_device_info_t dev_info[BTPAN_MAX_PAIRED_DEVICES] = {0};
#endif
#if defined(BRANDY_PRODUCT_EVB4) || defined(BRANDY_PRODUCT_EVB)
    if (!gap_get_paired_devices_list(dev_info, &number)) {
        LWIP_DEBUG_LOG(("get bt dev info fail\n"));
        return -1;
    }
#else
    if (gap_br_get_paired_devices_list(dev_info, &number) != ERRCODE_BT_SUCCESS) {
        LWIP_DEBUG_LOG(("get bt dev info fail\n"));
        return -1;
    }
#endif
    for (index = 0; index < BTPAN_MAX_PAIRED_DEVICES; index++) {
#if defined(BRANDY_PRODUCT_EVB4) || defined(BRANDY_PRODUCT_EVB)
        if ((gap_get_device_class(&dev_info[index].addr) & DEVICE_MAJOR_CLASS_PHONE) != 0) {
            break;
        }
#else
        if((dev_info[index].device_class & DEVICE_MAJOR_CLASS_PHONE) != 0) {
            break;
        }
#endif
    }
    if (index >= BTPAN_MAX_PAIRED_DEVICES) {
        LWIP_DEBUG_LOG(("Not find paired phone\n"));
        return -1;
    }
    if (memcpy_s(addr, sizeof(bd_addr_t), &dev_info[index].addr, sizeof(bd_addr_t)) != 0) {
        return -1;
    }
    return 0;
}

#if defined(PAN_LOW_POWER)
static int bt_pan_open_service(void)
{
    bd_addr_t addr;
    int ret;
    ret = bt_pan_get_paired_device(&addr);
    if (ret != 0) {
        LWIP_DEBUG_LOG(("bt_pan_get_paired_device fail\n"));
        return ret;
    }
    ret = pan_service_open(&addr);
    if (ret == 0) {
        LWIP_DEBUG_LOG(("pan_service_open ok\n"));
    } else {
        LWIP_DEBUG_LOG(("pan_service_open fail, ret=0x%x\n", ret));
    }
    return ret;
}

#else
static void bt_pan_thread(void *arg)
{
    bd_addr_t addr;
    int ret;
    thread_ctl *thread = (thread_ctl *)arg;
    if (thread == NULL) {
        return;
    }
    thread->thread_run = 1;
    sys_sem_new(&thread->pan_sem, 0);

    while (thread->end_thread == 0) {
        ret = bt_pan_get_paired_device(&addr);
        if (ret != 0) {
            LWIP_DEBUG_LOG(("bt_pan_get_paired_device fail\n"));
            break;
        }
        if (pan_service_open(&addr) == 0) {
            sys_arch_sem_wait(&thread->pan_sem, 0);
            pan_service_close(&addr);
        } else {
            break;
        }
        if (thread->end_thread) {
            break;
        }
        sleep(30); /* 30 : sleep 30s */
    }
    sys_sem_free(&thread->pan_sem);
    thread->thread_run = 0;
    thread->end_thread = 0;
}
#endif

void bt_pan_close(void)
{
    int ret;
    bd_addr_t addr;
    struct netif *netif_p = g_bt_pan_netif;
    if (netif_p == NULL) {
        return;
    }
    pan_deregister_callbacks();
#ifndef PAN_LOW_POWER
    if (g_pan_thread.thread_run == 1) {
        g_pan_thread.end_thread = 1;
        sys_sem_signal(&g_pan_thread.pan_sem);
    }

    if (g_pan_thread.param != NULL) {
        sys_thread_delete(g_pan_thread.param);
        g_pan_thread.param = NULL;
    }
#endif
    netifapi_netif_set_link_down(netif_p);
    netifapi_netif_set_down(netif_p);
    netifapi_netif_remove(netif_p);

    free(netif_p);
    g_bt_pan_netif = NULL;
    g_pan_net_state = CONNECTION_DISCONNECTED;

    ret = bt_pan_get_paired_device(&addr);
    if (ret != 0) {
        return;
    }
    pan_service_close(&addr);
}

void bt_pan_open(void)
{
    init_register_bt_netif();

    pan_register_callbacks(&g_bt_pan_callbacks_t);
#ifdef PAN_LOW_POWER
    if (bt_pan_open_service() != 0) {
        bt_pan_close();
    }
#else
    if (g_pan_thread.thread_run == 0) {
        g_pan_thread.end_thread = 0;
        g_pan_thread.param =
            (void *)sys_thread_new(BTPAN_THREAD_NAME, bt_pan_thread, &g_pan_thread, BTPAN_THREAD_STACKSIZE, BTPAN_THREAD_PRIO);
        if (g_pan_thread.param == NULL) {
            LWIP_DEBUG_LOG(("bt pan thread create fail\n"));
        }
    }
#endif
}

uint8_t pan_get_net_state(void)
{
    return g_pan_net_state;
}

#if LWIP_NETIF_EXT_STATUS_CALLBACK
static netif_ext_callback_t g_netif_ext_callback;

static void pan_net_callback(struct netif *netif, netif_nsc_reason_t reason, const netif_ext_callback_args_t *args)
{
    if (netif != g_bt_pan_netif) {
        return;
    }

    if ((reason & LWIP_NSC_IPV4_ADDRESS_CHANGED || reason & LWIP_NSC_IPV6_SET) &&
        g_pan_net_state != CONNECTION_CONNECTED) {
        g_pan_net_state = CONNECTION_CONNECTED;
        if (g_bt_pan_callback != NULL) {
            LWIP_DEBUG_LOG(("pan CONNECTION_CONNECTED\n"));
            g_bt_pan_callback(LWIP_NETIF_BT_PAN, CONNECTION_CONNECTED);
        }
    }

    if (reason & LWIP_NSC_STATUS_CHANGED && args->status_changed.state == 0 &&
        g_pan_net_state != CONNECTION_DISCONNECTED) {
        g_pan_net_state = CONNECTION_DISCONNECTED;
        if (g_bt_pan_callback != NULL) {
            LWIP_DEBUG_LOG(("pan CONNECTION_DISCONNECTED\n"));
            g_bt_pan_callback(LWIP_NETIF_BT_PAN, CONNECTION_DISCONNECTED);
        }
    }
}

void bt_pan_register_net_callback(net_connection_callback callback)
{
    if (g_bt_pan_callback != NULL) {
        return;
    }
    g_bt_pan_callback = callback;
    netif_add_ext_callback(&g_netif_ext_callback, pan_net_callback);
}

void bt_pan_unregister_net_callback(void)
{
    g_bt_pan_callback = NULL;
    netif_remove_ext_callback(&g_netif_ext_callback);
}
#else
void bt_pan_register_net_callback(net_connection_callback callback)
{
    (void)callback;
}

void bt_pan_unregister_net_callback(void)
{}
#endif