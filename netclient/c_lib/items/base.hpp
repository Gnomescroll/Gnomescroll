#pragma once

#include <c_lib/game/game.hpp>
#include <voxel/voxel_model.hpp>
#include <common/enum_types.hpp>

const int N_BASES = N_TEAMS;

const int BASE_PART_NUM = 1;
extern VoxDat base_vox_dat;

enum BASE_PARTS {
    BASE_PART_MAIN
};

class Base;

class Base {
    public:
        float x,y,z;
        //float vx,vy,vz;
        float theta,phi;

        int id;
        int team;
        Object_types type;
        
        Voxel_model* vox;

        void set_position(float x, float y, float z);

        Base(int id, int team);
        ~Base();
};

