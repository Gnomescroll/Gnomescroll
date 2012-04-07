#pragma once

#include <c_lib/components/component.hpp>
#include <c_lib/voxel/voxel_model.hpp>

class VoxelProperties /*: public ComponentProperties*/
{
    public:
        Voxel_model* vox;
        VoxDat* vox_dat;
        bool init_hitscan;
        bool init_draw;
        bool frozen_vox;

    VoxelProperties()
    :
    vox(NULL), vox_dat(NULL),
    init_hitscan(false), init_draw(false), frozen_vox(false)
    {}

    ~VoxelProperties()
    {
        if (this->vox != NULL)
            delete this->vox;
    }
};

class VoxelComponent
{
    public:
        VoxelProperties voxel_properties;

    VoxelComponent() {}
};
