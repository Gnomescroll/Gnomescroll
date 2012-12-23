#pragma once

#if DC_SERVER
dont_include_this_file_on_server
#endif

#include <common/compat_gl.h>
#include <common/macros.hpp>
#include <voxel/voxel_volume.hpp>
#include <SDL/shader_loader.hpp>

/*
    Double buffered drawing object template
*/

namespace Voxels
{

const int VOXEL_RENDER_LIST_SIZE = 1024;
const int VOXEL_RENDER_LISTS = 16;

struct VBOmeta
{
    GLuint id;
    class VoxelVertex* vertex_list;
    int vnum;
    int max_size; //in voxel vertex elements
    //use to prevent constant mallocing
    //int size_max; //increase size if it goes over this
};

class VoxelVolumeToken
{
    int index;
    int vnum;   //number of vertices
    int voff;   //offset of vertices
};

void init_voxel_render_list_shader1();

class VoxelRenderList
{
    public:
        bool first_load;
        bool needs_update;
        void update_vertex_buffer_object(); //gets called after draw
        class VoxelVolume** render_list;
        struct VBOmeta vbo_wrapper[2];

        int id;
        int num_elements;

    void register_voxel_volume(class VoxelVolume* vv);
    void unregister_voxel_volume(class VoxelVolume* vv);

    bool full()
    {
        return (this->num_elements >= VOXEL_RENDER_LIST_SIZE);
    }

    VoxelRenderList();
    ~VoxelRenderList();
};

class VoxelRenderListManager
{
    public:
        VoxelRenderList* lists;
        int max;

    void register_voxel_volume(class VoxelVolume* vv);
    void unregister_voxel_volume(class VoxelVolume* vv);

    void draw();

    void init(int max)
    {
        IF_ASSERT(this->lists != NULL) return;
        this->max = max;
        this->lists = new VoxelRenderList[this->max];
        for (int i=0; i<this->max; i++) this->lists[i].id = i;
    }

    void init()
    {
        this->init(VOXEL_RENDER_LISTS);
    }

    VoxelRenderListManager() :
        lists(NULL), max(0)
    {}

    ~VoxelRenderListManager()
    {
        if (this->lists != NULL) delete[] this->lists;
    }
};

}   // Voxels
