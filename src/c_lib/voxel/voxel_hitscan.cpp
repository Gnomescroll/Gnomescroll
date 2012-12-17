#include "voxel_hitscan.hpp"

#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/ray_trace.hpp>

bool VoxelHitscanList::hitscan(
    const float x0, const float y0, const float z0,     // source
    const float x1, const float y1, const float z1,     // direction
    int skip_id, EntityType skip_type, // skip player agent id
    float collision_point[3], float *distance,
    class VoxelHitscanTarget* target)
{
    float x2,y2,z2;

    float r2 = 100000.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float radius;
    class VoxelHitscanElement* vhe;
    class VoxelHitscanElement* target_hit;
    target_hit = NULL;
    int voxel[3];

    float dist;
    float min_dist = 1000000.0f;
    float max_dist = 256.0f;
    float tpos[3] = {0};

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
            if (!vhe->vv->hitscan_test(
                tpos[0], tpos[1], tpos[2], x1,y1,z1, r2, voxel)) continue; //test for voxel hit
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

class VoxelHitscanTarget* VoxelHitscanList::hitscan_all(
    struct Vec3 start, struct Vec3 end, size_t* n_targets)
{
    *n_targets = 0;
    if (vec3_equal(start, end)) return NULL;

    const size_t MAX_TARGETS = 16;
    static VoxelHitscanTarget targets[MAX_TARGETS];

    struct Vec3 direction = vec3_sub(end, start);
    float max_dist = vec3_length(direction);
    normalize_vector(&direction);
    size_t n = 0;
    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if (n >= MAX_TARGETS) break;
        class VoxelHitscanElement* vhe = hitscan_list[i];
        if (vhe == NULL) continue;
        if (!vhe->vv->hitscan) continue;

        struct Vec3 p = quadrant_translate_position(start, vhe->vv->world_matrix.c);
        float tpos[3] = {0};
        float r2 = 0.0f;
        float radius = vhe->vv->radius;
        float dist = sphere_line_distance(
            start.x, start.y, start.z,
            direction.x, direction.y, direction.z, 
            p.x, p.y, p.z, tpos, &r2);

        if (dist < 0.0f || dist > max_dist) continue;
        if (r2 < radius*radius)
        {
            tpos[0] = translate_point(tpos[0]);
            tpos[1] = translate_point(tpos[1]);
            // test for voxel hit
            int voxel[3] = {0};
            if (!vhe->vv->hitscan_test(
                tpos[0], tpos[1], tpos[2],
                direction.x, direction.y, direction.z, r2, voxel)) continue;

            class VoxelHitscanTarget* target = &targets[n++];
            target->copy_vhe(vhe);
            target->copy_voxel(voxel);
        }
    }

    *n_targets = n;
    if (n == 0) return NULL;
    return targets;
}


bool VoxelHitscanList::point_collision(struct Vec3 position, class VoxelHitscanTarget* target)
{
    for (int i=0; i<VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        class VoxelHitscanElement* vhe = this->hitscan_list[i];
        if (vhe == NULL) continue;
        if (vhe->vv->point_collision_test(position, target->voxel))
        {
            target->copy_vhe(vhe);
            return true;
        }
    }
    return false;
}

void VoxelHitscanList::register_voxel_volume(class VoxelVolume* vv)
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

void VoxelHitscanList::unregister_voxel_volume(class VoxelVolume* vv)
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

VoxelHitscanList::VoxelHitscanList() : num_elements(0)
{
    this->hitscan_list = (class VoxelHitscanElement**)calloc(VOXEL_HITSCAN_LIST_SIZE, sizeof(class VoxelHitscanElement*));
}

VoxelHitscanList::~VoxelHitscanList()
{
    if (this->hitscan_list != NULL) free(this->hitscan_list);
}

void VoxelHitscanTarget::copy_vhe(VoxelHitscanElement* vhe)
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

void VoxelHitscanTarget::copy_voxel(int voxel[3])
{
    for (int i=0; i<3; i++) voxel[i] = this->voxel[i];
}
