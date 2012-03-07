#pragma once

#include <physics/common.hpp>

#define CSPRAY_MAX 4096
#define CSPRAY_TTL 1200
#define CSPRAY_CEMENT_BLOCK_TYPE 2
#define CSPRAY_DAMP 0.0f
#define CSPRAY_TYPE 1
#define CSPRAY_TEXTURE_ID 21

class Cspray: public EventParticle
{
    public:
        int active;
        int stopped;

        void draw();
        void tick();

        explicit Cspray(int id);
        Cspray(int id, float x, float y, float z, float mx, float my, float mz);
};


#include <c_lib/template/object_list.hpp>

class Cspray_list: public Object_list<Cspray,CSPRAY_MAX>
{
    private:
        const char* name() { return "Cspray"; }
    public:
        void draw();
        void tick();

        Cspray_list() { print(); }
};
