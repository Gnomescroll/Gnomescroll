#pragma once

#if DC_CLIENT
#include <agent/client/player_agent.hpp>
#endif

#include <particle/particles.hpp>
#include <particle/billboard_sprite.hpp>
namespace Particle
{


/* These should be pulled from a dat or something */
const int GRENADE_MAX = 1024;
const float GRENADE_BLOCK_DESTROY_RADIUS = 2.0f;
const float GRENADE_DAMAGE_RADIUS = 5.0f;
const int GRENADE_SPLASH_DAMAGE = 100;
const int GRENADE_BLOCK_DAMAGE = 8;
const int GRENADE_SPAWNER_DAMAGE() { return randrange(20,40); }
const int GRENADE_TURRET_DAMAGE() { return randrange(20,40); }
const int GRENADE_MONSTER_BOMB_DAMAGE() { return randrange(50,100); }
const int GRENADE_MONSTER_BOX_DAMAGE() { return randrange(35,70); }

class grenade_StoC;     // forward decl

class Grenade: public ParticleMotion, public BillboardSprite
{
    private:
        int bounce_count;
        void reset();
    public:
        int owner;  // agent_id
        int ttl_max;

        void tick();
        void explode();
        void explode(int multiplier);

        int block_damage(int dist);
        void damage_blocks();
        void damage_blocks(int multiplier);

        #if DC_SERVER
        void broadcast();
        #endif
        
        explicit Grenade(int id);
        explicit Grenade();
        ~Grenade();
};

}   // Particle

namespace Particle
{

#include <common/template/object_list.hpp>
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
            return GRENADE_SPAWNER_DAMAGE();
            
        case OBJECT_TURRET:
            return GRENADE_TURRET_DAMAGE();
            
        case OBJECT_MONSTER_BOMB:
            return GRENADE_MONSTER_BOMB_DAMAGE();
            
        case OBJECT_MONSTER_BOX:
            return GRENADE_MONSTER_BOX_DAMAGE();
            
        default: return 0;
    }
}

}   // Particle
