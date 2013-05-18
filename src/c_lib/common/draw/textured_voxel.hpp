#pragma once

#if DC_SERVER
# error Don't include this file in the server
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

        CubeType cube_type;

        void set_texture();

        void init()
        {
            Voxel::init();
            this->set_texture();
        }

    TexturedVoxel() :
        texture_index(0), pixel_width(1), cube_type(NULL_CUBE)
    {}

};

} // Draw
