#pragma once

#include <common/enum_types.hpp>

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
    short part_id;
    Voxel_volume* vv;
};

class Voxel_hitscan_target
{
    public:
        short entity_id;
        short entity_type;
        short part_id;
        Voxel_volume* vv;
        int voxel[3];

        void copy_vhe(Voxel_hitscan_element* vhe);
        void copy_voxel(int voxel[3]);
};

class Voxel_hitscan_list
{
    private:
    Voxel_hitscan_element** hitscan_list;
    
    public:
    int num_elements;

    //void register_voxel_volume(Voxel_volume* vv, int entity_id, int entity_type, int part_id);
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
    
    Voxel_hitscan_list();
    ~Voxel_hitscan_list();
};
