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

/* These should be pulled from a dat or something */
#define GRENADE_MAX 1024
#define GRENADE_TTL 100
#define GRENADE_DAMP 0.5f
#define GRENADE_TYPE 1
#define GRENADE_TEXTURE_ID 5

/* Grenade used to be just a Particle2, and effectively is,
 * but fits in the Object_list template as a class */
class Grenade {
    public:
        struct Particle2 particle;

        void draw();
        void tick();

        Grenade(int id);
        Grenade(int id, float x, float y, float z, float vx, float vy, float vz);
};

#include <c_lib/template/object_list.hpp>

class Grenade_list: public Object_list<Grenade, GRENADE_MAX>
{
    private:
        const char* name() { return "Grenade"; }
    public:
        void draw();
        void tick();
};
