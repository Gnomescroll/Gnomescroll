#include "voxel_render.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/camera/fulstrum_test.hpp>

void Voxel_render_list::register_voxel_volume(Voxel_volume* vv)
{
    if(num_elements >= VOXEL_RENDER_LIST_SIZE)
    {
        printf("Voxel_render_list Error: number of voxel models exceeds VOXEL_RENDER_LIST_SIZE \n");
        return;
    }
    int i;
    for(i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL)
        {
            num_elements++;
            render_list[i] = vv;
            vv->id = i;
            vv->voxel_render_list = this;
            //printf("Added voxel volume %i \n", i);
            break;
        }
    }
    if (i == VOXEL_RENDER_LIST_SIZE) printf("WARNING: register_voxel_volume - no space available\n");

}

void Voxel_render_list::unregister_voxel_volume(Voxel_volume* vv)
{
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if (render_list[i] == NULL) continue;
        if(render_list[i] == vv)
        {
            num_elements--;
            render_list[i] = NULL;
            //printf("Removed voxel volume %i from render list\n", i);
            vv->id = -1;
            vv->voxel_render_list = NULL;
            return;
        }
    }

    printf("Voxel_render_list::unregister_voxel_volume error, volume was not on list \n");
    vv->id = -1;
    vv->voxel_render_list = NULL;
}

void Voxel_render_list::update_vertex_buffer_object()
{
    Voxel_volume* vv;

    VBOmeta* _vbo = &vbo_wrapper[0]; 
    int v_num = 0;
    int volumes_updated = 0;
    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if(render_list[i] == NULL) continue;

        vv = render_list[i];
        if( vv->needs_vbo_update == true )
        {
            vv->needs_vbo_update = false;
            volumes_updated++;
            vv->update_vertex_list(); //<-- look for bug here
            if(vv->vvl.vnum == 0) printf("Voxel_render_list::update_vertex_buffer_object, FATAL ERROR, voxel volume has no voxel!\n");
        }
        v_num +=  vv->vvl.vnum;
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
        if(render_list[i] == NULL) continue;
        vv = render_list[i];

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
int InNormal;
int InAO;
int InSide;

void Voxel_render_list::init_voxel_render_list_shader1()
{
    static int init=0;
    if (init)
    {
        printf("Voxel_render_list::init_voxel_render_list_shader1, error, tried to init twice\n");
        return;
    }
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

    //uniforms
    InRotationMatrix = glGetUniformLocationARB(voxel_shader_prog, "InRotationMatrix");

    //attributes
    InAO = glGetAttribLocation(voxel_shader_prog, "InAO");
    InNormal = glGetAttribLocation(voxel_shader_prog, "InNormal");
    InSide = glGetAttribLocation(voxel_shader_prog, "InNormal");

    init=1;
}

void Voxel_render_list::draw()
{
    glDisable(GL_TEXTURE_2D);

    VBOmeta* _vbo = &vbo_wrapper[0]; 

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
    //glEnableClientState(GL_NORMAL_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(struct Voxel_vertex), (GLvoid*)0);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct Voxel_vertex), (GLvoid*)12);
    
    //printf("innormal = %i \n", InNormal);
    glVertexAttribPointer(InNormal, 3, GL_BYTE, GL_FALSE, sizeof(struct Voxel_vertex), (GLvoid*)16);
    glVertexAttribPointer(InAO, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(struct Voxel_vertex), (GLvoid*)20);
    //glNormalPointer(GL_BYTE, sizeof(struct Voxel_vertex), (GLvoid*)16);

    Voxel_volume* vv;
    Vector4 v[4];

    v[0].w = 0.0f;
    v[1].w = 0.0f;
    v[2].w = 0.0f;
    v[3].w = 1.0f;

    for(int i=0; i < VOXEL_RENDER_LIST_SIZE; i++)
    {
        if( render_list[i] == NULL || !render_list[i]->draw ) continue;
        vv = render_list[i];

        if( sphere_fulstrum_test(vv->center.x, vv->center.y, vv->center.z, vv->radius) == false ) continue;
        
        if(vv->vvl.vnum == 0) continue;

        v[0].v3 = vector_scalar2(&vv->v[0], vv->scale);
        v[1].v3 = vector_scalar2(&vv->v[1], vv->scale);
        v[2].v3 = vector_scalar2(&vv->v[2], vv->scale);
        v[3].v3 = vv->v[3];
    
        glUniformMatrix4fv(InRotationMatrix, 1, false, (GLfloat*) &v);
        glDrawArrays( GL_QUADS, vv->vvl.voff, vv->vvl.vnum );
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    //glDisableClientState(GL_NORMAL_ARRAY);

    glDisableVertexAttribArray(InNormal);
    glDisableVertexAttribArray(InAO);

    glEnable (GL_DEPTH_TEST);

    glUseProgramObjectARB(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

/*
    Apply changes
*/
    this->update_vertex_buffer_object(); 
}

void voxel_renderer_draw_test()
{   
    printf("voxel test \n");

    static Voxel_volume vv (4,4,2, 2.0);
    static Voxel_render_list voxel_render_list;

    static int init = 0;
    if(init == 0)
    {
        voxel_render_list.init_voxel_render_list_shader1();
        printf("voxel_render_list.register(&voxel_volume); \n");
        voxel_render_list.register_voxel_volume(&vv);
        voxel_render_list.draw();
    }
    init = 1;

    static float c0 = 0.0;
    static float c1 = 0.0;
    static float c2 = 0.0;

    c0 += 0.0050 / (2*PI);
    c1 += 0.0025 / (2*PI);
    c2 += 0.0100 / (2*PI);
    
    vv.set_rotated_unit_axis(c0, c1, c2);
    
    vv.set_center( 8.0, 8.0, 8.0);

    voxel_render_list.draw();
    vv.draw_bounding_box();
}
