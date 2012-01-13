#pragma once

#include <compat_gl.h>

#include <voxel/voxel_volume.hpp>

#include <c_lib/SDL/shader_loader.hpp>

#include <physics/vector4.hpp>

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

#define VOXEL_RENDER_LIST_SIZE 1024

/*
    Note: again, here need a list template for adding/removing/iterating
    list, add, remove, swap, iterate
*/

class Voxel_render_list
{
    private:
    Voxel_volume** render_list;
    struct VBOmeta vbo_wrapper[2];

    public:

    int num_elements;

    void register_voxel_volume(Voxel_volume* vv);
    void unregister_voxel_volume(Voxel_volume* vv);

    static void init_voxel_render_list_shader1();

    void draw();
    void update_vertex_buffer_object();

    Voxel_render_list()
    :
    num_elements(0)
    {
        const int starting_size = 1024;

        vbo_wrapper[0].vertex_list = (Voxel_vertex*) malloc(starting_size*sizeof(Voxel_vertex));
        vbo_wrapper[1].vertex_list = (Voxel_vertex*) malloc(starting_size*sizeof(Voxel_vertex));

        vbo_wrapper[0].max_size = starting_size; //in voxel vertex
        vbo_wrapper[1].max_size = starting_size;

        vbo_wrapper[0].id = 0;
        vbo_wrapper[1].id = 0;

        vbo_wrapper[0].vnum = 0;
        vbo_wrapper[1].vnum = 0;

        render_list = new Voxel_volume*[VOXEL_RENDER_LIST_SIZE];
        for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++) render_list[i] = NULL;
    }
};
