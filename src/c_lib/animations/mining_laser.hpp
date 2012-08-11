#pragma once

#ifdef DC_CLIENT
#include <common/gl_assert.hpp>
#endif

#include <physics/vec3.hpp>
#include <animations/common.hpp>

#include <physics/verlet.hpp>

namespace Animations 
{

VertexElementList1* mining_laser_vlist = NULL;

//unsigned int mining_laser_vbo;
class SHADER mining_laser_shader;

int mining_laser_surface;
unsigned int mining_laser_texture;

unsigned int mining_laser_TexCoord;


void init_mining_laser_texture();
void init_mining_laser_shader();

void init_mining_laser()
{
    mining_laser_vlist = new VertexElementList1;

    //glGenBuffers(1, &mining_laser_vbo);

    init_mining_laser_texture();
    init_mining_laser_shader();
}

void teardown_mining_laser()
{
    delete mining_laser_vlist;
}

void init_mining_laser_texture()
{
    //int ret = create_texture_from_file((char*) "./media/sprites/mining_laser2.png", &mining_laser_texture);
    int ret = create_texture_from_file((char*) "./media/sprites/mining_laser4.png", &mining_laser_texture);
    GS_ASSERT(ret == 0);
}

void init_mining_laser_shader()
{
    mining_laser_shader.set_debug(false);

    mining_laser_shader.load_shader( "mining shader",
        "./media/shaders/weapon/mining_laser.vsh",
        "./media/shaders/weapon/mining_laser.fsh" );
    
    mining_laser_TexCoord = mining_laser_shader.get_attribute("InTexCoord");
}


const int MINING_LASER_TTL = 120; // 10 seconds
const float MINING_LASER_DAMPENING = 0.20f;

class MiningLaser
{
    public:

    public:
        Verlet::VerletComponent verlet;

        int id;
        int type;

        int ttl;

        float damp;

    inline void init(float x, float y, float z, float mx, float my, float mz, const float speed, const float length)
    {
        type = 0+rand()%4;
        verlet.dampening = MINING_LASER_DAMPENING;

        verlet.position = vec3_init(x,y,z);
        verlet.velocity = vec3_init(mx,my,mz);

        verlet.velocity = vec3_scalar_mult(verlet.velocity, speed);
        this->ttl = (length / speed) * 30;
    }

    void tick()
    {
        //verlet.bounce_box(0.125);
        
        
        if (verlet.collide_no_gravity()) ttl = 0;
        
        //verlet.bounce_box_no_gravity(0.125)
        //verlet.radial(rx, ry);

        this->ttl--;
    }

    void prep(Vec3 cam)
    {
        const float scale = 0.25f;
        const float _h = scale / 2;

        Vec3 up = vec3_init(
            model_view_matrix[0]*scale,
            model_view_matrix[4]*scale,
            model_view_matrix[8]*scale
        );
        Vec3 right = vec3_init(
            model_view_matrix[1]*scale,
            model_view_matrix[5]*scale,
            model_view_matrix[9]*scale
        );

        int texture_index = this->type;
        //int texture_index = 4;

        float tx_min, tx_max, ty_min, ty_max;
        tx_min = (float)(texture_index%4)* (1.0f/4.0f);
        tx_max = tx_min + (1.0f/4.0f);
        ty_min = (float)(texture_index/4)* (1.0f/4.0f);
        ty_max = ty_min + (1.0f/4.0f);

        Vec3 position = quadrant_translate_position(current_camera_position, verlet.position);
        position.z += _h;

        Vec3 p = vec3_sub(position, vec3_add(right, up));
        mining_laser_vlist->push_vertex(p, tx_min,ty_max);

        p = vec3_add(position, vec3_sub(up, right));
        mining_laser_vlist->push_vertex(p, tx_max,ty_max);

        p = vec3_add(position, vec3_add(up, right));
        mining_laser_vlist->push_vertex(p, tx_max,ty_min);

        p = vec3_add(position, vec3_sub(right, up));
        mining_laser_vlist->push_vertex(p, tx_min,ty_min);

    }

    MiningLaser() : ttl(MINING_LASER_TTL) {}

};

}

#include <common/template/object_list.hpp>

namespace Animations 
{

const int MINING_LASER_MAX = 1024;

class MiningLaserEffect_list: public Simple_object_list<MiningLaser, MINING_LASER_MAX>
{
    private:
        const char* name() { return "MiningLaserEffect_list"; }

    public:
        MiningLaserEffect_list() { print();}

        void prep();
        void draw();
        void tick();

};


void MiningLaserEffect_list::prep()
{
    //mining_laser_vlist->reset_index();

    Vec3 cam = current_camera->get_position();
    for (unsigned int i=0; i<this->num; i++)
        this->a[i].prep(cam);

    mining_laser_vlist->buffer(); //upload data to GPU and reset list
}


void MiningLaserEffect_list::draw()
{
    if (!mining_laser_shader.shader_valid) return;

    if(mining_laser_vlist->vertex_number == 0) return;
    const static unsigned int stride = sizeof(struct vertexElement1);

    GS_ASSERT(mining_laser_vlist->VBO != 0);
    if (mining_laser_vlist->VBO == 0) return;

    GL_ASSERT(GL_DEPTH_WRITEMASK, false);
    GL_ASSERT(GL_BLEND, true);

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    glBindBuffer(GL_ARRAY_BUFFER, mining_laser_vlist->VBO);

    glBindTexture( GL_TEXTURE_2D, mining_laser_texture );


    glUseProgramObjectARB(mining_laser_shader.shader);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(mining_laser_TexCoord);

    glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);
    glVertexAttribPointer(mining_laser_TexCoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)12);

    glDrawArrays(GL_QUADS,0, mining_laser_vlist->vertex_number);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(mining_laser_TexCoord);
    glUseProgramObjectARB(0);

    glDisable(GL_TEXTURE_2D);
}

void MiningLaserEffect_list::tick()
{
    for (unsigned int i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) this->destroy(i);
    }
}

}   // Animations
