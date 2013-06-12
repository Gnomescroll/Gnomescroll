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
    void explode(int multiplier=1);
    void damage_blocks(int multiplier=1);

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
    const char* name()
    {
        return "Grenade";
    }

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
    return randrange(10, 30);
}

}   // Particle
