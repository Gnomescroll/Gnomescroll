#include "voxel_render.hpp"

#include <state/client_state.hpp>
#include <camera/fulstrum_test.hpp>

#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
//#include <physics/mat3.hpp>
//#include <physics/mat4.hpp>
#include <physics/affine.hpp>

#include <entity/constants.hpp>

#include <input/handlers.hpp>

#define VOXEL_RENDER_DEBUG 0

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
        printf("Voxel_render_list::init_voxel_render_list_shader1, error, tried to init more than once\n");
        return;
    }
    printf("init voxel shader\n");

    int DEBUG = 0;

    voxel_shader_prog = glCreateProgramObjectARB(); //glCreateProgram();
    voxel_shader_vert = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    voxel_shader_frag = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    char *vs, *fs;

    vs = textFileRead((char*) "./media/shaders/voxel/voxel.vsh");
    fs = textFileRead((char*) "./media/shaders/voxel/voxel.fsh");

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

// Voxel_render_list


Voxel_render_list::Voxel_render_list()
:
needs_update(false),
id(-1),
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

    this->render_list = (Voxel_volume**)calloc(VOXEL_RENDER_LIST_SIZE, sizeof(Voxel_volume*));
}

Voxel_render_list::~Voxel_render_list()
{
    if (this->render_list != NULL)
        free(this->render_list);

    if (vbo_wrapper[0].vertex_list != NULL)
        free(vbo_wrapper[0].vertex_list);
    if (vbo_wrapper[1].vertex_list != NULL)
        free(vbo_wrapper[1].vertex_list);
}

void Voxel_render_list::register_voxel_volume(Voxel_volume* vv)
{
    if(num_elements >= VOXEL_RENDER_LIST_SIZE)
    {
        printf("Voxel_render_list Error: number of voxel models exceeds VOXEL_RENDER_LIST_SIZE \n");
        return;
    }
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(this->render_list[i] == NULL)
        {
            num_elements++;
            this->render_list[i] = vv;
            vv->id = i;
            vv->voxel_render_list = this;
            vv->voxel_render_list_id = this->id;
            this->needs_update = true;
            break;
        }
    }
}

void Voxel_render_list::unregister_voxel_volume(Voxel_volume* vv)
{
    if (vv == NULL) return;
    if (vv->id < 0) return;
    if (this->render_list[vv->id] == NULL) return;

    this->num_elements--;
    this->render_list[vv->id] = NULL;

    vv->id = -1;
    vv->voxel_render_list = NULL;

    this->needs_update = true;
}

//void Voxel_render_list::update()
//{
    ////this->update_vertex_buffer_object();
//}

void Voxel_render_list::update_vertex_buffer_object()
{
    //if (!this->needs_update) return;
    //this->needs_update = false;
    
    Voxel_volume* vv;

    struct VBOmeta* _vbo = &vbo_wrapper[0];
    int v_num = 0;
    int volumes_updated = 0;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(this->render_list[i] == NULL ) continue;

        vv = this->render_list[i];
        if (!vv->draw) continue;
        if( vv->needs_vbo_update == true )
        {
            vv->needs_vbo_update = false;
            volumes_updated++;
            vv->update_vertex_list();
        }
        v_num += vv->vvl.vnum;
    }

    _vbo->vnum = v_num;
    //printf("v_num = %i\n", v_num);
    
    if(v_num == 0) return;
    if(volumes_updated == 0) return; //return if nothing to update

    if( v_num >= _vbo->max_size ) 
    {
        while(v_num >= _vbo->max_size)
        {
            _vbo->max_size *= 2; //double max size until its large enough and realloc
        }
        _vbo->vertex_list = (Voxel_vertex*) realloc (_vbo->vertex_list, _vbo->max_size*sizeof(Voxel_vertex) );
    }

    int index = 0;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(this->render_list[i] == NULL || !this->render_list[i]->draw) continue;
        vv = this->render_list[i];

        if(vv->vvl.vnum == 0) printf("Voxel_render_list::update_vertex_buffer_object, vox errro 1: vv->vvl.vnum == 0 \n");
        if(vv->vvl.vertex_list == 0) printf("Voxel_render_list::update_vertex_buffer_object, vox errro 3: vv->vvl.vertex_list == NULL \n");


        memcpy( &_vbo->vertex_list[index], vv->vvl.vertex_list, vv->vvl.vnum*sizeof(Voxel_vertex) );
        vv->vvl.voff = index;
        index += vv->vvl.vnum;
    }
    if( _vbo->id == 0 )  glGenBuffers( 1, &_vbo->id );
    glBindBuffer(GL_ARRAY_BUFFER, _vbo->id);
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(Voxel_vertex), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(Voxel_vertex), _vbo->vertex_list, GL_STATIC_DRAW);
}

