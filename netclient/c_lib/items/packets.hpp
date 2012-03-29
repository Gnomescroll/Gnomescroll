#pragma once

#include <net_lib/net.hpp>

class object_create_StoC: public FixedSizeNetPacketToClient<object_create_StoC>
{
    public:
        uint8_t type;
        uint8_t id;
        float x,y,z,mx,my,mz;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);
        }
        inline void handle();
};

class object_destroy_StoC: public FixedSizeNetPacketToClient<object_destroy_StoC>
{
    public:
        uint8_t type;
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class object_picked_up_StoC: public FixedSizeNetPacketToClient<object_picked_up_StoC>
{
    public:
        uint8_t type;
        uint8_t id;
        uint8_t agent_id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&agent_id, buff, buff_n, pack);
        }
        inline void handle();
};

class object_shot_object_StoC: public FixedSizeNetPacketToClient<object_shot_object_StoC>
{
    public:
        uint8_t id;
        uint8_t type;
        uint16_t target_id;
        uint8_t target_type;
        uint8_t target_part;
        uint8_t voxel_x;
        uint8_t voxel_y;
        uint8_t voxel_z;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u16(&target_id, buff, buff_n, pack);
            pack_u8(&target_type, buff, buff_n, pack);
            pack_u8(&target_part, buff, buff_n, pack);
            pack_u8(&voxel_x, buff, buff_n, pack);
            pack_u8(&voxel_y, buff, buff_n, pack);
            pack_u8(&voxel_z, buff, buff_n, pack);
        }
    inline void handle();
};

class object_shot_terrain_StoC: public FixedSizeNetPacketToClient<object_shot_terrain_StoC>
{
    public:
        uint8_t id;
        uint8_t type;
        uint8_t cube;
        uint8_t side;
        float x,y,z;

    inline void packet(char* buffer, int* buff_n, bool pack)
    {
        pack_u8(&id, buffer, buff_n, pack);
        pack_u8(&type, buffer, buff_n, pack);
        pack_u8(&cube, buffer, buff_n, pack);
        pack_u8(&side, buffer, buff_n, pack);
        pack_float(&x, buffer, buff_n, pack);
        pack_float(&y, buffer, buff_n, pack);
        pack_float(&z, buffer, buff_n, pack);
    }
    inline void handle();
};

class object_shot_nothing_StoC: public FixedSizeNetPacketToClient<object_shot_nothing_StoC>
{
    public:
        uint8_t id;
        uint8_t type;
        float x,y,z;

    inline void packet(char* buffer, int* buff_n, bool pack)
    {
        pack_u8(&id, buffer, buff_n, pack);
        pack_u8(&type, buffer, buff_n, pack);
        pack_float(&x, buffer, buff_n, pack);
        pack_float(&y, buffer, buff_n, pack);
        pack_float(&z, buffer, buff_n, pack);
    }
    inline void handle();
};
