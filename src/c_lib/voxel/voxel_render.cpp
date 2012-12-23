#include "voxel_render.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <state/client_state.hpp>
#include <camera/fulstrum_test.hpp>
#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/affine.hpp>
#include <entity/constants.hpp>
#include <input/handlers.hpp>
#include <agent/client/player_agent.hpp>

#define VOXEL_RENDER_DEBUG 0

namespace Voxels
{

// Shader

static GLenum voxel_shader_vert = 0;
static GLenum voxel_shader_frag = 0;
static GLenum voxel_shader_prog = 0;

int InRotationMatrix;
int InTranslation;

int InNormal;
int InAO;
int InTex;
//int InSide;

void init_voxel_render_list_shader1()
{
    static int init=0;
    if (init++)
    {
        printf("VoxelRenderList::init_voxel_render_list_shader1, error, tried to init more than once\n");
        return;
    }
    printf("init voxel shader\n");

    int DEBUG1 = 0;

    voxel_shader_prog = glCreateProgramObjectARB(); //glCreateProgram();
    voxel_shader_vert = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    voxel_shader_frag = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;

    vs = textFileRead("./media/shaders/voxel/voxel.vsh");
    fs = textFileRead("./media/shaders/voxel/voxel.fsh");

    glShaderSourceARB(voxel_shader_vert, 1, (const GLcharARB**)&vs, NULL);
    glShaderSourceARB(voxel_shader_frag, 1, (const GLcharARB**)&fs, NULL);

    glCompileShaderARB(voxel_shader_vert);
    if (DEBUG1) printShaderInfoLog(voxel_shader_vert);

    glCompileShaderARB(voxel_shader_frag);
    if (DEBUG1) printShaderInfoLog(voxel_shader_frag);
    
    glAttachObjectARB(voxel_shader_prog, voxel_shader_vert);
    glAttachObjectARB(voxel_shader_prog, voxel_shader_frag);

    glLinkProgramARB(voxel_shader_prog);

    if (DEBUG1) printProgramInfoLog(voxel_shader_prog); // print diagonostic information

    //uniforms
    InRotationMatrix = glGetUniformLocationARB(voxel_shader_prog, "InRotationMatrix");
    InTranslation = glGetUniformLocationARB(voxel_shader_prog, "InTranslation");
    //attributes
    InNormal = glGetAttribLocation(voxel_shader_prog, "InNormal");
    InAO = glGetAttribLocation(voxel_shader_prog, "InAO");
    InTex = glGetAttribLocation(voxel_shader_prog, "InTex");

    free(vs);
    free(fs);
}

// VoxelRenderList


VoxelRenderList::VoxelRenderList() :
    needs_update(false), id(-1), num_elements(0)
{
    const int starting_size = 1024;

    vbo_wrapper[0].vertex_list = (VoxelVertex*) malloc(starting_size*sizeof(VoxelVertex));
    vbo_wrapper[1].vertex_list = (VoxelVertex*) malloc(starting_size*sizeof(VoxelVertex));

    vbo_wrapper[0].max_size = starting_size; //in voxel vertex
    vbo_wrapper[1].max_size = starting_size;
    
    vbo_wrapper[0].id = 0;
    vbo_wrapper[1].id = 0;

    vbo_wrapper[0].vnum = 0;
    vbo_wrapper[1].vnum = 0;

    this->render_list = (VoxelVolume**)calloc(VOXEL_RENDER_LIST_SIZE, sizeof(VoxelVolume*));
}

VoxelRenderList::~VoxelRenderList()
{
    if (this->render_list != NULL)
        free(this->render_list);

    if (vbo_wrapper[0].vertex_list != NULL)
        free(vbo_wrapper[0].vertex_list);
    if (vbo_wrapper[1].vertex_list != NULL)
        free(vbo_wrapper[1].vertex_list);
}

void VoxelRenderList::register_voxel_volume(VoxelVolume* vv)
{
    if (this->num_elements >= VOXEL_RENDER_LIST_SIZE)
    {
        printf("VoxelRenderList Error: number of voxel models exceeds VOXEL_RENDER_LIST_SIZE \n");
        return;
    }
    for (int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if (this->render_list[i] == NULL)
        {
            this->num_elements++;
            this->render_list[i] = vv;
            vv->render_id = i;
            vv->voxel_render_list = this;
            vv->voxel_render_list_id = this->id;
            this->needs_update = true;
            break;
        }
    }
}

void VoxelRenderList::unregister_voxel_volume(VoxelVolume* vv)
{
    if (vv == NULL) return;
    if (vv->render_id < 0) return;
    if (this->render_list[vv->render_id] == NULL) return;

    this->num_elements--;
    this->render_list[vv->render_id] = NULL;

    vv->render_id = -1;
    vv->voxel_render_list = NULL;

    this->needs_update = true;
}

void VoxelRenderList::update_vertex_buffer_object()
{
    VoxelVolume* vv = NULL;

    struct VBOmeta* _vbo = &vbo_wrapper[0];
    int v_num = 0;
    int volumes_updated = 0;
    for (int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if (this->render_list[i] == NULL ) continue;

        vv = this->render_list[i];
        if (vv->needs_vbo_update == true )
        {
            vv->needs_vbo_update = false;
            volumes_updated++;
            vv->update_vertex_list();
        }
        v_num += vv->vvl.vnum;
    }

    _vbo->vnum = v_num;
    
    if (v_num == 0) return;
    if (!this->needs_update && volumes_updated == 0) return; //return if nothing to update
    this->needs_update = false;
    
    if (v_num >= _vbo->max_size)
    {
        while (v_num >= _vbo->max_size) _vbo->max_size *= 2; //double max size until its large enough and realloc
        _vbo->vertex_list = (VoxelVertex*)realloc(_vbo->vertex_list, _vbo->max_size*sizeof(VoxelVertex));
    }

    int index = 0;
    // continue counting indices, but realigning changed data
    for (int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if (this->render_list[i] == NULL) continue;
        vv = this->render_list[i];

        GS_ASSERT(vv->vvl.vnum != 0);
        GS_ASSERT(vv->vvl.vertex_list != 0)
        
        memcpy( &_vbo->vertex_list[index], vv->vvl.vertex_list, vv->vvl.vnum*sizeof(VoxelVertex) );
        vv->vvl.voff = index;
        index += vv->vvl.vnum;
    }
    GS_ASSERT(index == v_num);
    if (_vbo->id == 0 )  glGenBuffers(1, &_vbo->id);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo->id);
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(VoxelVertex), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(VoxelVertex), _vbo->vertex_list, GL_STATIC_DRAW);
}

