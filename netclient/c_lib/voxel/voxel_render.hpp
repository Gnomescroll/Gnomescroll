#pragma once

#include <compat_gl.h>

#include <voxel/voxel_volume.hpp>

#include <c_lib/SDL/shader_loader.hpp>

void voxel_render_init();


struct VBOmeta
{
    GLint id;
    Voxel_vertex* vertex_list;
    int vnum;
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
    list, add, remove, swap, iterate
*/

class Voxel_render_list
{
    public:
    struct VBOmeta vbo[2];

    Voxel_volume** render_list;
    int num_elements;

    void register_voxel_volume(Voxel_volume* vv);
    void unregister_voxel_volume(Voxel_volume* vv);

    static void init_voxel_render_list_shader1();

    void draw();
    void update_vertex_buffer_object();

    Voxel_render_list()
    {
        vbo[0].vertex_list = NULL;
        vbo[1].vertex_list = NULL;

        vbo[0].id = 0;
        vbo[1].id = 0;

        vbo[0].vnum = 0;
        vbo[1].vnum = 0;

        render_list = new Voxel_volume*[VOXEL_RENDER_LIST_SIZE];
        num_elements = 0;
        for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++) render_list[i] = NULL;
    }
};

//void * memcpy ( void * destination, const void * source, size_t num );

void Voxel_render_list::update_vertex_buffer_object()
{
    int v_num = 0;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL) continue;
        if(render_list[i]->needs_vbo_update == true)
        {
            render_list[i]->update_vertex_list();
        }
        v_num +=  render_list[i]->vvl.size;
    }

    VBOmeta* _vbo = &vbo[0]; 
    //vbo->id
    _vbo->vnum = v_num;

    if(vbo->vertex_list != NULL) delete vbo->vertex_list;
    vbo->vertex_list = new Voxel_vertex[v_num];

    int index = 0;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {   
        if(render_list[i] == NULL) continue;
        memcpy(vbo->vertex_list+index, render_list[i]->vvl.vertex_list ,_vbo->vnum*sizeof(Voxel_vertex) );

        render_list[i]->vvl.voff = index;
        index += _vbo->vnum*sizeof(Voxel_vertex);
    }

}

static GLenum voxel_shader_vert = 0;
static GLenum voxel_shader_frag = 0;
static GLenum voxel_shader_prog = 0;

int InXYZT; 
int InRGBA; 

void Voxel_render_list::init_voxel_render_list_shader1()
{
    printf("init voxel shader\n");

    int DEBUG = 1;

    voxel_shader_prog = glCreateProgramObjectARB();
    voxel_shader_vert = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    voxel_shader_frag = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;

    vs = textFileRead((char*) "./media/shaders/voxel/voxel.vsh");
    fs = textFileRead((char*) "./media/shaders/voxel/voxel.fsh");

    //vs = textFileRead((char*) "./media/shaders/terrain_map_mipmap.vsh");
    //fs = textFileRead((char*) "./media/shaders/terrain_map_mipmap.fsh");

    glShaderSourceARB(voxel_shader_vert, 1, (const GLcharARB**)&vs, NULL);
    glShaderSourceARB(voxel_shader_frag, 1, (const GLcharARB**)&fs, NULL);
    glCompileShaderARB(voxel_shader_vert);
    if(DEBUG) printShaderInfoLog(voxel_shader_vert);

    glCompileShaderARB(voxel_shader_frag);
    if(DEBUG) printShaderInfoLog(voxel_shader_frag);
    
    glAttachObjectARB(voxel_shader_prog, voxel_shader_vert);
    glAttachObjectARB(voxel_shader_prog, voxel_shader_frag);

    glLinkProgramARB(voxel_shader_prog);

    if(DEBUG) printProgramInfoLog(voxel_shader_prog); // print diagonostic information
    
    InXYZT = glGetAttribLocation(voxel_shader_prog, "InXYZT");
    InRGBA = glGetAttribLocation(voxel_shader_prog, "InRGBA");
}

void Voxel_render_list::draw()
{
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL) continue;
    }
}
