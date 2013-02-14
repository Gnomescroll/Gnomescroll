#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/gl_assert.hpp>
#include <physics/vec3.hpp>
#include <animations/common.hpp>
#include <physics/verlet.hpp>
#include <SDL/texture_loader.hpp>
#include <camera/camera.hpp>

namespace Animations
{

VertexElementList1* mining_laser_vlist = NULL;

//unsigned int mining_laser_vbo;
class Shader mining_laser_shader;

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
    int ret = create_texture_from_file(MEDIA_PATH "sprites/animation/mining_laser.png", &mining_laser_texture);
    GS_ASSERT(ret == 0);
}

void init_mining_laser_shader()
{
    mining_laser_shader.set_debug(false);
    mining_laser_shader.load_shader("mining shader",
        MEDIA_PATH "shaders/weapon/mining_laser.vsh",
        MEDIA_PATH "shaders/weapon/mining_laser.fsh");
    mining_laser_TexCoord = mining_laser_shader.get_attribute("InTexCoord");
}

const int MINING_LASER_TTL = 120; // 10 seconds
const float MINING_LASER_DAMPENING = 0.20f;

class MiningLaser
{
    public:
        Verlet::VerletComponent verlet;
        int id;
        int type;
        int ttl;

    void reset()
    {
        this->verlet.dampening = MINING_LASER_DAMPENING;
        this->ttl = 0;
    }

    inline void update_type(int row_index)
    {
        this->type = (row_index*4)+(randrange(0,3));
    }

    inline void set_state(float x, float y, float z, float mx, float my, float mz, const float speed, const float length)
    {
        verlet.position = vec3_init(x,y,z);
        verlet.velocity = vec3_init(mx,my,mz);
        verlet.velocity = vec3_scalar_mult(verlet.velocity, speed);
    }

    void tick()
    {
        //if (verlet.collide_no_gravity()) ttl = this->ttl_max;
        this->verlet.move_no_gravity();
        this->ttl++;
    }

    void prep()
    {
        this->prep(0.5f);
    }

    void prep(const float h_mult)
    {
        struct Vec3 position = quadrant_translate_position(current_camera_position, this->verlet.position);

        const float scale = 0.22f;
        const float h = scale * h_mult;

        Vec3 up = vec3_init(model_view_matrix[0]*scale,
                            model_view_matrix[4]*scale,
                            model_view_matrix[8]*scale);
        Vec3 right = vec3_init(model_view_matrix[1]*scale,
                               model_view_matrix[5]*scale,
                               model_view_matrix[9]*scale);

        float tx_min, tx_max, ty_min, ty_max;
        tx_min = float(this->type % 4) * (1.0f/4.0f);
        tx_max = tx_min + (1.0f/4.0f);
        ty_min = float(this->type / 4) * (1.0f/4.0f);
        ty_max = ty_min + (1.0f/4.0f);

        position.z += h;

        Vec3 p = vec3_sub(position, vec3_add(right, up));
        mining_laser_vlist->push_vertex(p, tx_min,ty_max);
        p = vec3_add(position, vec3_sub(up, right));
        mining_laser_vlist->push_vertex(p, tx_max,ty_max);
        p = vec3_add(position, vec3_add(up, right));
        mining_laser_vlist->push_vertex(p, tx_max,ty_min);
        p = vec3_add(position, vec3_sub(right, up));
        mining_laser_vlist->push_vertex(p, tx_min,ty_min);
    }

    MiningLaser() :
        ttl(0)
    {}

};

}

#include <common/template/object_list.hpp>

namespace Animations
{

const int MINING_LASER_MAX = 1024;

class MiningLaserEffectList: public Simple_object_list<MiningLaser, MINING_LASER_MAX>
{
    private:

    const char* name()
    {
        return "MiningLaserEffectList";
    }

    public:

    void prep();
    void draw();
    void tick();
};


void MiningLaserEffectList::prep()
{
    GS_ASSERT(mining_laser_vlist != NULL);
    if (mining_laser_vlist == NULL) return;

    for (size_t i=0; i<this->num; i++)
        this->a[i].prep();

    mining_laser_vlist->buffer(); //upload data to GPU and reset list
}


void MiningLaserEffectList::draw()
{
    GS_ASSERT(mining_laser_vlist != NULL);
    if (mining_laser_vlist == NULL) return;

    if (!mining_laser_shader.shader_valid) return;

    if (mining_laser_vlist->vertex_number == 0) return;
    const static unsigned int stride = sizeof(struct vertexElement1);

    GS_ASSERT(mining_laser_vlist->VBO != 0);
    if (mining_laser_vlist->VBO == 0) return;

    GL_ASSERT(GL_DEPTH_WRITEMASK, false);
    GL_ASSERT(GL_BLEND, true);

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    glBindBuffer(GL_ARRAY_BUFFER, mining_laser_vlist->VBO);

    glBindTexture(GL_TEXTURE_2D, mining_laser_texture);

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

void MiningLaserEffectList::tick()
{
    for (size_t i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) this->destroy(i);
    }
}

}   // Animations
