#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/udp.h"
#include "lwip/timeouts.h"
#include "netif/ethernet.h"

/* Include Windows Pcap network interface initializer */
#include "pcapif.h"

// System tick required for lwIP
u32_t sys_now(void) {
    return GetTickCount();
}

struct netif g_pcap_netif;

int main(void) {
    printf("===================================================\n");
    printf(" lwIP Real-IP String Sender (Windows Code::Blocks) \n");
    printf("===================================================\n\n");

    // 1. Initialize lwIP stack
    lwip_init();

    // 2. Define Network Configurations
    // NOTE: Match these to your actual LAN network router setup!
    ip4_addr_t local_ip, netmask, gateway, target_ip;
    IP4_ADDR(&local_ip,  192, 168, 29, 83);  // IP for lwIP stack on Windows
    IP4_ADDR(&netmask,   255, 255, 255, 0);  // Subnet mask
    IP4_ADDR(&gateway,   192, 168, 29, 1);    // Gateway

    // Target IP address to receive the string (another PC, phone, or board)
    //IP4_ADDR(&target_ip, 155, 155, 255, 255);   // Target IP
    IP4_ADDR(&target_ip, 192,168,29,198);
    u16_t target_port = 5000;                // Target UDP Port

    // 3. Register PCAP Network Interface
    // pcapif_init will display a menu in console to pick your real Network Adapter
    printf("Initializing Network Adapter via Npcap...\n");
    if (netif_add(&g_pcap_netif, &local_ip, &netmask, &gateway, NULL, pcapif_init, ethernet_input) == NULL) {
        printf("[ERROR] Failed to initialize pcapif network interface.\n");
        return -1;
    }

    netif_set_default(&g_pcap_netif);
    netif_set_link_up(&g_pcap_netif);
    netif_set_up(&g_pcap_netif);

    printf("\nNetwork Interface is UP.\n");

    // 4. Create UDP Protocol Control Block (PCB)
    struct udp_pcb *udp_client = udp_new();
    if (udp_client == NULL) {
        printf("[ERROR] Could not create UDP PCB.\n");
        return -1;
    }

    // Bind to any local port
    udp_bind(udp_client, IP_ADDR_ANY, 0);

    // 5. Create payload buffer and copy the message string
    const char *message = "Hello from lwIP running in Code::Blocks on Windows!";
    u16_t msg_len = (u16_t)strlen(message);

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, msg_len, PBUF_RAM);
    if (p != NULL) {
        memcpy(p->payload, message, msg_len);

        printf("\nSending UDP string to %d.%d.%d.%d:%d ...\n",
               ip4_addr1(&target_ip), ip4_addr2(&target_ip),
               ip4_addr3(&target_ip), ip4_addr4(&target_ip), target_port);

        // 6. Transmit packet over physical Ethernet/Wi-Fi card
        err_t err = udp_sendto(udp_client, p, &target_ip, target_port);
        if (err == ERR_OK) {
            printf("[SUCCESS] Packet passed to Npcap physical driver!\n");
        } else {
            printf("[ERROR] Send failed with error code: %d\n", err);
        }

        pbuf_free(p);
    }

    // 7. Polling Super-Loop (processes ARP replies and hardware packets)
    printf("Polling network interface (10 seconds)... Press Ctrl+C to stop.\n");
    DWORD start_time = GetTickCount();
    while (GetTickCount() - start_time < 10000) {
        /* Poll Npcap for incoming physical packets & ARP responses */
        pcapif_poll(&g_pcap_netif);
        sys_check_timeouts();
        Sleep(10);
    }

    // Cleanup
    udp_remove(udp_client);
    printf("Done.\n");

    return 0;
}
