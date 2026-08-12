#if 1
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/*============================================================================
 * Core Operating System Settings
 *============================================================================*/
#define NO_SYS                          1   /* No RTOS - bare metal mode [reference:1][reference:2] */
#define MEM_ALIGNMENT           4
#define SYS_LIGHTWEIGHT_PROT            0   /* Disable protection (single thread) [reference:3] */

/*============================================================================
 * Protocol Support - UDP Only (Disable TCP)
 *============================================================================*/
#define LWIP_UDP                        0   /* Enable UDP [reference:4][reference:5] */
#define LWIP_TCP                        0   /* Disable TCP (saves ~40KB of code) [reference:6] */
#define LWIP_RAW                        0   /* Disable RAW API (not needed) */

/*============================================================================
 * Network Discovery (ARP) - Required for your device to be "seen"
 *============================================================================*/
#define LWIP_ARP                        1   /* Enable ARP - makes device discoverable [reference:7] */
#define LWIP_ETHERNET           1
#define LWIP_ICMP               1           /* Ping response */
#define ARP_TABLE_SIZE                  10  /* Max entries in ARP cache [reference:8] */
#define ETHARP_TRUST_IP_MAC             1   /* Learn MAC from incoming IP packets */

/*============================================================================
 * IP Layer Settings
 *============================================================================*/
#define LWIP_IPV4                       1   /* Enable IPv4 */
#define IP_FORWARD                      0   /* Disable forwarding (not a router) */
#define IP_REASSEMBLY                   0   /* Disable reassembly (saves RAM) */
#define IP_FRAG                         0   /* Disable fragmentation */

#define MEMP_NUM_SYS_TIMEOUT    3           /* ARP and link timers */

/*============================================================================
 * DHCP / AutoIP - Disable for static IP (faster startup)
 *============================================================================*/
#define LWIP_DHCP                       0   /* Disable DHCP - use static IP [reference:9][reference:10] */
#define LWIP_AUTOIP                     0   /* Disable AutoIP */

/*============================================================================
 * Memory Settings - Keep small for microcontrollers
 *============================================================================*/
#define MEM_SIZE                        (1 * 1024)  /* Heap size (8KB) [reference:11] */
#define MEMP_NUM_UDP_PCB                2           /* Max UDP connections [reference:12][reference:13] */
#define MEMP_NUM_PBUF                   2           /* Max packet buffers */
#define PBUF_POOL_SIZE                  2           /* Number of packet buffers [reference:14] */
#define PBUF_POOL_BUFSIZE               1536         /* Size of each buffer */

/*============================================================================
 * Checksum Settings - Let hardware or skip for speed
 *============================================================================*/
#define CHECKSUM_GEN_UDP                1           /* Generate UDP checksums */
#define CHECKSUM_CHECK_UDP              1           /* Verify incoming UDP checksums */

/*============================================================================
 * Debugging - Disable for release (saves code)
 *============================================================================*/
#define LWIP_DEBUG                      0           /* Disable all debugging */

/*============================================================================
 * API Selection - Using Raw API only
 *============================================================================*/
#define LWIP_SOCKET                     0   /* Disable Socket API (requires OS)  */
#define LWIP_NETCONN                    0   /* Disable Netconn API (requires OS)  */

#define LWIP_TIMEVAL_PRIVATE		0
/* --- PPP Support --- */
#define PPP_SUPPORT             0   /* Disable PPP (you already have this) */
#define PPP_NUM_TIMEOUTS        0   /* Explicitly define to 0 when disabled */

#endif /* __LWIPOPTS_H__ */
#endif



#if 0
/* Minimal lwipopts.h for UDP/ICMP (~2.5 KB total RAM usage) */
#define NO_SYS                  1
#define MEM_ALIGNMENT           4

/* Memory Heap & Pools */
#define MEM_SIZE                (2 * 1024)  /* 1 KB Heap */
#define PBUF_POOL_SIZE          3           /* 3 RX Buffers */
#define PBUF_POOL_BUFSIZE       512         /* Chained 512-byte buffers */
#define MEMP_NUM_PBUF           3

/* Protocols */
#define LWIP_ARP                1
#define LWIP_ETHERNET           1
#define LWIP_ICMP               1           /* Ping response */
#define LWIP_UDP                1
#define LWIP_TCP                0           /* Disable TCP completely to save ~3 KB */

/* Memory Pools for Control Blocks */
#define MEMP_NUM_UDP_PCB        1
#define MEMP_NUM_TCP_PCB        0
#define MEMP_NUM_SYS_TIMEOUT    3           /* ARP and link timers */

