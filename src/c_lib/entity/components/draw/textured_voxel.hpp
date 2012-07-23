#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <entity/constants.hpp>
#include <entity/components/draw/voxel.hpp>

namespace Components
{

class TexturedVoxelComponent: public VoxelComponent
{
    private:        
        void set_texture();

    public:
        // Not configurable:
        float tx,ty;
        float sprite_width;

        int texture_index;
        int pixel_width;
        
        void init()
        {
            VoxelComponent::init();
            this->set_texture();
        }
        void call();
        inline void draw(Vec3 position) __attribute__((always_inline));

    TexturedVoxelComponent()
    : VoxelComponent(COMPONENT_TEXTURED_VOXEL),
    texture_index(0), pixel_width(1)
    {}

};
    
} // Components
