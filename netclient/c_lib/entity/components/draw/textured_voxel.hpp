#pragma once

#if DC_CLIENT

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/draw/voxel.hpp>

namespace Components
{

class TexturedVoxelComponent: public VoxelComponent
{
    private:
        Vec3 forward;
        Vec3 right;
        Vec3 normal;
        float tx,ty;
        float pixel_margin;
        
        void set_texture();

    public:
        int texture_index;
        int pixel_width;
        
        void init()
        {
            VoxelComponent::init();
            this->set_texture();
        }
        void call();

    TexturedVoxelComponent()
    : VoxelComponent(COMPONENT_TEXTURED_VOXEL),
    texture_index(0), pixel_width(1)
    {}

};
    
} // Components

#endif
