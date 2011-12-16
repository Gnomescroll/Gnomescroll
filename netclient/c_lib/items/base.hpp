#pragma once

#include <c_lib/game/game.hpp>

const int N_BASES = N_TEAMS;

class Base {
    public:
        float x,y,z;
        //float vx,vy,vz;

        //int id;
        int team;

        unsigned char r,g,b;

        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_position(float x, float y, float z);
        void draw();
};

