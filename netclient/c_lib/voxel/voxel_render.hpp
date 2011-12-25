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

class Voxel_volume_token
{
    int index;
    int vnum;   //number of vertices
    int voff;   //offset of vertices

};

class Voxel_render_list
{
    public:
    struct VBOmeta vbo1;


    void register_voxel_volume(Voxel_volume* vv)
    {
        vv->voxel_render_list = this;
    }

    void unregister_voxel_volume(Voxel_volume* vv)
    {
        
    }


    Voxel_render_list()
    {
        
    }
};