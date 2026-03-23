#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/netif.h"
#include "lwip/err.h"

#define NETIF_MTU								      ( 1500 )

#define NETIF_IN_TASK_STACK_SIZE			( 1024 )
#define NETIF_IN_TASK_PRIORITY			  ( 3 )

#define NETIF_OUT_TASK_STACK_SIZE			( 1024 )
#define NETIF_OUT_TASK_PRIORITY			  ( 3 )

err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct pbuf *p, struct netif *netif);
void ethernetif_output(void *argument);
void ethernetif_update_config(struct netif *netif);
void ethernetif_notify_conn_changed(struct netif *netif);

u32_t sys_jiffies(void);
u32_t sys_now(void);

#endif
