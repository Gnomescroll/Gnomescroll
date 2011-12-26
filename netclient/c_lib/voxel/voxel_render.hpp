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

#define VOXEL_RENDER_LIST_SIZE 1024

/*
    list, add, remove, swap, iterate
*/

class Voxel_render_list
{
    public:
    struct VBOmeta vbo1;

    Voxel_volume** render_list;
    int num_elements;

    void register_voxel_volume(Voxel_volume* vv);
    void unregister_voxel_volume(Voxel_volume* vv);

    static void init_voxel_render_list_shader1();

    void draw();
    void update_vertex_buffer_object();

    Voxel_render_list()
    {
        render_list = new Voxel_volume*[VOXEL_RENDER_LIST_SIZE];
        num_elements = 0;
        for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++) render_list[i] = NULL;
    }
};

void Voxel_render_list::update_vertex_buffer_object()
{
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] != NULL && render_list[i]->needs_vbo_update == true)
        {
            render_list[i]->update_vertex_list();
        }
    }
}


void Voxel_render_list::init_voxel_render_list_shader1()
{
    printf("test\n");

}

void Voxel_render_list::draw()
{
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL) continue;
        
    }
}



void Voxel_render_list::register_voxel_volume(Voxel_volume* vv)
{
    if(num_elements >= VOXEL_RENDER_LIST_SIZE)
    {
        printf("Voxel_render_list Error: number of voxel models exceeds VOXEL_RENDER_LIST_SIZE \n");
        return;
    }
    num_elements++;

    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL)
        {
            render_list[i] = vv;
            printf("Added voxel volume %i \n", i);
            break;
        }
    }

    vv->voxel_render_list = this;
}

void Voxel_render_list::unregister_voxel_volume(Voxel_volume* vv)
{
    num_elements--;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == vv)
        {
            render_list[i] = NULL;
            printf("Removed voxel volume %i \n", i);
            break;
        }
    }
}