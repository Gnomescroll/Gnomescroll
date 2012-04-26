#pragma once

#include <compat_gl.h>

#include <voxel/voxel_volume.hpp>

#include <SDL/shader_loader.hpp>

//#include <physics/vector4.hpp>

/*
    Double buffered drawing object template
*/

struct VBOmeta
{
    GLuint id;
    Voxel_vertex* vertex_list;
    int vnum;
    int max_size; //in voxel vertex elements
    //use to prevent constant mallocing
    //int size_max; //increase size if it goes over this
};

class Voxel_volume_token
{
    int index;
    int vnum;   //number of vertices
    int voff;   //offset of vertices
};

/*
    Note: again, here need a list template for adding/removing/iterating
    list, add, remove, swap, iterate
*/

class Voxel_render_list
{
    private:
    Voxel_volume** render_list;
    struct VBOmeta vbo_wrapper[2];

    void update_vertex_buffer_object(); //gets called after draw

    public:
    
    int num_elements;

    void register_voxel_volume(Voxel_volume* vv);
    void unregister_voxel_volume(Voxel_volume* vv);

    void draw();
    void update();

    static void init_voxel_render_list_shader1();

    Voxel_render_list();
    ~Voxel_render_list();
};