// VoxelRenderListManager

void VoxelRenderListManager::register_voxel_volume(class VoxelVolume* vv)
{
    GS_ASSERT(this->max > 0);
    GS_ASSERT(this->lists != NULL);
    if (this->max <= 0) return;
    if (this->lists == NULL) return;
    // choose a list to register with
    int smallest = 0;
    int smallest_count = 99999999;
    for (int i=0; i<this->max; i++)
    {
        if (this->lists[i].num_elements < smallest_count)
        {
            smallest = i;
            smallest_count = this->lists[i].num_elements;
        }
    }
    this->lists[smallest].register_voxel_volume(vv);
}

void VoxelRenderListManager::unregister_voxel_volume(class VoxelVolume* vv)
{
    GS_ASSERT(this->max > 0);
    GS_ASSERT(this->lists != NULL);
    // unregister from correct list

    int id = vv->voxel_render_list_id;
    if (id < 0 || id >= this->max) return;
    this->lists[id].unregister_voxel_volume(vv);
}

void VoxelRenderListManager::draw()
{
    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);
    glColor3ub(255,255,255);

    //;
    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    glUseProgramObjectARB(voxel_shader_prog);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glEnableVertexAttribArray(InNormal);
    glEnableVertexAttribArray(InAO);
    glEnableVertexAttribArray(InTex);

    for (int k=0; k < this->max; k++)
    {
        VoxelRenderList* vrl = &this->lists[k];
        struct VBOmeta* _vbo = &vrl->vbo_wrapper[0];

        vrl->update_vertex_buffer_object(); 

        if (_vbo->vnum == 0) continue;
        IF_ASSERT(_vbo->id == 0) continue;

        glBindBuffer(GL_ARRAY_BUFFER, _vbo->id);

        glVertexPointer(3, GL_FLOAT, sizeof(class VoxelVertex), (GLvoid*)0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(class VoxelVertex), (GLvoid*)12);
        
        glVertexAttribPointer(InNormal, 3, GL_BYTE, GL_FALSE, sizeof(class VoxelVertex), (GLvoid*)16);
        glVertexAttribPointer(InAO, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(class VoxelVertex), (GLvoid*)20);
        glVertexAttribPointer(InTex, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(class VoxelVertex), (GLvoid*)24);

        for (int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
        {
            if (vrl->render_list[i] == NULL) continue;
            VoxelVolume* vv = vrl->render_list[i];
            if (!vv->draw) continue;
            if (vv->vvl.vnum == 0) continue;
            
            glUniformMatrix3fv(InRotationMatrix, 1, false, (GLfloat*)vv->world_matrix._f);
            Vec3 p = quadrant_translate_position(current_camera_position, vv->world_matrix.c);
            glUniform3fv(InTranslation, 1, (GLfloat*)p.f);

            if (_vbo->vnum < (int)(vv->vvl.vnum+vv->vvl.voff))
            {
                printf("VoxelRenderListManager::draw error!! would draw past VBO memory\n");
                printf("vbo vnum= %i vv vnum= %i vv voff= %i \n", _vbo->vnum, vv->vvl.vnum, vv->vvl.voff);
                printf("would read %i vertices past vbo \n", vv->vvl.vnum+vv->vvl.voff - _vbo->vnum);
                //GS_ABORT();
            }
            //printf("vnum= %i, vbo_id= %i voff=%i vnum_local %i \n", _vbo->vnum, _vbo->id, vv->vvl.voff,vv->vvl.vnum);
            glDrawArrays(GL_QUADS, vv->vvl.voff, vv->vvl.vnum);
        }
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);

    glDisableVertexAttribArray(InNormal);
    glDisableVertexAttribArray(InAO);
    glDisableVertexAttribArray(InTex);

    glUseProgramObjectARB(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //glEnd();
    glDisable(GL_CULL_FACE);
    //glEnable(GL_TEXTURE_2D);

    glColor3ub(255,255,255);

    for (int k=0; k < this->max; k++)
    {
        VoxelRenderList* vrl = &this->lists[k];
        vrl->update_vertex_buffer_object(); 
    }
}

}   // Voxels
