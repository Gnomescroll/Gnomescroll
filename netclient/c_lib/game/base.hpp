#pragma once

#include <game/game.hpp>
#include <voxel/voxel_model.hpp>
#include <entity/constants.hpp>

const int N_BASES = N_TEAMS;

const int BASE_PART_NUM = 1;

enum BASE_PARTS {
    BASE_PART_MAIN
};

const int BASE_SPAWN_RADIUS = 10;
const float BASE_HEIGHT = 1.0f;

class Base {
    public:
        float x,y,z;
        //float vx,vy,vz;
        float theta,phi;

        int id;
        int team;
        ObjectType type;

        unsigned int spawn_radius;
        
        Voxel_model* vox;

        void set_position(float x, float y, float z);
        struct Vec3 get_spawn_point(float agent_height, float agent_radius);

        void tick();
        void update(); // client side
        
        Base(int id, int team);
        ~Base();
};

