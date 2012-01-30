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

const int BASE_SPAWN_RADIUS = 10;

class Base {
    public:
        float x,y,z;
        //float vx,vy,vz;
        float theta,phi;

        int id;
        int team;
        Object_types type;

        unsigned int spawn_radius;
        
        Voxel_model* vox;

        void set_position(float x, float y, float z);
        void get_spawn_point(int *spawn, int agent_height);
        
        Base(int id, int team);
        ~Base();
};

