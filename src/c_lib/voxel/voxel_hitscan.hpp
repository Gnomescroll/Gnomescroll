/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <entity/constants.hpp>
#include <physics/vec3.hpp>

namespace Voxels
{

const int VOXEL_HITSCAN_LIST_SIZE = 1024;

/*
    Create list template later
    add, remove, swap, reorder for iteration
    use instances instead of pointers
*/

class VoxelHitscanElement
{
    public:
        EntityID entity_id;
        EntityType entity_type;
        short part_id;
        class VoxelVolume* vv;

    VoxelHitscanElement() :
        entity_id(NULL_ENTITY), entity_type(NULL_ENTITY_TYPE), part_id(-1), vv(NULL)
    {}
};

class VoxelHitscanTarget: public VoxelHitscanElement
{
    public:
        Vec3i voxel;

    void copy_vhe(const VoxelHitscanElement* vhe);
    void print()
    {
        printf("Target:\n");
        printf("id   %d\n", this->entity_id);
        printf("type %d\n", this->entity_type);
        printf("part %d\n", this->part_id);
        printf("voxel: ");
        vec3i_print(this->voxel);
    }

    VoxelHitscanTarget() :
        VoxelHitscanElement(), voxel(vec3i_init(0))
    {}
};

class VoxelHitscanList
{
    private:
        class VoxelHitscanElement** hitscan_list;

    public:
        int num_elements;

        void register_voxel_volume(class VoxelVolume* vv);
        void unregister_voxel_volume(class VoxelVolume* vv);

    bool hitscan(const Vec3& p, const Vec3& v,
                 int skip_id, EntityType skip_type, float range,
                 Vec3& collision_point, float& distance,
                 class VoxelHitscanTarget& target);

    // Temporary wrapper with range defaulted
    bool hitscan(const Vec3& p, const Vec3& v,
                 int skip_id, EntityType skip_type,
                 Vec3& collision_point, float& distance,
                 class VoxelHitscanTarget& target)
    {
        const float range = 128.0f;
        return this->hitscan(p, v, skip_id, skip_type, range, collision_point,
                             distance, target);
    }

    class VoxelHitscanTarget* hitscan_all(const Vec3& start, const Vec3& end, size_t* n_targets);

    bool point_collision(const Vec3& position, class VoxelHitscanTarget* target);

    VoxelHitscanList();
    ~VoxelHitscanList();
};

}   // Voxels
