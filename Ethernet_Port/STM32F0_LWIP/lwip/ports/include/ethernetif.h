#ifndef ETHERNETIF_PORT_H
#define ETHERNETIF_PORT_H

#include <string.h>

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "enc28j60.h"
#include "main.h"

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"


extern ENC28J60_ConfigTypeDef encdevice;
extern struct netif enc28j60_netif;


err_t ethernetif_init(struct netif *netif);
void ethernetif_input(struct netif *netif);

#endif  // ETHERNETIF_PORT_H
