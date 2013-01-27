#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/draw/voxel.hpp>

namespace Draw
{

class TexturedVoxel: public Voxel
{
    public:
        // Not configurable:
        float tx,ty;
        float sprite_width;

        // Must configure
        int texture_index;
        int pixel_width;

        int cube_type;

        void set_texture();

        void init()
        {
            Voxel::init();
            this->set_texture();
        }

    TexturedVoxel() :
    texture_index(0), pixel_width(1), cube_type(255)
    {}

};

} // Draw
