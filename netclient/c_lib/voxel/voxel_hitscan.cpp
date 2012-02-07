#include "voxel_hitscan.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/ray_trace/ray_trace.h>

bool Voxel_hitscan_list::hitscan(
const float x0, const float y0, const float z0, 
const float x1, const float y1, const float z1,
int skip_id, Object_types skip_type, // skip player agent id
float collision_point[3], float *distance,
Voxel_hitscan_target* target
)
{
    float x2,y2,z2;

    float r2;
    float x,y,z;

    float radius;
    struct Voxel_hitscan_element* vhe;
    struct Voxel_hitscan_element* target_hit;
    target_hit = NULL;

    int voxel[3];
    int* voxel_hit;

    float dist;
    float min_dist = 1000000.0f;
    float max_dist = 10000.0f;
    float tpos[3];

    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if (hitscan_list[i] == NULL) continue;
        if (!hitscan_list[i]->vv->hitscan) {
            continue;
        }
        vhe = hitscan_list[i];

        // skip firing agent
        if(vhe->entity_id == skip_id && vhe->entity_type == skip_type) continue;
        
        float* _tmp = vhe->vv->world_matrix.v[3].f;
        x2 = _tmp[0];
        y2 = _tmp[1];
        z2 = _tmp[2];
        radius = vhe->vv->radius;

        dist = sphere_line_distance(x0, y0, z0, x1,y1,z1, x2,y2,z2, tpos, &r2);
        if (dist < 0.0f || dist > max_dist) continue; //check this

        if( r2 < radius*radius ) {
            if (dist > min_dist) continue;  //check this
            if (!vhe->vv->hitscan_test(tpos[0],tpos[1],tpos[2], x1,y1,z1, r2, voxel)) continue; //test for voxel hit
            min_dist = dist;
            x = tpos[0];
            y = tpos[1];
            z = tpos[2];
            voxel_hit = voxel;
            target_hit = vhe;
        }
    }

    if (target_hit != NULL) 
    {
        *distance = min_dist;
        collision_point[0] = x;
        collision_point[1] = y;
        collision_point[2] = z;
        target->copy_vhe(target_hit);
        target->copy_voxel(voxel_hit);
        return true;
    }
    return false;
}

void Voxel_hitscan_target::copy_vhe(Voxel_hitscan_element* vhe)
{
    this->entity_id = vhe->entity_id;
    this->entity_type = vhe->entity_type;
    this->part_id = vhe->part_id;
    this->vv = vhe->vv;
}

void Voxel_hitscan_target::copy_voxel(int voxel[3])
{
    for (int i=0; i<3; i++)
    {
        this->voxel[i] = voxel[i];
    }
}

void Voxel_hitscan_list::register_voxel_volume(Voxel_volume* vv)
{
    if(num_elements >= VOXEL_HITSCAN_LIST_SIZE)
    {
        printf("Voxel_hitscan_list Error: number of voxel models exceeds VOXEL_HITSCAN_LIST_SIZE \n");
        return;
    }
    int i;
    for(i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if(hitscan_list[i] == NULL)
        {
            hitscan_list[i] = &(vv->vhe);
            num_elements++;
                      
            vv->voxel_hitscan_list = this;
            break;
        }
    }
    if (i == VOXEL_HITSCAN_LIST_SIZE) printf("WARNING: register_voxel_hitscan - no space available\n");

}

void Voxel_hitscan_list::unregister_voxel_volume(Voxel_volume* vv)
{
    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if (hitscan_list[i] == NULL) continue;
        if(hitscan_list[i]->vv == vv)
        {
            num_elements--;
            hitscan_list[i] = NULL;
            //printf("Removed voxel volume %i from hitscan list\n", i);
            vv->voxel_hitscan_list = NULL;
            return;
        }
    }

    //printf("Voxel_hitscan_list::unregister_voxel_hitscan error, volume was not on list \n");
    vv->id = -1;
    vv->voxel_hitscan_list = NULL;
}

Voxel_hitscan_list::Voxel_hitscan_list()
:
num_elements(0)
{
    hitscan_list = new Voxel_hitscan_element*[VOXEL_HITSCAN_LIST_SIZE];
    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++) hitscan_list[i] = NULL;
}
