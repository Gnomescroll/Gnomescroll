#pragma once

#define VOXEL_HITSCAN_LIST_SIZE 1024

/*
    Create list template later
    add, remove, swap, reorder for iteration
    use instances instead of pointers
*/

/*
    list, add, remove, iterate
*/

struct Voxel_hitscan_element
{
    short entity_id;
    short entity_type;
    Voxel_volume* vv;
};

class Voxel_hitscan_list
{
    private:
    Voxel_hitscan_element** hitscan_list;
    public:
    int num_elements;

    void register_voxel_volume(Voxel_volume* vv, int entity_id, int entity_type);
    void unregister_voxel_volume(Voxel_volume* vv);

    Voxel_hitscan_list()
    :
    num_elements(0)
    {
        hitscan_list = new Voxel_hitscan_element*[VOXEL_HITSCAN_LIST_SIZE];
        for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++) hitscan_list[i] = NULL;
    }
};
