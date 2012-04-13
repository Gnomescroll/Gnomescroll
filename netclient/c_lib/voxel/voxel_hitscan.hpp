#pragma once

#include <common/enum_types.hpp>
#include <c_lib/physics/vec3.hpp>

#define VOXEL_HITSCAN_LIST_SIZE 1024

/*
    Create list template later
    add, remove, swap, reorder for iteration
    use instances instead of pointers
*/

/*
    list, add, remove, iterate
*/

class Voxel_volume; // forward decl

struct Voxel_hitscan_element
{
    short entity_id;
    short entity_type;
    short entity_subtype;
    short part_id;
    Voxel_volume* vv;
};

class Voxel_hitscan_target: public Voxel_hitscan_element
{
    public:
        int voxel[3];

        void copy_vhe(Voxel_hitscan_element* vhe);
        void copy_voxel(int voxel[3]);
        void print()
        {
            printf("Target:\n");
            printf("id   %d\n", entity_id);
            printf("type %d\n", entity_type);
            printf("subtype %d\n", entity_subtype);
            printf("part %d\n", part_id);
            printf("voxel: %d,%d,%d\n", voxel[0], voxel[1], voxel[2]);
        }
};

class Voxel_hitscan_list
{
    private:
    Voxel_hitscan_element** hitscan_list;
    
    public:
    int num_elements;

    void register_voxel_volume(Voxel_volume* vv);
    void unregister_voxel_volume(Voxel_volume* vv);

    //pass in x,y,z fire point and direction of projectile
    bool hitscan(
        const float x0, const float y0, const float z0,
        const float x1, const float y1, const float z1,
        int skip_id, Object_types skip_type,
        float collision_point[3], float *distance,
        Voxel_hitscan_target* target
    );

    bool point_collision(Vec3 position, Voxel_hitscan_target* target);
    
    Voxel_hitscan_list();
    ~Voxel_hitscan_list();
};
