#pragma once

#include <c_lib/game/game.hpp>
#include <voxel/object_vox.hpp>
#include <common/enum_types.hpp>

const int N_FLAGS = N_TEAMS;

const int FLAG_PART_NUM = 1;
extern VoxBody flag_vox_dat;

enum FLAG_PARTS {
    FLAG_PART_CLOTH
};

class Flag;

class Flag_vox: public Object_vox<Flag, FLAG_PART_NUM>
{
    public:
    void init_parts(VoxBody* vox_dat);
    Flag_vox(Flag* a, VoxBody* vox_dat)
    :
    Object_vox<Flag, FLAG_PART_NUM>(a)
    {}
};

class Flag {
    public:
        float x,y,z;
        //float vx,vy,vz;
        float theta,phi;

        int id;
        int team;
        Object_types type;

        Flag_vox* vox;

        void set_position(float x, float y, float z);

        Flag();
        ~Flag();
};

