#pragma once

#include <compat_gl.h>

#include <voxel/voxel_volume.hpp>

void voxel_render_init();


struct VBOmeta
{
    GLint id;
    int vnum;
    int size_max; //increase size if it goes over this
};

class Voxel_render_list
{
    struct VBOmeta vbometa1;



    Voxel_render_list()
    {
        
    }
}