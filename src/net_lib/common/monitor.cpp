/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#include "monitor.hpp"

#include <net_lib/common/message_handler.h>

void NetworkMonitor::print() const
{
    const char fmt[] = "%llu bytes :: [%d] %s \n";
    printf("=============\n");
    printf("Network stats:\n");
    printf("=============\n");
    printf("Received: %llu bytes\n", (long long unsigned)this->total_received());
    for (uint8_t i=0; i<0xFF; i++)
    {
        #if DC_SERVER
        const char* name = get_server_packet_name(i);
        #else
        const char* name = get_client_packet_name(i);
        #endif
        if (name != NULL)
            printf(fmt, (long long unsigned)this->incoming[i], i, name);
    }
    printf("=============\n");
    printf("Sent: %llu bytes\n", (long long unsigned)this->total_sent());
    for (uint8_t i=0; i<0xFF; i++)
    {
        #if DC_SERVER
        const char* name = get_client_packet_name(i);
        #else
        const char* name = get_server_packet_name(i);
        #endif
        if (name != NULL)
            printf(fmt, (long long unsigned)this->outgoing[i], i, name);
    }
    printf("=============\n");
}
