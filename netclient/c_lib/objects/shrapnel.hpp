#pragma once

#include <compat.h>
#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <physics/common.h>
#include <ray_trace/ray_trace.h>
#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#ifdef DC_CLIENT
    #include <texture_loader.h>
#endif


#define SHRAPNEL_MAX 4092
#define SHRAPNEL_TTL 30
#define SHRAPNEL_DAMP 0.5f
#define SHRAPNEL_TYPE 5
#define SHRAPNEL_TEXTURE_ID 5
#define SHRAPNEL_TEXTURE_SCALE 0.1f

class Shrapnel {
    public:
        Particle2 particle;

        void draw();
        void tick();
        Shrapnel(int id);
        Shrapnel(int id, float x, float y, float z, float vx, float vy, float vz);
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
    
