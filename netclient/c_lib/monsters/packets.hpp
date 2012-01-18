#pragma once

#include <net_lib/net.hpp>

class CreateSlime_StoC: public FixedSizeReliableNetPacketToClient<CreateSlime_StoC>
{

    public:
        int id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }

        inline void handle();
};

class DestroySlime_StoC: public FixedSizeReliableNetPacketToClient<DestroySlime_StoC>
{

    public:
        int id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }

        inline void handle();
};

class SlimeState_StoC: public FixedSizeReliableNetPacketToClient<SlimeState_StoC>
{
    public:
        int id;
        int seq;

        float x,y,z;
        float vx,vy,vz;
        float theta, phi;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);   //assume id is 1 byte
            pack_u8(&seq, buff, buff_n, pack);

            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);

            pack_float(&theta, buff, buff_n, pack);
            pack_float(&phi, buff, buff_n, pack);
        }

        inline void handle();
};