/* Disable all unnecessary features */
#define LWIP_DHCP               0
#define LWIP_STATS              0
#define LWIP_DEBUG              0

//----------------------------------
#define SYS_LIGHTWEIGHT_PROT        0   /* Disable task protection macros */

#define LWIP_TIMEVAL_PRIVATE		0
#define LWIP_SOCKET					0
#define LWIP_NETCONN					0
#endif

#if 0
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/* =====================================================================
   1. System & Architecture Options
   ===================================================================== */
#define NO_SYS                      1   /* Bare-metal mode (No RTOS) */
#define SYS_LIGHTWEIGHT_PROT        0   /* Disable task protection macros */
#define MEM_ALIGNMENT               4   /* 32-bit alignment for STM32 */

/* =====================================================================
   2. Memory Allocation & Heap Setup
   ===================================================================== */
#define MEM_LIBC_MALLOC             0   /* Do NOT use standard malloc */
#define MEMP_MEM_MALLOC             0   /* Use dedicated memory pools */

/* Main heap size for dynamic lwIP allocations (reduced to 2 KB) */
#define MEM_SIZE                    (2 * 1024)

/* Number of struct pbuf allocated from pool */
#define MEMP_NUM_PBUF               2
/* Number of raw connection PCBs */
#define MEMP_NUM_RAW_PCB            0
/* Number of UDP PCBs (Control Blocks) */
#define MEMP_NUM_UDP_PCB            2
/* Number of TCP PCBs */
#define MEMP_NUM_TCP_PCB            2
/* Number of listening TCP PCBs */
#define MEMP_NUM_TCP_PCB_LISTEN     1
/* Number of active TCP segments queued for transmission */
#define MEMP_NUM_TCP_SEG            4
/* Number of ARP queue entries */
#define MEMP_NUM_ARP_QUEUE          2
/* Number of active timers */
#define MEMP_NUM_SYS_TIMEOUT        5

/* =====================================================================
   3. Packet Buffer (PBUF) Configuration
   ===================================================================== */
/* Number of RAM buffers in the pbuf pool */
#define PBUF_POOL_SIZE              2

/*
 * Packet buffer size: Set to ENC28J60 maximum frame size (1536 bytes)
 * so 1 packet fits in 1 buffer.
 * Total RAM used for PBUF Pool: 4 * 1536 = 6 KB (shared in heap/pools)
 */
#define PBUF_POOL_BUFSIZE           1536

/* =====================================================================
   4. IPv4 & ARP Setup
   ===================================================================== */
#define LWIP_IPV4                   1
#define LWIP_ARP                    1
#define LWIP_ETHERNET               1
#define LWIP_ICMP                   1   /* Enable Ping response */
#define LWIP_RAW                    0

/* =====================================================================
   5. UDP Configuration
   ===================================================================== */
#define LWIP_UDP                    1
#define UDP_TTL                     255

/* =====================================================================
   6. TCP Configuration (Strictly Scaled Down)
   ===================================================================== */
#define LWIP_TCP                    0
#define TCP_TTL                     255

/* Maximum Segment Size: Keep it to 536 for low RAM usage */
#define TCP_MSS                     536

/* TCP Receive Window: Must be at least 2 * MSS */
#define TCP_WND                     (2 * TCP_MSS)

/* Send buffer space in bytes */
#define TCP_SND_BUF                 (2 * TCP_MSS)

/* Send queue length (number of pbufs in send buffer) */
#define TCP_SND_QUEUELEN            4

/* =====================================================================
   7. Feature Reduction (Disable Unused Modules to Save RAM/Flash)
   ===================================================================== */
#define LWIP_DHCP                   0   /* Disable DHCP (Use Static IP) */
#define LWIP_AUTOIP                 0
#define LWIP_DNS                    0
#define LWIP_IGMP                   0
#define LWIP_STATS                  0   /* Disable statistics gathering */
#define LWIP_NETIF_STATUS_CALLBACK  0
#define LWIP_NETIF_LINK_CALLBACK    0
#define LWIP_HAVE_LOOPIF            0
#define LWIP_NETIF_LOOPBACK         0

/* Disable debug output overhead */
#define LWIP_DEBUG                  0



#define LWIP_TIMEVAL_PRIVATE		0
#define LWIP_SOCKET					0
#define LWIP_NETCONN					0


#endif /* __LWIPOPTS_H__ */

#endif	// if 1
