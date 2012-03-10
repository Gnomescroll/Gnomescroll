#pragma once

/* These should be pulled from a dat or something */
const int GRENADE_MAX = 1024;
const float GRENADE_BLOCK_DESTROY_RADIUS = 2.0f;
const float GRENADE_DAMAGE_RADIUS = 4.5f;
const int GRENADE_SPLASH_DAMAGE = 100;
const int GRENADE_BLOCK_DAMAGE = 32;
const int GRENADE_SPAWNER_DAMAGE = 20;

using Verlet::VerletParticle;

class grenade_StoC;     // forward decl

class Grenade: public Particle, public BillboardSprite
{
    private:
        int bounce_count;
        void init();
    public:
        int owner;  // agent_id

        void tick();
        void explode();

        int block_damage(int dist);
        void damage_blocks();

        #ifdef DC_SERVER
        void broadcast();
        #endif
        
        explicit Grenade(int id);
        Grenade(int id, float x, float y, float z, float mx, float my, float mz);
        ~Grenade();
};

#include <c_lib/template/object_list.hpp>
class Grenade_list: public Object_list<Grenade, GRENADE_MAX>
{
    private:
        const char* name() { return "Grenade"; }
    public:
        void draw();
        void tick() GNOMESCROLL_API;

        Grenade_list() { print(); }
};
