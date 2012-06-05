#pragma once

#include <net_lib/net.hpp>

static int _CtoSbenchmark_counter = 0;

class benchmarkCtoS: public FixedSizeNetPacketToServer<benchmarkCtoS>
{
    public:

        int id;
        int seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_u16(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            _CtoSbenchmark_counter++;
            if(_CtoSbenchmark_counter % 1000 == 0) printf("%i messages\n", _CtoSbenchmark_counter);
            return;
        }
};


//static int _r_sent = 0;
static int _r_received = 0;

class benchmarkReliableCtoS: public FixedSizeReliableNetPacketToServer<benchmarkReliableCtoS>
{
    public:

        int id;
        int seq;
        uint16_t cs;
        float theta;
        float phi;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&seq, buff, buff_n, pack);
            pack_u16(&cs, buff, buff_n, pack);
            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle() {
            _r_received++;
            //if(_CtoSbenchmark_counter % 1000 == 0) printf("%i messages\n", _CtoSbenchmark_counter);
            printf("Received %i reliable messages\n", _r_received);
            return;
        }
};
