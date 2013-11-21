/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
   Licensed under GPLv3 */
#pragma once

#include <string.h>
#include <stdio.h>

class NetworkMonitor
{
    public:
        size_t incoming[0xFF];
        size_t outgoing[0xFF];

    size_t total_sent() const
    {
        size_t x = 0;
        for (int i=0; i<0xFF; i++)
            x += this->outgoing[i];
        return x;
    }

    size_t total_received() const
    {
        size_t x = 0;
        for (int i=0; i<0xFF; i++)
            x += this->incoming[i];
        return x;
    }

    void received(uint8_t message_id, size_t transferred)
    {
        this->incoming[message_id] += transferred;
    }

    void sent(uint8_t message_id, size_t transferred)
    {
        this->outgoing[message_id] += transferred;
    }

    void print() const;

    NetworkMonitor()
    {
        memset(this->incoming, 0, sizeof(this->incoming));
        memset(this->outgoing, 0, sizeof(this->outgoing));
    }
};
