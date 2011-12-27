#pragma once

#include <compat_gl.h>

#include <voxel/voxel_volume.hpp>

#include <c_lib/SDL/shader_loader.hpp>

void voxel_render_init();


struct VBOmeta
{
    GLuint id;
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
    struct VBOmeta vbo_wrapper[2];

    Voxel_volume** render_list;
    int num_elements;

    void register_voxel_volume(Voxel_volume* vv);
    void unregister_voxel_volume(Voxel_volume* vv);

    static void init_voxel_render_list_shader1();

    void draw();
    void update_vertex_buffer_object();

    Voxel_render_list()
    {
        vbo_wrapper[0].vertex_list = NULL;
        vbo_wrapper[1].vertex_list = NULL;

        vbo_wrapper[0].id = 0;
        vbo_wrapper[1].id = 0;

        vbo_wrapper[0].vnum = 0;
        vbo_wrapper[1].vnum = 0;

        render_list = new Voxel_volume*[VOXEL_RENDER_LIST_SIZE];
        num_elements = 0;
        for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++) render_list[i] = NULL;
    }
};

//void * memcpy ( void * destination, const void * source, size_t num );

void Voxel_render_list::update_vertex_buffer_object()
{
    VBOmeta* _vbo = &vbo_wrapper[0]; 
    int v_num = 0;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL) continue;
        if(render_list[i]->needs_vbo_update == true)
        {
            printf("%i vnum= %i \n", i, _vbo->vnum);
            render_list[i]->update_vertex_list();
        }
        v_num +=  render_list[i]->vvl.size;
    }


    //vbo->id
    _vbo->vnum = v_num;

    printf("total vnum= %i \n", _vbo->vnum);

    if(v_num == 0)
    {
        printf("Voxel_render_list::update_vertex_buffer_object, zero vertices \n");
        return;
    }
    //printf("2 vnum= %i \n", v_num);

/*
    Avoid mallocing all the time if possible

*/
    if(_vbo->vertex_list != NULL) delete _vbo->vertex_list;
    _vbo->vertex_list = new Voxel_vertex[v_num];


    int index = 0;
    Voxel_volume* vv;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {   
        if(render_list[i] == NULL) continue;
        vv = render_list[i];

        memcpy( _vbo->vertex_list+index, vv->vvl.vertex_list, vv->vvl.vnum*sizeof(Voxel_vertex) );

        render_list[i]->vvl.voff = index;
        index += vv->vvl.vnum*sizeof(Voxel_vertex);
    }

    if( _vbo->id == 0 )  glGenBuffers( 1, &_vbo->id );
    glBindBuffer(GL_ARRAY_BUFFER, _vbo->id);
    glBufferData(GL_ARRAY_BUFFER, index, NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, index, _vbo->vertex_list, GL_STATIC_DRAW);

    printf("Uploaded %i bytes to VBO \n", index);

}

static GLenum voxel_shader_vert = 0;
static GLenum voxel_shader_frag = 0;
static GLenum voxel_shader_prog = 0;

//int InCood0; 
//int InRGBA; 

int InRotationMatrix;
int InTranslation;

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
    
    //InCood0 = glGetAttribLocation(voxel_shader_prog, "InCood0");
    //InRGBA = glGetAttribLocation(voxel_shader_prog, "InRGBA");

    //uniforms

    InRotationMatrix = glGetUniformLocationARB(voxel_shader_prog, "InRotationMatrix");
    InTranslation = glGetUniformLocationARB(voxel_shader_prog, "InTranslation");

    //GLint glGetUniformLocationARB(GLhandleARB program, const GLcharARB * name)
    //voxel_vertex_index

/*
specifying uniforms
void glUniform{1|2|3|4}{f|i}ARB(GLint location, TYPE val)
void glUniform{1|2|3|4}{f|i}vARB(GLint location, GLuint count, const TYPE * vals)
void glUniformMatrix{2|3|4|}fvARB(GLint location, GLuint count, GLboolean transpose, const GLfloat * vals)
*/

}

void Voxel_render_list::draw()
{


    glDisable(GL_TEXTURE_2D);
    //    glEnable(GL_TEXTURE_2D);

    glDisable (GL_DEPTH_TEST);

    glColor3ub((unsigned char)0,(unsigned char)255,(unsigned char)0);
    glPointSize(10.0);
    glBegin(GL_POINTS);
    glVertex3f(-5.0, -5.0, 10.);
    glEnd();
    glPointSize(1.0);


    //glEnable (GL_DEPTH_TEST);

    VBOmeta* _vbo = &vbo_wrapper[0]; 

    //printf("buff= %i \n",  _vbo->id);

    if( _vbo->vnum == 0 )
    {
        printf("Voxel_render_list::draw, vnum equals zero \n");
        return;
    }

    if( _vbo->id == 0 )
    {
        printf("Voxel_render_list::draw, vbo is zero \n");
        return;
    }

    glPointSize(10.0);

    glUseProgramObjectARB(voxel_shader_prog);

    //glEnableVertexAttribArray(InCood0);
    //glEnableVertexAttribArray(InRGBA);

    glColor3b(255,255,255);

    //glShadeModel(GL_FLAT);
    //glEnable(GL_CULL_FACE);

    glDisable(GL_TEXTURE_2D);
    //glDisable (GL_DEPTH_TEST);

    //printf("Start Draw\n");

    glBindBuffer(GL_ARRAY_BUFFER, _vbo->id);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(struct Voxel_vertex), (GLvoid*)0);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Voxel_vertex), (GLvoid*)12);

    Voxel_volume* vv;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL) continue;
        vv = render_list[i];
        if(vv->vvl.vnum == 0) printf("no vertices \n");

        //glVertexAttribPointer(InCood0 , 3, GL_FLOAT, GL_FALSE, sizeof(struct Voxel_vertex), (GLvoid*)0);
        //glVertexAttribPointer(InRGBA , 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Voxel_vertex), (GLvoid*)12);

        //printf("drawing %i vertices from offset %i \n", vv->vvl.vnum, vv->vvl.voff);

        //GL_POINTS

//int InRotationMatrix;
//int InTranslation;

        glUniformMatrix3fv(InRotationMatrix, 1, false, (GLfloat*) &vv->v[0]);
        glUniform3fvARB(InTranslation, 1, (GLfloat*)&vv->v[3]);

        glDrawArrays( GL_QUADS, vv->vvl.voff, vv->vvl.vnum );
        //glDrawArrays( GL_POINTS, vv->vvl.voff, vv->vvl.vnum );
    }

    //glEnable (GL_DEPTH_TEST);

    //glDisableVertexAttribArray(InCood0);
    //glDisableVertexAttribArray(InRGBA);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glUseProgramObjectARB(0);
    glPointSize(1.0);
}

void voxel_renderer_draw_test()
{
    

    static Voxel_volume voxel_volume (4,4,4, 1.0);
    static Voxel_render_list voxel_render_list;

    static int init = 0;
    if(init == 0)
    {

        voxel_render_list.init_voxel_render_list_shader1();
        printf("voxel_render_list.register(&voxel_volume); \n");
        voxel_render_list.register_voxel_volume(&voxel_volume);

        //printf("voxel_render_list.update_vertex_buffer_object(); \n");
        //voxel_render_list.update_vertex_buffer_object();
    }

    init = 1;

    //printf("voxel_render_list.update_vertex_buffer_object(); \n");
    voxel_render_list.update_vertex_buffer_object();

    //printf("draw \n");
    voxel_render_list.draw();
    return;

}