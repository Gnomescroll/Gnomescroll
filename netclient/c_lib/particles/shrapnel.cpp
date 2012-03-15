#include "shrapnel.hpp"

#include <physics/common.hpp>

const float SHRAPNEL_MASS = 0.2f;

void Shrapnel::init()
{
    this->ttl_max = SHRAPNEL_TTL;
    this->type = SHRAPNEL_TYPE;
    this->texture_index = SHRAPNEL_TEXTURE_ID;
    this->scale = SHRAPNEL_TEXTURE_SCALE;
}

Shrapnel::Shrapnel(int id)
:
Particle(id, 0,0,0,0,0,0, SHRAPNEL_MASS),
BillboardSprite()
{
    this->init();
}

Shrapnel::Shrapnel(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z, mx,my,mz, SHRAPNEL_MASS),
BillboardSprite()
{
    this->init();
}

void Shrapnel::tick()
{
    Vec3 p = this->vp->p;
    Verlet::bounce(this->vp, SHRAPNEL_DAMP);
    ttl++;

    Vec3 v = vec3_sub(this->vp->p, p);
    float len = vec3_length(v);
    normalize_vector(&v);
    
    // check for collision with agent
    // use delta vector to effect blood
    // destroy voxel based on collision with agent
    // can do point collision -- check in voxel volume box, then check if voxel is not empty

    struct Voxel_hitscan_target target;
    float collision_point[3];
    float vox_distance = 100000.0f;
    bool voxel_hit = STATE::voxel_hitscan_list->hitscan(
        p.x, p.y, p.z,
        v.x, v.y, v.z,
        -1, (Object_types)-1,
        collision_point, &vox_distance,
        &target
    );

    if (!voxel_hit) return;
    if (target.entity_type != OBJ_TYPE_AGENT) return;
    if (vox_distance > len) return;
    destroy_object_voxel(target.entity_id, target.entity_type, target.part_id, target.voxel, 3);

}

/* Shrapnel list */

void Shrapnel_list::tick()
{
    for (int i=0; i<n_max; i++)
    {
        if (a[i] == NULL) continue;
        a[i]->tick();
        if (a[i]->ttl >= a[i]->ttl_max)
            destroy(a[i]->id);
    }
}

void Shrapnel_list::draw()
{
#ifdef DC_CLIENT
    if(num == 0) return;
    for(int i=0; i<n_max; i++)
        if (a[i] != NULL)
            a[i]->draw(a[i]->vp->p);
#endif
}

