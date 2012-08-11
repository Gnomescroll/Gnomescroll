#include "grenade_shrapnel.hpp"

#include <physics/common.hpp>
#include <agent/net_agent.hpp>

namespace Particle
{


const float GRENADE_SHRAPNEL_MASS = 1.0f;

inline void Grenade_shrapnel::init()
{
    this->ttl = GRENADE_SHRAPNEL_TTL;
    this->type = GRENADE_SHRAPNEL_TYPE;
    this->verlet.dampening = GRENADE_SHRAPNEL_DAMP;
}

Grenade_shrapnel::Grenade_shrapnel()
:
ParticleMotion(-1, 0,0,0,0,0,0, GRENADE_SHRAPNEL_MASS)
{
    this->init();
}

void Grenade_shrapnel::tick()
{
    const float base_dmg = 55;
    //const int voxel_blast_radius = 3;
    Voxel_hitscan_target target;
    bool voxel_hit;
    int j=0;
    for (int i=this->ttl; i>0; i--)
    {
        if (j == 10)
        {
            this->ttl -= j;
            return;
        }
        j++;
        voxel_hit = ServerState::voxel_hitscan_list->point_collision(this->get_position(), &target);
        this->verlet.bounce();

        if (!voxel_hit) continue;
        if (target.entity_type != OBJECT_AGENT)
        {
            this->ttl = 0;
            return;   // collided with a model we arent handling
        }

        Agent_state* a = ServerState::agent_list->get(target.entity_id);
        if (a == NULL)
        {
            this->ttl = 0;
            return;
        }

        //destroy_object_voxel(target.entity_id, target.entity_type, target.part_id, target.voxel, voxel_blast_radius);
        //destroy_voxel_StoC msg;
        //msg.x = target.voxel[0];
        //msg.y = target.voxel[1];
        //msg.z = target.voxel[2];
        //msg.entity_id = target.entity_id;
        //msg.entity_type = target.entity_type;
        //msg.entity_part = target.part_id;
        //msg.radius = voxel_blast_radius;
        //msg.broadcast();

        float dmg = base_dmg * ((float)(this->ttl-i)/((float)GRENADE_SHRAPNEL_TTL+1));
        if (dmg > 0) a->status.apply_damage(dmg, this->owner, OBJECT_GRENADE);
        
        this->ttl = 0;
        return;
    }
}

}

/* Grenade_shrapnel list */

namespace Particle
{

void Grenade_shrapnel_list::tick()
{
    for (unsigned int i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) destroy(i);
    }
}

}
