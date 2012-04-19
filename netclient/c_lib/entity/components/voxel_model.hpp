#pragma once

#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class VoxelModelComponent: public Component
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
    
    ~VoxelModelComponent()
    {
        if (this->vox != NULL) delete this->vox;
    }

    VoxelModelComponent()
    : Component(COMPONENT_VOXEL_MODEL, COMPONENT_INTERFACE_VOXEL_MODEL),
    vox(NULL), vox_dat(NULL),
    init_hitscan(false), init_draw(false), frozen_vox(false)
    {}
};


} // Objects

