#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/draw.hpp>

namespace Components
{

class TexturedVoxelComponent: public DrawComponent
{
    private:
        Vec3 forward;
        Vec3 right;
        Vec3 normal;
        float tx,ty;
        
    public:
        int sprite_index;
        float pixel_margin;
        float scale;
        
        void call();

    TexturedVoxelComponent()
    : DrawComponent(COMPONENT_TEXTURED_VOXEL),
    sprite_index(0), pixel_margin(1.0f), scale(1.0f)
    {}

};
    
} // Components