//void Voxel_render_list::draw()
//{

//}


// Voxel_render_list_manager

void Voxel_render_list_manager::register_voxel_volume(class Voxel_volume* vv)
{
    assert(this->max > 0);
    assert(this->lists != NULL);
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

void Voxel_render_list_manager::unregister_voxel_volume(class Voxel_volume* vv)
{
    assert(this->max > 0);
    assert(this->lists != NULL);
    // unregister from correct list

    int id = vv->voxel_render_list_id;
    if (id < 0 || id >= this->max) return;
    this->lists[id].unregister_voxel_volume(vv);
}



void Voxel_render_list_manager::draw()
{

/*
    #if !PRODUCTION
    if (input_state.skeleton_editor)
    {
        for(int k=0; k < this->max; k++)
        {
            Voxel_render_list* vrl = &this->lists[k];

            for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
            {
                if (vrl->render_list[i] == NULL || !vrl->render_list[i]->draw ) continue;
                Voxel_volume* vv = vrl->render_list[i];

                if (vv->vvl.vnum == 0) continue;
                if (!sphere_fulstrum_test(
                    vv->world_matrix.v[3].x,
                    vv->world_matrix.v[3].y,
                    vv->world_matrix.v[3].z,
                    vv->radius
                )) continue;

                vv->draw_bounding_box();
            }
        }
    }
      #endif
*/


    GL_ASSERT(GL_DEPTH_TEST, true);
    GL_ASSERT(GL_BLEND, false);
    glColor3ub(255,255,255);

    glShadeModel(GL_FLAT);
    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    glUseProgramObjectARB(voxel_shader_prog);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glEnableVertexAttribArray(InNormal);
    glEnableVertexAttribArray(InAO);
    glEnableVertexAttribArray(InTex);

    for(int k=0; k < this->max; k++)
    {
        Voxel_render_list* vrl = &this->lists[k];
        struct VBOmeta* _vbo = &vrl->vbo_wrapper[0];

        vrl->update_vertex_buffer_object(); 

        if( _vbo->vnum == 0 ) 
        {
            //if (VOXEL_RENDER_DEBUG)
            //    printf("Voxel_render_list::draw, vnum equals zero \n");
            continue;
        }

        if( _vbo->id == 0 )
        {
            //if (VOXEL_RENDER_DEBUG)
                printf("Voxel_render_list::draw, vbo is zero !!! SHOULD NOT OCCUR! \n");
            continue;
        }

        glBindBuffer(GL_ARRAY_BUFFER, _vbo->id);

        glVertexPointer(3, GL_FLOAT, sizeof(struct Voxel_vertex), (GLvoid*)0);
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Voxel_vertex), (GLvoid*)12);
        
        glVertexAttribPointer(InNormal, 3, GL_BYTE, GL_FALSE, sizeof(struct Voxel_vertex), (GLvoid*)16);
        glVertexAttribPointer(InAO, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Voxel_vertex), (GLvoid*)20);
        glVertexAttribPointer(InTex, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Voxel_vertex), (GLvoid*)24);

        //int drawn = 0;
        for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
        {
            if (vrl->render_list[i] == NULL || !vrl->render_list[i]->draw) continue;
            if(_vbo->vnum ==0 ) continue;
            Voxel_volume* vv = vrl->render_list[i];

            if (!sphere_fulstrum_test(
                vv->world_matrix.v[3].x,
                vv->world_matrix.v[3].y,
                vv->world_matrix.v[3].z,
                vv->radius
            )) continue;

            //vrl->update_vertex_buffer_object(); 
            if (vv->vvl.vnum == 0) continue;

            glUniformMatrix3fv(InRotationMatrix, 1, false, (GLfloat*) vv->world_matrix._f );
            glUniform3fv(InTranslation, 1, (GLfloat*) (vv->world_matrix._f + 9) );

            if(_vbo->vnum < (int)(vv->vvl.vnum+vv->vvl.voff))
            {
                printf("Voxel_render_list_manager::draw error!! would draw past VBO memory\n");
                printf("vbo vnum= %i vv vnum= %i vv voff= %i \n", _vbo->vnum, vv->vvl.vnum, vv->vvl.voff);
                printf("would read %i vertices past vbo \n", vv->vvl.vnum+vv->vvl.voff - _vbo->vnum);
                GS_ABORT();
            }
            //printf("vnum= %i, vbo_id= %i voff=%i vnum_local %i \n", _vbo->vnum, _vbo->id, vv->vvl.voff,vv->vvl.vnum);
            glDrawArrays( GL_QUADS, vv->vvl.voff, vv->vvl.vnum );
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

    for(int k=0; k < this->max; k++)
    {
        Voxel_render_list* vrl = &this->lists[k];
        vrl->update_vertex_buffer_object(); 
    }
}
