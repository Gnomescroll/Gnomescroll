#pragma once

#define VOXEL_HITSCAN_LIST_SIZE 1024

/*
    list, add, remove, iterate
*/

class Voxel_hitscan_list
{
    private:
    Voxel_volume** hitscan_list;
    public:
    int num_elements;

    void register_voxel_volume(Voxel_volume* vv);
    void unregister_voxel_volume(Voxel_volume* vv);

    Voxel_hitscan_list()
    :
    num_elements(0)
    {
        hitscan_list = new Voxel_volume*[VOXEL_HITSCAN_LIST_SIZE];
        for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++) hitscan_list[i] = NULL;
    }
};
