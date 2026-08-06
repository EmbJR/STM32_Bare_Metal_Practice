#ifndef ETHERNETIF_PORT_H
#define ETHERNETIF_PORT_H

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

extern ENC28J60_ConfigTypeDef encdevice;
extern struct netif enc28j60_netif;


err_t ethernetif_init(struct netif *netif);

#endif  // ETHERNETIF_PORT_H
