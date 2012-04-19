#pragma once

#include <c_lib/voxel/voxel_model.hpp>

namespace Components
{

class VoxelComponent
{
    public:
        Voxel_model* vox;
        VoxDat* vox_dat;
        bool init_hitscan;
        bool init_draw;
        bool frozen_vox;

    Vec3 get_center()
    {
        return this->vox->get_part(0)->get_center();
    }
    
    Vec3 get_center(int part)
    {
        return this->vox->get_part(part)->get_center();
    }
    
    ~VoxelComponent()
    {
        if (this->vox != NULL) delete this->vox;
    }

    VoxelComponent()
    : Component(COMPONENT_VOXEL_MODEL, COMPONENT_INTERFACE_VOXEL_MODEL),
    vox(NULL), vox_dat(NULL),
    init_hitscan(false), init_draw(false), frozen_vox(false)
    {}
};


} // Objects

