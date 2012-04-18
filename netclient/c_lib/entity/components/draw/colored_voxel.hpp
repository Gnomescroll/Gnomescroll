#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/draw.hpp>

namespace Components
{

class ColoredVoxelComponent: public DrawComponent
{
    private:
        Vec3 forward;
        Vec3 right;
        Vec3 normal;
        
    public:
        struct Color color;
        float scale;
        
        void call();

    ColoredVoxelComponent()
    : DrawComponent(COMPONENT_COLORED_VOXEL), scale(1.0f)
    {}

};
    
} // Components
