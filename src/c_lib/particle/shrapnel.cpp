#include "shrapnel.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <physics/vec3.hpp>
#include <animations/common.hpp>

namespace Particle
{

using Animations::VertexElementList1;

const float SHRAPNEL_MASS = 0.2f;
const int SHRAPNEL_TTL = 30;
const float SHRAPNEL_DAMP = 0.5f;
const int SHRAPNEL_TEXTURE_ID = 5;
const float SHRAPNEL_TEXTURE_SCALE = 0.15f;

class SHADER shrapnel_shader;
unsigned int shrapnel_TexCoord;

/* Shrapnel vlist */

VertexElementList1* shrapnel_vlist = NULL;

void init_shrapnel_shader()
{
    shrapnel_shader.set_debug(false);

    shrapnel_shader.load_shader( "shrapnel shader",
        "./media/shaders/weapon/shrapnel.vsh",
        "./media/shaders/weapon/shrapnel.fsh" );
    shrapnel_TexCoord = shrapnel_shader.get_attribute("InTexCoord");

}

void init_shrapnel()
{
    GS_ASSERT(shrapnel_vlist == NULL);
    shrapnel_vlist = new VertexElementList1;
    init_shrapnel_shader();
}

void teardown_shrapnel()
{
    if (shrapnel_vlist != NULL) delete shrapnel_vlist;
}

/* Shrapnel */

inline void Shrapnel::reset()
{
    this->ttl = SHRAPNEL_TTL;
    this->scale = SHRAPNEL_TEXTURE_SCALE;
    this->texture_index = SHRAPNEL_TEXTURE_ID;
    this->verlet.dampening = SHRAPNEL_DAMP;
}

void Shrapnel::tick()
{
    this->verlet.bounce();
    ttl--;
}

void Shrapnel::prep()
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
    shrapnel_vlist->push_vertex(p, tx_min,ty_max);

    p = vec3_add(position, vec3_sub(up, right));
    shrapnel_vlist->push_vertex(p, tx_max,ty_max);

    p = vec3_add(position, vec3_add(up, right));
    shrapnel_vlist->push_vertex(p, tx_max,ty_min);

    p = vec3_add(position, vec3_sub(right, up));
    shrapnel_vlist->push_vertex(p, tx_min,ty_min);
}

/* Shrapnel list */

void Shrapnel_list::tick()
{
    for(unsigned int i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0) destroy(i);
    }
}

void Shrapnel_list::prep()
{
    for(unsigned int i=0; i<this->num; a[i++].prep());
    shrapnel_vlist->buffer();
}
 

void Shrapnel_list::draw()
{
    if (!shrapnel_shader.shader_valid) return;
    
    if(shrapnel_vlist->vertex_number == 0) return;

    GS_ASSERT(particle_texture != 0);
    if (particle_texture == 0) return;
    GS_ASSERT(shrapnel_vlist->VBO != 0);
    if (shrapnel_vlist->VBO == 0) return;
    
    const unsigned int stride = shrapnel_vlist->stride;

    glBindBuffer(GL_ARRAY_BUFFER, shrapnel_vlist->VBO);

    GL_ASSERT(GL_DEPTH_WRITEMASK, false);
    GL_ASSERT(GL_BLEND, true);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, particle_texture);

    glUseProgramObjectARB(shrapnel_shader.shader);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableVertexAttribArray(shrapnel_TexCoord);
    
    GS_ASSERT(sizeof(float) == sizeof(GL_FLOAT));
    GS_ASSERT(sizeof(Vec3) == sizeof(GL_FLOAT)*3);
    glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);
    //glTexCoordPointer(2, GL_FLOAT, stride, (GLvoid*)12);
    glVertexAttribPointer(shrapnel_TexCoord, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)12);

    glDrawArrays(GL_QUADS, 0, shrapnel_vlist->vertex_number);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableVertexAttribArray(shrapnel_TexCoord);
    glUseProgramObjectARB(0);

    glDisable(GL_TEXTURE_2D);
}

}
