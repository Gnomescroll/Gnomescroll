#pragma once



#include <physics/common.hpp>


#define SHRAPNEL_MAX 4096
#define SHRAPNEL_TTL 30
#define SHRAPNEL_DAMP 0.5f
#define SHRAPNEL_TYPE 5
#define SHRAPNEL_TEXTURE_ID 5
#define SHRAPNEL_TEXTURE_SCALE 0.15f

class Shrapnel: public Particle
{
    public:
        inline void draw() __attribute((always_inline));
        void tick();
        Shrapnel(int id);
        Shrapnel(int id, float x, float y, float z, float ix, float iy, float iz);
};

#include <c_lib/template/object_list.hpp>

class Shrapnel_list: public Object_list<Shrapnel, SHRAPNEL_MAX>
{
    private:
        const char* name() { return "Shrapnel"; }
    public:
        void draw();
        void tick();
};
    
