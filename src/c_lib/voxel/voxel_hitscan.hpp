#pragma once

#include <entity/constants.hpp>
#include <physics/vec3.hpp>

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
        unsigned int voxel[3];

        void copy_vhe(VoxelHitscanElement* vhe);
        void copy_voxel(int voxel[3]);
        void print()
        {
            printf("Target:\n");
            printf("id   %d\n", entity_id);
            printf("type %d\n", entity_type);
            printf("part %d\n", part_id);
            printf("voxel: %d,%d,%d\n", voxel[0], voxel[1], voxel[2]);
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
    bool hitscan(
        const float x0, const float y0, const float z0, // source
        const float x1, const float y1, const float z1, // direction
        int skip_id, EntityType skip_type,
        float collision_point[3], float *distance,
        class VoxelHitscanTarget* target);
    
    class VoxelHitscanTarget* hitscan_all(struct Vec3 start, struct Vec3 end, size_t* n_targets);

    bool point_collision(struct Vec3 position, class VoxelHitscanTarget* target);
    
    VoxelHitscanList();
    ~VoxelHitscanList();
};
