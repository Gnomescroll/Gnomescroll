#include "grenade_shrapnel.hpp"

#include <physics/common.hpp>
#include <c_lib/agent/net_agent.hpp>

const float GRENADE_SHRAPNEL_MASS = 1.0f;

void Grenade_shrapnel::init()
{
    this->ttl_max = GRENADE_SHRAPNEL_TTL;
    this->type = GRENADE_SHRAPNEL_TYPE;
}

Grenade_shrapnel::Grenade_shrapnel(int id)
:
Particle(id, 0,0,0,0,0,0, GRENADE_SHRAPNEL_MASS)
{
    this->init();
}

Grenade_shrapnel::Grenade_shrapnel(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, GRENADE_SHRAPNEL_MASS)
{
    this->init();
}

void Grenade_shrapnel::tick()
{
    const float base_dmg = 55;
    const int voxel_blast_radius = 3;
    Voxel_hitscan_target target;
    bool voxel_hit;
    int j=0;
    for (int i=this->ttl; i<=this->ttl_max; i++)
    {
        if (j == 10)
        {
            this->ttl += j;
            return;
        }
        j++;
        voxel_hit = ServerState::voxel_hitscan_list->point_collision(this->vp->p, &target);
        Verlet::bounce(this->vp, GRENADE_SHRAPNEL_DAMP);

        if (!voxel_hit) continue;
        if (target.entity_type != OBJ_TYPE_AGENT)
        {
            this->ttl = this->ttl_max;
            return;   // collided with a model we arent handling
        }

        Agent_state* a = ServerState::agent_list->get(target.entity_id);
        if (a == NULL)
        {
            this->ttl = this->ttl_max;
            return;
        }

        destroy_object_voxel(target.entity_id, target.entity_type, target.part_id, target.voxel, voxel_blast_radius);
        destroy_voxel_StoC msg;
        msg.x = target.voxel[0];
        msg.y = target.voxel[1];
        msg.z = target.voxel[2];
        msg.entity_id = target.entity_id;
        msg.entity_type = target.entity_type;
        msg.entity_part = target.part_id;
        msg.radius = voxel_blast_radius;
        msg.broadcast();

        float dmg = base_dmg * ((float)(this->ttl+i)/((float)ttl_max+1));
        a->status.apply_damage(dmg, this->owner, OBJ_TYPE_GRENADE);
        
        this->ttl = this->ttl_max;
        return;
    }
}

/* Grenade_shrapnel list */

void Grenade_shrapnel_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}
