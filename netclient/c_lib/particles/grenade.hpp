#pragma once

#include <physics/common.h>


/* These should be pulled from a dat or something */
#define GRENADE_MAX 1024
#define GRENADE_TTL 100
#define GRENADE_DAMP 0.3f
#define GRENADE_TYPE 1
#define GRENADE_TEXTURE_ID 5
#define GRENADE_TEXTURE_SCALE 1.0f

#define GRENADE_BLOCK_DESTROY_RADIUS 2
#define GRENADE_AGENT_DAMAGE_RADIUS 3.0f
#define GRENADE_SPLASH_DAMAGE 20
#define GRENADE_BLOCK_DAMAGE 32

class Grenade {
    public:
        struct Particle2 particle;
        int owner;  // agent_id
        void draw();
        void tick();
        void set_ttl(int ttl);

        void explode();

        int block_damage(int dist);
        void damage_blocks();
        
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
