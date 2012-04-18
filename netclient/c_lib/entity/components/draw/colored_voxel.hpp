#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/draw/voxel.hpp>

namespace Components
{

class ColoredVoxelComponent: public VoxelComponent
{
    private:
        Vec3 forward;
        Vec3 right;
        Vec3 normal;
        
    public:
        struct Color color;
        
        void call();

        void set_color(struct Color color)
        {
            this->color = color;
        }
        
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
