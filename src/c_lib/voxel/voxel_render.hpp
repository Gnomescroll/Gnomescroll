#pragma once

#if DC_SERVER
dont_include_this_file_on_server
#endif

#include <common/compat_gl.h>
#include <common/macros.hpp>
#include <voxel/voxel_volume.hpp>
#include <SDL/shader_loader.hpp>

//#include <physics/vector4.hpp>

/*
    Double buffered drawing object template
*/

const int VOXEL_RENDER_LIST_SIZE = 1024;
const int VOXEL_RENDER_LISTS = 16;

struct VBOmeta
{
    GLuint id;
    class Voxel_vertex* vertex_list;
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

void init_voxel_render_list_shader1();

class Voxel_render_list
{
    public:
        bool first_load;
        bool needs_update;
        void update_vertex_buffer_object(); //gets called after draw
        class Voxel_volume** render_list;
        struct VBOmeta vbo_wrapper[2];

        int id;
        int num_elements;

        void register_voxel_volume(class Voxel_volume* vv);
        void unregister_voxel_volume(class Voxel_volume* vv);

        bool full() { return this->num_elements >= VOXEL_RENDER_LIST_SIZE; }

    Voxel_render_list();
    ~Voxel_render_list();
};

class Voxel_render_list_manager
{
    public:
        Voxel_render_list* lists;
        int max;

        void register_voxel_volume(class Voxel_volume* vv);
        void unregister_voxel_volume(class Voxel_volume* vv);

        void draw();

        void init(int max)
        {
            GS_ASSERT(this->lists == NULL);
            if (this->lists != NULL) return;
            this->max = max;
            this->lists = new Voxel_render_list[this->max];
            for (int i=0; i<this->max; i++) this->lists[i].id = i;
        }

        void init()
        {
            this->init(VOXEL_RENDER_LISTS);
        }

    Voxel_render_list_manager()
    : lists(NULL), max(0)
    {}

    ~Voxel_render_list_manager()
    {
        if (this->lists != NULL) delete[] this->lists;
    }
};
