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

class Base_vox: public Object_vox<Base, BASE_PART_NUM>
{
    public:
    Base_vox(Base* a, VoxBody* vox_dat) : Object_vox<Base, BASE_PART_NUM>(a, vox_dat) {}
};

class Base {
    public:
        float x,y,z;
        //float vx,vy,vz;

        int id;
        int team;
        Object_types type;
        
        unsigned char r,g,b;

        Base_vox* vox;

        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_position(float x, float y, float z);
        void draw();

        Base();
        ~Base();
};

