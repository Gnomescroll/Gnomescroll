#pragma once

#if DC_CLIENT
#include <c_lib/agent/client/player_agent.hpp>
#endif

#include <c_lib/particles/particles.hpp>
#include <c_lib/particles/billboard_sprite.hpp>
namespace Particles
{


/* These should be pulled from a dat or something */
const int GRENADE_MAX = 1024;
const float GRENADE_BLOCK_DESTROY_RADIUS = 2.0f;
//const float GRENADE_DAMAGE_RADIUS = 4.5f;
const float GRENADE_DAMAGE_RADIUS = 4.5f;
const int GRENADE_SPLASH_DAMAGE = 100;
const int GRENADE_BLOCK_DAMAGE = 8;
const int GRENADE_SPAWNER_DAMAGE = 25;
const int GRENADE_TURRET_DAMAGE = 25;
const int GRENADE_SLIME_DAMAGE = 100;

class grenade_StoC;     // forward decl

class Grenade: public ParticleMotion, public BillboardSprite
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

        #if DC_SERVER
        void broadcast();
        #endif
        
        explicit Grenade(int id);
        explicit Grenade();
        ~Grenade();
};

}

namespace Particles
{

#include <c_lib/common/template/object_list.hpp>
class Grenade_list: public Object_list<Grenade, GRENADE_MAX>
{
    private:
        const char* name() { return "Grenade"; }
    public:
        void draw();
        void tick();

        Grenade_list() { print_list((char*)this->name(), this); }
};
 
int get_grenade_damage(ObjectType type)
{
    switch (type)
    {
        case OBJECT_AGENT_SPAWNER:
        case OBJECT_MONSTER_SPAWNER:
            return GRENADE_SPAWNER_DAMAGE;
        case OBJECT_TURRET:
            return GRENADE_TURRET_DAMAGE;
        case OBJECT_MONSTER_BOMB:
            return GRENADE_SLIME_DAMAGE;
        case OBJECT_MONSTER_BOX:
            return 1000;
        default: return 0;
    }
}

}
