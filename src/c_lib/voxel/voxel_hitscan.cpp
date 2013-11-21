/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "voxel_hitscan.hpp"

#include <physics/ray_trace/hitscan.hpp>
#include <physics/ray_trace/ray_trace.hpp>

namespace Voxels
{

bool VoxelHitscanList::hitscan(const struct Vec3& pos, const struct Vec3& forward,
                               int skip_id, EntityType skip_type, // skip player agent id
                               float range,
                               Vec3& collision_point, float& distance,
                               class VoxelHitscanTarget& target)
{
    distance = 0;
    collision_point = vec3_init(INT_MAX);
    const float range_sq = range * range;
    Vec3 dest = vec3_init(0);
    VoxelHitscanElement* target_hit = NULL;
    Vec3i voxel_hit = vec3i_init(0);
    float min_dist_sq = map_dim.x * map_dim.x + 10;

    for (int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        VoxelHitscanElement* vhe = hitscan_list[i];
        if (vhe == NULL) continue;
        if (!vhe->vv->hitscan) continue;
        // skip firing agent
        if (vhe->entity_id == skip_id && vhe->entity_type == skip_type)
            continue;

        Vec3 p = quadrant_translate_position(pos, vhe->vv->world_matrix.c);
        float distance_sq = vec3_distance_squared(pos, p);
        if (distance_sq > range_sq)
            continue;

        Vec3 collision;
        float rad_sq;
        if (!sphere_line_distance(pos, forward, p, collision, rad_sq))
            continue;

        float radius = vhe->vv->radius;
        if (rad_sq >= radius*radius)
            continue;

        collision = translate_position(collision);
        Vec3i voxel;
        if (!vhe->vv->hitscan_test(collision, forward, rad_sq, voxel))
            continue;

        min_dist_sq = distance_sq;
        target_hit = vhe;
        dest = collision;
        voxel_hit = voxel;
    }

    if (target_hit != NULL)
    {
        if (min_dist_sq > 0)
            distance = sqrtf(min_dist_sq);
        else
            distance = 0;
        collision_point = dest;
        target.copy_vhe(target_hit);
        target.voxel = voxel_hit;
        return true;
    }
    return false;
}

class VoxelHitscanTarget* VoxelHitscanList::hitscan_all(
    const Vec3& start, const Vec3& end, size_t* n_targets)
{
    *n_targets = 0;
    if (vec3_equal(start, end)) return NULL;

    const size_t MAX_TARGETS = 16;
    static VoxelHitscanTarget targets[MAX_TARGETS];

    struct Vec3 direction = vec3_sub(end, start);
    float max_dist_sq = vec3_length_squared(direction);
    direction = vec3_normalize(direction);
    size_t n = 0;
    for (int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if (n >= MAX_TARGETS) break;

        class VoxelHitscanElement* vhe = hitscan_list[i];
        if (vhe == NULL) continue;
        if (!vhe->vv->hitscan) continue;

        Vec3 p = quadrant_translate_position(start, vhe->vv->world_matrix.c);
        float distance_sq = vec3_distance_squared(start, p);
        if (distance_sq > max_dist_sq)
            continue;

        Vec3 collision;
        float rad_sq;
        if (!sphere_line_distance(start, direction, p, collision, rad_sq))
            continue;

        float radius = vhe->vv->radius;
        if (rad_sq >= radius*radius)
            continue;

        collision = translate_position(collision);
        Vec3i voxel;
        if (!vhe->vv->hitscan_test(collision, direction, rad_sq, voxel))
            continue;

        class VoxelHitscanTarget* target = &targets[n++];
        target->copy_vhe(vhe);
        target->voxel = voxel;
    }

    *n_targets = n;
    if (n == 0) return NULL;
    return targets;
}


bool VoxelHitscanList::point_collision(const Vec3& position, class VoxelHitscanTarget* target)
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
    IF_ASSERT(this->num_elements >= VOXEL_HITSCAN_LIST_SIZE) return;

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
    for (int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
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

VoxelHitscanList::VoxelHitscanList() :
    num_elements(0)
{
    this->hitscan_list = (class VoxelHitscanElement**)calloc(VOXEL_HITSCAN_LIST_SIZE, sizeof(class VoxelHitscanElement*));
}

VoxelHitscanList::~VoxelHitscanList()
{
    free(this->hitscan_list);
}

void VoxelHitscanTarget::copy_vhe(const VoxelHitscanElement* vhe)
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
}   // Voxels
