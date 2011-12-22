#pragma once

#include <physics/common.h>

#define CSPRAY_MAX 4096
#define CSPRAY_TTL 1200
#define CSPRAY_CEMENT_BLOCK_TYPE 2
#define CSPRAY_DAMP 0.0f
#define CSPRAY_TYPE 1
#define CSPRAY_TEXTURE_ID 21

class Cspray {
    public:
        struct Particle2 particle;
        int active;
        int stopped;

        void draw();
        void tick();

        Cspray(int _id);
        Cspray(int _id, float x, float y, float z, float vx, float vy, float vz);
};


#include <c_lib/template/object_list.hpp>

class Cspray_list: public Object_list<Cspray,CSPRAY_MAX>
{
    private:
        const char* name() { return "Cspray"; }
    public:
        void draw();
        //void draw(int);
        void tick();
};
