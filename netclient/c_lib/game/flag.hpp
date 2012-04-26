#pragma once

#include <c_lib/game/game.hpp>
#include <voxel/voxel_model.hpp>
#include <entity/constants.hpp>

const int N_FLAGS = N_TEAMS;

const int FLAG_PART_NUM = 1;

enum FLAG_PARTS {
    FLAG_PART_CLOTH
};

const float FLAG_HEIGHT = 1.0f;

class Flag {
    public:
        float x,y,z;
        //float vx,vy,vz;
        float theta,phi;

        int id;
        int team;
        ObjectType type;

        bool held;

        Voxel_model* vox;

        void set_position(float x, float y, float z);
        void tick();
        void animate(); // client side
        void update();  // client side
        
        Flag(int id, int team);
        ~Flag();
};

