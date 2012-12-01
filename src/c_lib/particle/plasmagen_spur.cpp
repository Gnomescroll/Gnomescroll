#include "plasmagen_spur.hpp"

#include <physics/common.hpp>

namespace Particle
{

const float PLASMAGEN_SPUR_MASS = 0.1f;

inline void PlasmagenSpur::reset()
{
    this->travelled = 0.0f;
    this->ttl = 0;
    this->scale = PLASMAGEN_SPUR_TEXTURE_SCALE;
    this->verlet.dampening = PLASMAGEN_SPUR_DAMP;
}

PlasmagenSpur::PlasmagenSpur() :
    ParticleMotion(-1, 0,0,0,0,0,0, PLASMAGEN_SPUR_MASS),
    BillboardSprite(),
    travelled(0.0f)
{
    this->reset();
}

void PlasmagenSpur::tick()
{
    float dist = 0.0f;
    class RaytraceData data;
    if (this->verlet.collide_no_gravity(&data, &dist))
    {
        #if DC_SERVER
        t_map::apply_damage_broadcast(
            data.collision_point[0],
            data.collision_point[1],
            data.collision_point[2], 
            t_gen::PLASMAGEN_BLOCK_DAMAGE,
            TMA_PLASMAGEN);
        #endif
        this->travelled = t_gen::PLASMAGEN_BLAST_RADIUS;
    }
    this->travelled += dist;
}

void PlasmagenSpur::die()
{
    #if DC_CLIENT
    // create shrapnel animation where we are
    #endif
}

/* PlasmagenSpur list */

void PlasmagenSpurList::tick()
{
    for (unsigned int i=0; i<num; i++)
    {
        a[i].tick();
        if (a[i].travelled >= t_gen::PLASMAGEN_BLAST_RADIUS)
        {
            a[i].die();
            this->destroy(i);
        }
    }
}

#if DC_CLIENT
void PlasmagenSpurList::draw()
{
    glColor4ub(255,255,255,255);
    for(unsigned int i=0; i<num; i++)
        a[i].draw(a[i].get_position());
}
#endif

}   // Particle
