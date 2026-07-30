#include <stdio.h>
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/sys.h"

int main(void) {
    printf("Initializing lwIP Stack in Code::Blocks...\n");

    // 1. Initialize lwIP Core
    lwip_init();

    // 2. Define Network Addresses
    ip4_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr, 192, 168, 1, 100);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
    IP4_ADDR(&gw, 192, 168, 1, 1);

    printf("lwIP initialized successfully!\n");
    printf("Static IP Configuration: %s\n", ip4addr_ntoa(&ipaddr));

    return 0;
}

/* This function is only required to prevent arch.h including stdio.h
 * (which it does if LWIP_PLATFORM_ASSERT is undefined)
 */
void lwip_example_app_platform_assert(const char *msg, int line, const char *file)
{
  printf("Assertion \"%s\" failed at line %d in %s\n", msg, line, file);
  fflush(NULL);
  abort();
}

#if defined(_WIN32)
#include <windows.h>
#endif

/* System tick timer function required by lwIP */
u32_t sys_now(void) {
#if defined(_WIN32)
    /* For Windows simulation in Code::Blocks, return Windows millisecond count */
    return GetTickCount();
#else
    /* For STM32 bare-metal target hardware, return your SysTick counter */
    extern volatile uint32_t uwTick; // Or HAL_GetTick()
    return uwTick;
#endif
}
