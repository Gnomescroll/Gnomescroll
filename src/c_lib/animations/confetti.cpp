#include "shrapnel.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <physics/vec3.hpp>
#include <animations/common.hpp>

namespace Particle
{

using Animations::VertexElementList1;

const float CONFETTI_MASS = 0.2f;
const int CONFETTI_TTL = 30;
const float CONFETTI_DAMP = 0.5f;

class SHADER confetti_shader;
unsigned int confetti_TexCoord;

/* Confetti vlist */

VertexElementList1* confetti_vlist = NULL;

void init_confetti_shader()
{
    confetti_shader.set_debug(false);

    confetti_shader.load_shader( "confetti shader",
        "./media/shaders/weapon/confetti.vsh",
        "./media/shaders/weapon/confetti.fsh" );
    confetti_TexCoord = confetti_shader.get_attribute("InTexCoord");

}

void init_confetti()
{
    GS_ASSERT(confetti_vlist == NULL);
    confetti_vlist = new VertexElementList1;
    init_confetti_shader();
}

void teardown_confetti()
{
    if (confetti_vlist != NULL) delete confetti_vlist;
}

/* Confetti */

inline void Confetti::init(float x, float y, float z, float mx, float my, float mz)
{
    verlet.dampening = CONFETTI_DAMP;

    verlet.position = vec3_init(x,y,z);
    verlet.velocity = vec3_init(mx,my,mz);

    this->ttl = CONFETTI_TTL;
    this->scale = CONFETTI_TEXTURE_SCALE;
    this->texture_index = CONFETTI_TEXTURE_ID;

}

void Confetti::tick()
{
    this->verlet.bounce();
    ttl--;
}

void Confetti::prep()
{
    Vec3 position = quadrant_translate_position(current_camera_position, this->verlet.position);
    if (point_fulstrum_test(position.x, position.y, position.z) == false)
        return;

    Vec3 up = vec3_init(
        model_view_matrix[0]*this->scale,
        model_view_matrix[4]*this->scale,
        model_view_matrix[8]*this->scale
    );
    Vec3 right = vec3_init(
        model_view_matrix[1]*this->scale,
        model_view_matrix[5]*this->scale,
        model_view_matrix[9]*this->scale
    );

    float tx_min, tx_max, ty_min, ty_max;
    tx_min = (float)(this->texture_index%16)* (1.0f/16.0f);
    tx_max = tx_min + (1.0f/16.0f);
    ty_min = (float)(this->texture_index/16)* (1.0f/16.0f);
    ty_max = ty_min + (1.0f/16.0f);

    position.z += this->scale / 2.0f;

    Vec3 p = vec3_sub(position, vec3_add(right, up));
    confetti_vlist->push_vertex(p, tx_min,ty_max);

    p = vec3_add(position, vec3_sub(up, right));
    confetti_vlist->push_vertex(p, tx_max,ty_max);

    p = vec3_add(position, vec3_add(up, right));
    confetti_vlist->push_vertex(p, tx_max,ty_min);

    p = vec3_add(position, vec3_sub(right, up));
    confetti_vlist->push_vertex(p, tx_min,ty_min);
}

/* Confetti list */

void Confetti_list::tick()
{
    for(unsigned int i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) destroy(i);
    }
}

void Confetti_list::prep()
{
    for(unsigned int i=0; i<this->num; a[i++].prep());
    confetti_vlist->buffer();
}
 

void Confetti_list::draw()
{
    if (!confetti_shader.shader_valid) return;
    
    if(confetti_vlist->vertex_number == 0) return;

    GS_ASSERT(particle_texture != 0);
    if (particle_texture == 0) return;
    GS_ASSERT(confetti_vlist->VBO != 0);
    if (confetti_vlist->VBO == 0) return;
    
    const unsigned int stride = confetti_vlist->stride;

    glBindBuffer(GL_ARRAY_BUFFER, confetti_vlist->VBO);

    GL_ASSERT(GL_DEPTH_WRITEMASK, true);
    GL_ASSERT(GL_BLEND, false);


    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, particle_texture);

    glUseProgramObjectARB(confetti_shader.shader);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(confetti_TexCoord);
    
    GS_ASSERT(sizeof(float) == sizeof(GL_FLOAT));
    GS_ASSERT(sizeof(Vec3) == sizeof(GL_FLOAT)*3);
    glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);
    //glTexCoordPointer(2, GL_FLOAT, stride, (GLvoid*)12);
    glVertexAttribPointer(confetti_TexCoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)12);

    glDrawArrays(GL_QUADS, 0, confetti_vlist->vertex_number);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(confetti_TexCoord);
    glUseProgramObjectARB(0);

    glDisable(GL_TEXTURE_2D);
}

}
