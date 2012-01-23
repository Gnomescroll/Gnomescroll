#include "voxel_hitscan.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/ray_trace/ray_trace.h>

bool Voxel_hitscan_list::hitscan
(const float x0, const float y0, const float z0, 
const float x1, const float y1, const float z1,
int skip_id, Object_types skip_type, // skip player agent id
float collision_point[3], float *distance,
struct Voxel_hitscan_element* target)
{
    float x2,y2,z2;

    float r2;
    float x,y,z;

    float radius;
    struct Voxel_hitscan_element* vhe;
    struct Voxel_hitscan_element* target_hit;
    target_hit = NULL;

    float dist;
    float min_dist = 1000000.0f;
    float max_dist = 10000.0f;
    float tpos[3];

    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if(hitscan_list[i] == NULL) continue;
        if (!hitscan_list[i]->vv->hitscan) continue;    // shouldnt be registered anyway, but if bool hitscan wants to be toggled, leave it
        vhe = hitscan_list[i];

        // skip firing agent
        if(vhe->entity_id == skip_id && vhe->entity_type == skip_type) continue;

        x2 = vhe->vv->center.x;
        y2 = vhe->vv->center.y;
        z2 = vhe->vv->center.z;
        radius = vhe->vv->radius;

        dist = sphere_line_distance(x0, y0, z0, x1,y1,z1, x2,y2,z2, tpos, &r2);
        if (dist < 0.0f || dist > max_dist) continue;

        if( r2 < radius*radius ) {
            if (dist > min_dist) continue;
            min_dist = dist;
            x = tpos[0];
            y = tpos[1];
            z = tpos[2];
            target_hit = vhe;
        }
    }

    *distance = min_dist;
    collision_point[0] = x;
    collision_point[1] = y;
    collision_point[2] = z;

    if (target_hit != NULL) {
        *target = *target_hit;
        return true;
    }
    return false;
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
