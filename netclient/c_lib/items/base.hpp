#pragma once

#include <c_lib/game/game.hpp>
#include <voxel/object_vox.hpp>
#include <common/enum_types.hpp>

const int N_BASES = N_TEAMS;

const int BASE_PART_NUM = 1;
extern VoxBody base_vox_dat;

enum BASE_PARTS {
    BASE_PART_MAIN
};

class Base;

class Base_vox: public Team_vox<Base, BASE_PART_NUM>
{
    public:
    Base_vox(Base* a)
    :
    Team_vox<Base, BASE_PART_NUM>(a)
    {}
};

class Base {
    public:
        float x,y,z;
        //float vx,vy,vz;
        float theta,phi;

        int id;
        int team;
        Object_types type;
        
        Base_vox* vox;

        void set_position(float x, float y, float z);

        Base();
        ~Base();
};

