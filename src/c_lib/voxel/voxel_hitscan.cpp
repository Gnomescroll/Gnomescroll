#include "voxel_hitscan.hpp"

#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/ray_trace.hpp>

bool Voxel_hitscan_list::hitscan(
    const float x0, const float y0, const float z0,     // source
    const float x1, const float y1, const float z1,     // direction
    int skip_id, ObjectType skip_type, // skip player agent id
    float collision_point[3], float *distance,
    Voxel_hitscan_target* target
)
{
    float x2,y2,z2;

    float r2;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float radius;
    class Voxel_hitscan_element* vhe;
    class Voxel_hitscan_element* target_hit;
    target_hit = NULL;
    int voxel[3];

    float dist;
    float min_dist = 1000000.0f;
    float max_dist = 10000.0f;
    float tpos[3];

    Vec3 source = vec3_init(x0,y0,z0);
    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        vhe = hitscan_list[i];
        if (vhe == NULL) continue;
        if (!vhe->vv->hitscan) continue;
        // skip firing agent
        if (vhe->entity_id == skip_id && vhe->entity_type == skip_type) continue;

        Vec3 p = quadrant_translate_position(source, vhe->vv->world_matrix.c);
        float* _tmp = p.f;
        x2 = _tmp[0];
        y2 = _tmp[1];
        z2 = _tmp[2];
        radius = vhe->vv->radius;
        dist = sphere_line_distance(x0, y0, z0, x1,y1,z1, x2,y2,z2, tpos, &r2);

        if (dist < 0.0f || dist > max_dist) continue;
        if (r2 < radius*radius)
        {
            if (dist > min_dist) continue;
            tpos[0] = translate_point(tpos[0]);
            tpos[1] = translate_point(tpos[1]);
            if (!vhe->vv->hitscan_test(tpos[0], tpos[1], tpos[2], x1,y1,z1, r2, voxel)) continue; //test for voxel hit
            min_dist = dist;
            target_hit = vhe;
            x = tpos[0];
            y = tpos[1];
            z = tpos[2];
        }
    }

    if (target_hit != NULL) 
    {
        *distance = min_dist;
        collision_point[0] = x;
        collision_point[1] = y;
        collision_point[2] = z;
        target->copy_vhe(target_hit);
        target->copy_voxel(voxel);
        return true;
    }
    return false;
}

bool Voxel_hitscan_list::point_collision(Vec3 position, Voxel_hitscan_target* target)
{
    class Voxel_hitscan_element* vhe;
    for (int i=0; i<VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        vhe = this->hitscan_list[i];
        if (vhe == NULL) continue;
        if (vhe->vv->point_collision_test(position, target->voxel))
        {
            target->copy_vhe(vhe);
            return true;
        }
    }
    return false;
}

void Voxel_hitscan_list::register_voxel_volume(Voxel_volume* vv)
{
    GS_ASSERT(this->num_elements < VOXEL_HITSCAN_LIST_SIZE);
    if (this->num_elements >= VOXEL_HITSCAN_LIST_SIZE) return;
    
    for (int i=0; i<VOXEL_HITSCAN_LIST_SIZE; i++)
        if (hitscan_list[i] == NULL)
        {
            this->hitscan_list[i] = &(vv->vhe);
            this->num_elements++;
            vv->voxel_hitscan_list = this;
            return;
        }
    GS_ASSERT(false);
}

void Voxel_hitscan_list::unregister_voxel_volume(Voxel_volume* vv)
{
    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
        if (this->hitscan_list[i] != NULL && this->hitscan_list[i] == &(vv->vhe))
        {
            this->num_elements--;
            this->hitscan_list[i] = NULL;
            //printf("Removed voxel volume %i from hitscan list\n", i);
            vv->voxel_hitscan_list = NULL;
            return;
        }
    GS_ASSERT(false);
    vv->voxel_hitscan_list = NULL;
}

Voxel_hitscan_list::Voxel_hitscan_list() : num_elements(0)
{
    hitscan_list = (class Voxel_hitscan_element**)calloc(VOXEL_HITSCAN_LIST_SIZE, sizeof(class Voxel_hitscan_element*));
}

Voxel_hitscan_list::~Voxel_hitscan_list()
{
    if (this->hitscan_list != NULL) free(this->hitscan_list);
    this->hitscan_list = NULL;
}

void Voxel_hitscan_target::copy_vhe(Voxel_hitscan_element* vhe)
{
    GS_ASSERT(vhe->entity_id >= 0);
    GS_ASSERT(vhe->entity_type >= 0);
    GS_ASSERT(vhe->part_id >= 0);
    GS_ASSERT(vhe->vv != NULL);
    this->entity_id = vhe->entity_id;
    this->entity_type = vhe->entity_type;
    this->part_id = vhe->part_id;
    this->vv = vhe->vv;
}

void Voxel_hitscan_target::copy_voxel(int voxel[3])
{
    for (int i=0; i<3; i++) voxel[i] = this->voxel[i];
}

