#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "lwip/timeouts.h"

u32_t sys_now(void) {
    return GetTickCount();
}

struct netif loop_netif;
static volatile u8_t packet_received = 0;

// 1. Output wrapper for IPv4 loopback
static err_t my_loop_output(struct netif *netif, struct pbuf *p, const ip4_addr_t *ipaddr) {
    LWIP_UNUSED_ARG(ipaddr);
    return netif_loop_output(netif, p);
}

// 2. Loopback Netif Initialization
static err_t my_loopif_init(struct netif *netif) {
    netif->name[0] = 'l';
    netif->name[1] = 'o';
    netif->output = my_loop_output;
    return ERR_OK;
}

// 3. UDP Receive Callback
void udp_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                       const ip_addr_t *addr, u16_t port) {
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(pcb);
    LWIP_UNUSED_ARG(addr);
    LWIP_UNUSED_ARG(port);

    if (p != NULL) {
        printf("\n==========================================\n");
        printf("[SUCCESS] UDP Packet Received via lwIP Loopback!\n");
        printf("Payload Length: %d bytes\n", p->tot_len);
        printf("Data Content: %s\n", (char *)p->payload);
        printf("==========================================\n\n");

        packet_received = 1;
        pbuf_free(p);
    }
}

int main(void) {
    printf("--- Starting lwIP Bare-Metal Loopback Test ---\n");

    // Initialize lwIP Stack
    lwip_init();

    // Set up Loopback Netif (127.0.0.1)
    ip4_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr, 127, 0, 0, 1);
    IP4_ADDR(&netmask, 255, 0, 0, 0);
    IP4_ADDR(&gw, 127, 0, 0, 1);

    netif_add(&loop_netif, &ipaddr, &netmask, &gw, NULL, my_loopif_init, netif_input);
    netif_set_default(&loop_netif);
    netif_set_link_up(&loop_netif);
    netif_set_up(&loop_netif);

    // Create UDP Server listening on port 7000
    struct udp_pcb *server_pcb = udp_new();
    udp_bind(server_pcb, IP_ADDR_ANY, 7000);
    udp_recv(server_pcb, udp_recv_callback, NULL);

    printf("UDP Server listening on port 7000...\n");

    // Create UDP Client and Send Data
    struct udp_pcb *client_pcb = udp_new();
    const char *message = "Hello from lwIP in Code::Blocks!";

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, (u16_t)(strlen(message) + 1), PBUF_RAM);
    if (p != NULL) {
        memcpy(p->payload, message, strlen(message) + 1);

        printf("Sending packet: '%s'\n", message);
        udp_sendto(client_pcb, p, &ipaddr, 7000);
        pbuf_free(p);
    }

    // Bare-Metal Polling Super-Loop
    DWORD start_time = GetTickCount();
    while (!packet_received && (GetTickCount() - start_time < 3000)) {
        // Poll enqueued loopback packets to deliver them to netif_input
        netif_poll_all();
        sys_check_timeouts();
        Sleep(10);
    }

    if (!packet_received) {
        printf("[FAIL] Timeout: Packet was not received.\n");
    }

    // Clean up
    udp_remove(server_pcb);
    udp_remove(client_pcb);

    return 0;
}
