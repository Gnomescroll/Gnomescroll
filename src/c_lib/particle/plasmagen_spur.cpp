#include "plasmagen_spur.hpp"

#include <physics/common.hpp>
#include <animations/_interface.hpp>

namespace Particle
{

const float PLASMAGEN_SPUR_MASS = 0.1f;

inline void PlasmagenSpur::reset()
{
    this->travelled = 0.0f;
    this->ttl = 0;
    this->scale = PLASMAGEN_SPUR_TEXTURE_SCALE;
    this->verlet.dampening = PLASMAGEN_SPUR_DAMP;
    #if DC_CLIENT
    this->texture_index = PLASMAGEN_SPUR_TEXTURE_ID;
    #endif
}

PlasmagenSpur::PlasmagenSpur(int id) :
    ParticleMotion(id, 0,0,0,0,0,0, PLASMAGEN_SPUR_MASS),
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
        //this->broadcast_death();
        #endif
        this->travelled = t_gen::PLASMAGEN_BLAST_RADIUS;
    }
    this->travelled += dist;
}

void PlasmagenSpur::die()
{
    #if DC_CLIENT
    Animations::play_animation("plasmagen_spur_explode", this->get_position());
    #endif
}

/* Packet */

void plasmagen_explode_StoC::handle()
{
    #if DC_CLIENT
    static const int dir[2] = { -1, 1 };
    static const struct Vec3 vsides[3] = {
        vec3_init(1, 0, 0),
        vec3_init(0, 1, 0),
        vec3_init(0, 0, 1)};
    
    // create spurs to damage nearby blocks
    for (int i=0; i<3; i++)
    for (int j=0; j<2; j++)
    {
        struct Vec3 direction = vec3_scalar_mult(vsides[i], dir[j]);
        struct Vec3 momentum = vec3_scalar_mult(direction, PLASMAGEN_MOMENTUM);
        class PlasmagenSpur* spur = plasmagen_spur_list->create();
        GS_ASSERT(spur != NULL);
        if (spur == NULL) break;
        spur->set_state(this->position, momentum);
    }

    Sound::play_3d_sound("plasmagen_explode", this->position, vec3_init(0,0,0));
    #endif
}


/* PlasmagenSpur list */

void PlasmagenSpurList::tick()
{
    for (size_t i=0; i<this->max; i++)
    {
        if (this->objects[i].id == this->null_id)
            continue;
        this->objects[i].tick();
        if (this->objects[i].travelled >= t_gen::PLASMAGEN_BLAST_RADIUS)
        {
            this->objects[i].die();
            this->destroy(i);
        }
    }
}

#if DC_CLIENT
void PlasmagenSpurList::draw()
{
    glColor4ub(255,255,255,255);
    for(size_t i=0; i<this->max; i++)
        if (this->objects[i].id != this->null_id)
            this->objects[i].draw(this->objects[i].get_position());
}
#endif

}   // Particle
