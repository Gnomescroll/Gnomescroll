#pragma once

#if DC_CLIENT

#include <entity/constants.hpp>
#include <entity/components/draw/voxel.hpp>

namespace Components
{

class ColoredVoxelComponent: public VoxelComponent
{
    public:
        struct Color color;
        
        void call();

        void set_color(unsigned char r, unsigned char g, unsigned char b)
        {
            this->color.r = r;
            this->color.g = g;
            this->color.b = b;
        }

    ColoredVoxelComponent()
    : VoxelComponent(COMPONENT_COLORED_VOXEL)
    {}

};
    
} // Components

#endif
