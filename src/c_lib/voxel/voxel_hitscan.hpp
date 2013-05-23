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
        short entity_id;
        short entity_type;
        short part_id;
        class VoxelVolume* vv;

    VoxelHitscanElement() :
        entity_id(-1), entity_type(-1), part_id(-1), vv(NULL)
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
};

class VoxelHitscanList
{
    private:
        class VoxelHitscanElement** hitscan_list;

    public:
        int num_elements;

        void register_voxel_volume(class VoxelVolume* vv);
        void unregister_voxel_volume(class VoxelVolume* vv);

    //pass in x,y,z fire point and direction of projectile
    bool hitscan(const Vec3& p, const Vec3& v,
                 int skip_id, EntityType skip_type,
                 Vec3& collision_point, float& distance,
                 class VoxelHitscanTarget& target);

    class VoxelHitscanTarget* hitscan_all(const Vec3& start, const Vec3& end, size_t* n_targets);

    bool point_collision(const Vec3& position, class VoxelHitscanTarget* target);

    VoxelHitscanList();
    ~VoxelHitscanList();
};

}   // Voxels
