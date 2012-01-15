#include "voxel_hitscan.hpp"

// TEMPORARY:
#include <c_lib/ray_trace/hitscan.hpp>

int Voxel_hitscan_list::hitscan
(const float _x0, const float _y0, const float _z0, 
const float x1, const float y1, const float z1,
int skip_id, // skip player agent id
float collision_point[3], float *distance, int *metadata)
{
    float x0,y0,z0;
    float x2,y2,z2;

    float t, d;
    float r2;
    float x,y,z;

    float radius;
    struct Voxel_hitscan_element* vhe;

    float min_dist = 10000000.0f; // big #
    int voxel_hit = -1;
    int body_part = -1;
    
    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if(hitscan_list[i] == NULL) continue;
        //if(i == skip_id) continue;
        vhe = hitscan_list[i];

        x2 = vhe->vv->center.x;
        y2 = vhe->vv->center.y;
        z2 = vhe->vv->center.z;
        radius = vhe->vv->radius;
        
        x0 = _x0 - x1;
        y0 = _y0 - y1;
        z0 = _z0 - z1;

        t =  x0*x2 + y0*y2 + z0*z2; // <x0|x2>

        d = t/(x2*x2+y2*y2+z2*z2); //distance to collision

        x = t*x2 - x0;
        y = t*y2 - y0;
        z = t*z2 - z0;
        r2 = x*x+y*y+z*z; //distance squared between v0 and closest point on line to v0

        if( r2 < radius*radius ) {
            printf("BAM! HIT %i!!\n", i);
            if (d > min_dist) continue;
            min_dist = d;
            voxel_hit = i;
            body_part = 0;
            //x,y,z is closest point
            x = t*x2 + x1;
            y = t*y2 + y1;
            z = t*z2 + z1;
        }
    }
    metadata[0] = voxel_hit;
    metadata[1] = body_part;
    *distance = min_dist;
    collision_point[0] = x;
    collision_point[1] = y;
    collision_point[2] = z;
    return Hitscan::HITSCAN_TARGET_AGENT;   // TODO
}

void Voxel_hitscan_list::register_voxel_volume(Voxel_volume* vv, int entity_id, int entity_type)
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
            hitscan_list[i] = new Voxel_hitscan_element;
            num_elements++;
            hitscan_list[i]->vv = vv;
            hitscan_list[i]->entity_id = entity_id;
            hitscan_list[i]->entity_type = entity_type;
                      
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
        if(hitscan_list[i]->vv == vv)
        {
            num_elements--;
            delete hitscan_list[i];
            hitscan_list[i] = NULL;
            printf("Removed voxel volume %i \n", i);
            vv->voxel_hitscan_list = NULL;
            return;
        }
    }

    printf("Voxel_hitscan_list::unregister_voxel_hitscan error, volume was not on list \n");
    vv->id = -1;
    vv->voxel_hitscan_list = NULL;
}
