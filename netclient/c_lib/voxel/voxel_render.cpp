#include "voxel_render.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/camera/fulstrum_test.hpp>

#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
//#include <physics/mat3.hpp>
//#include <physics/mat4.hpp>
#include <physics/affine.hpp>

#include <c_lib/common/enum_types.hpp>

const int VOXEL_RENDER_LIST_SIZE = 1024;

Voxel_render_list::Voxel_render_list()
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

    this->render_list = (Voxel_volume**)calloc(VOXEL_RENDER_LIST_SIZE, sizeof(Voxel_volume*));
}

Voxel_render_list::~Voxel_render_list()
{
    if (this->render_list)
        free(this->render_list);

    free(vbo_wrapper[0].vertex_list);
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
            break;
        }
    }
}

void Voxel_render_list::unregister_voxel_volume(Voxel_volume* vv)
{
    if (vv == NULL) return;
    int i;
    for(i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(this->render_list[i] == vv)
        {
            num_elements--;
            this->render_list[i] = NULL;
            break;
        }
    }

    vv->id = -1;
    vv->voxel_render_list = NULL;
    if (i == VOXEL_RENDER_LIST_SIZE)
        printf("Voxel_render_list::unregister_voxel_volume error, volume was not on list \n");
}

void Voxel_render_list::update_vertex_buffer_object()
{
    Voxel_volume* vv;

    struct VBOmeta* _vbo = &vbo_wrapper[0];
    int v_num = 0;
    int volumes_updated = 0;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(this->render_list[i] == NULL) continue;

        vv = this->render_list[i];
        if( vv->needs_vbo_update == true )
        {
            vv->needs_vbo_update = false;
            volumes_updated++;
            vv->update_vertex_list();
            if(vv->vvl.vnum == 0) printf("Voxel_render_list::update_vertex_buffer_object, FATAL ERROR, voxel volume has no voxel!\n");
        }
        v_num += vv->vvl.vnum;
    }

    _vbo->vnum = v_num;

    if(volumes_updated == 0)
    {
        return;         //no voxel volumes were updated
    }

    if(v_num == 0)
    {
        printf("Voxel_render_list::update_vertex_buffer_object, zero vertices \n");
        return;
    }

    //printf("Voxel_render_list::update_vertex_buffer_object: total vnum= %i \n", _vbo->vnum);

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
        if(this->render_list[i] == NULL) continue;
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

static GLenum voxel_shader_vert = 0;
static GLenum voxel_shader_frag = 0;
static GLenum voxel_shader_prog = 0;

int InRotationMatrix;
int InTranslation;

int InNormal;
int InAO;
int InTex;
//int InSide;

void Voxel_render_list::init_voxel_render_list_shader1()
{
    static int init=0;
    if (init)
    {
        printf("Voxel_render_list::init_voxel_render_list_shader1, error, tried to init twice\n");
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

    init=1;
}

void Voxel_render_list::draw()
{

    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if( render_list[i] == NULL || !render_list[i]->draw ) continue;
        Voxel_volume* vv = render_list[i];

        if(vv->vvl.vnum == 0) continue;
        if(! sphere_fulstrum_test( vv->world_matrix.v[3].x, vv->world_matrix.v[3].y, vv->world_matrix.v[3].z, vv->radius) ) continue;

        vv->draw_bounding_box();
    }


    glDisable(GL_TEXTURE_2D);

    struct VBOmeta* _vbo = &vbo_wrapper[0];

    //printf("buff= %i \n",  _vbo->id);

    if( _vbo->vnum == 0 )
    {
        printf("Voxel_render_list::draw, vnum equals zero \n");
        this->update_vertex_buffer_object();
        return;
    }
    if( _vbo->id == 0 )
    {
        printf("Voxel_render_list::draw, vbo is zero !!! SHOULD NOT OCCUR! \n");
        return;
    }

    glUseProgramObjectARB(voxel_shader_prog);

    glColor3b(255,255,255);
    
    glShadeModel(GL_FLAT);
    glEnable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo->id);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glEnableVertexAttribArray(InNormal);
    glEnableVertexAttribArray(InAO);
    glEnableVertexAttribArray(InTex);

    glVertexPointer(3, GL_FLOAT, sizeof(struct Voxel_vertex), (GLvoid*)0);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Voxel_vertex), (GLvoid*)12);
    
    glVertexAttribPointer(InNormal, 3, GL_BYTE, GL_FALSE, sizeof(struct Voxel_vertex), (GLvoid*)16);
    glVertexAttribPointer(InAO, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Voxel_vertex), (GLvoid*)20);
    glVertexAttribPointer(InTex, 2, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(struct Voxel_vertex), (GLvoid*)24);

    //int drawn = 0;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if( this->render_list[i] == NULL || !this->render_list[i]->draw ) continue;
        Voxel_volume* vv = this->render_list[i];

        if(vv->vvl.vnum == 0) continue;
        if(! sphere_fulstrum_test( vv->world_matrix.v[3].x, vv->world_matrix.v[3].y, vv->world_matrix.v[3].z, vv->radius) ) continue;

        //drawn++;

        int debug = 0;

        if( vv->vhe.entity_type == OBJ_TYPE_AGENT && vv->vhe.part_id == 2 )
        {
            //debug = 1;
        }
        if( vv->vhe.entity_type == OBJ_TYPE_AGENT && vv->vhe.part_id == 3 )
        {
            //debug = 1;
        }

        if(debug)
        { 
            printf("=== \n");

            printf("entity= %i, entity_type= %i, part_id= %i \n", 
            vv->vhe.entity_id, vv->vhe.entity_type, vv->vhe.part_id);

            printf("parent_world_matrix= \n");
            print_affine( *vv->parent_world_matrix );

            printf("local_matrix= \n");
            print_affine( vv->local_matrix );

            printf("result= \n");
            print_affine( vv->world_matrix );
        }

        glUniformMatrix3fv(InRotationMatrix, 1, false, (GLfloat*) vv->world_matrix._f );
        glUniform3fv(InTranslation, 1, (GLfloat*) (vv->world_matrix._f + 9) );

        glDrawArrays( GL_QUADS, vv->vvl.voff, vv->vvl.vnum );
    }
    //printf("%i drawn \n", drawn);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);

    glDisableVertexAttribArray(InNormal);
    glDisableVertexAttribArray(InAO);
    glDisableVertexAttribArray(InTex);

    glEnable (GL_DEPTH_TEST);

    glUseProgramObjectARB(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //glEnd();
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_DEPTH_TEST);
    //glShadeModel(shade_model);

/*
    Apply changes
*/
    this->update_vertex_buffer_object(); 
}