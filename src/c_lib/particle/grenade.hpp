#pragma once

#if DC_CLIENT
#include <agent/client/player_agent.hpp>
#endif

#include <particle/particles.hpp>
#include <particle/billboard_sprite.hpp>

namespace Particle
{


/* These should be pulled from a dat or something */
const unsigned int MAX_GRENADES = 1024;
const int GRENADE_BLOCK_DESTROY_RADIUS = 2;
const float GRENADE_DAMAGE_RADIUS = 5.0f;
const int GRENADE_SPLASH_DAMAGE = 40;
const int GRENADE_BLOCK_DAMAGE = 8;
int GRENADE_SPAWNER_DAMAGE() { return randrange(20,40); }
int GRENADE_TURRET_DAMAGE() { return randrange(20,40); }
int GRENADE_MONSTER_BOMB_DAMAGE() { return randrange(50,100); }
int GRENADE_MONSTER_BOX_DAMAGE() { return randrange(35,70); }

class grenade_StoC;     // forward decl

class Grenade: public ParticleMotion, public BillboardSprite
{
    private:
        int bounce_count;
        void reset();
        inline int block_damage(int dist);
    public:
        AgentID owner;
        int ttl_max;

        void tick();
        void explode() { this->explode(1); }
        void explode(int multiplier);

        void damage_blocks() { this->damage_blocks(1); }
        void damage_blocks(int multiplier);

        #if DC_SERVER
        void broadcast();
        #endif

        explicit Grenade(int id);
        ~Grenade();
};

}   // Particle

namespace Particle
{

#include <common/template/object_list.hpp>

class GrenadeList: public ObjectList<Grenade>
{
    private:
        const char* name() { return "Grenade"; }
    public:
        void draw();
        void tick();

        explicit GrenadeList(size_t capacity) :
            ObjectList<Grenade>(capacity)
        {
        }
};

int get_grenade_damage(EntityType type)
{
    switch (type)
    {
        case OBJECT_AGENT_SPAWNER:
        case OBJECT_MONSTER_SPAWNER:
            return GRENADE_SPAWNER_DAMAGE();

        case OBJECT_TURRET:
            return GRENADE_TURRET_DAMAGE();

        case OBJECT_MONSTER_BOMB:
        case OBJECT_MONSTER_SLIME:
            return GRENADE_MONSTER_BOMB_DAMAGE();

        case OBJECT_MONSTER_BOX:
            return GRENADE_MONSTER_BOX_DAMAGE();

        case OBJECT_GRENADE:
        case OBJECT_AGENT:
        case OBJECT_CANNONBALL:
        case OBJECT_PLASMAGEN:
        case OBJECT_ENERGY_CORE:
        case OBJECT_BASE:
        case OBJECT_DESTINATION:
        case OBJECT_NONE:
            return 0;
        default:
            GS_ASSERT(false);
            return 0;
    }
}

}   // Particle
