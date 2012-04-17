#include "shrapnel.hpp"

#include <physics/vec3.hpp>
#include <c_lib/animations/common.hpp>

namespace Particles
{

using Animations::VertexElementList1;

const float SHRAPNEL_MASS = 0.2f;


/* Shrapnel vlist */

VertexElementList1* shrapnel_vlist = NULL;

void init_shrapnel()
{
    shrapnel_vlist = new VertexElementList1;
}

void teardown_shrapnel()
{
    delete shrapnel_vlist;
}


/* Shrapnel */

void Shrapnel::init(float x, float y, float z, float mx, float my, float mz)
{
    verlet.dampening = SHRAPNEL_DAMP;

    verlet.position = vec3_init(x,y,z);
    verlet.velocity = vec3_init(mx,my,mz);

    this->ttl = SHRAPNEL_TTL;
    this->scale = SHRAPNEL_TEXTURE_SCALE;
    this->texture_index = SHRAPNEL_TEXTURE_ID;

}

void Shrapnel::tick()
{
    this->verlet.bounce();
    ttl--;
}

void Shrapnel::prep()
{
    if (point_fulstrum_test(verlet.position.x, verlet.position.y, verlet.position.z) == false)
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
    tx_min = (float)(this->texture_index%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(this->texture_index/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    Vec3 position = verlet.position;

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
    for(int i=0; i<this->num; i++)
    {
        a[i].tick();
        if (a[i].ttl <= 0)
            destroy(a[i].id);
    }
}

void Shrapnel_list::prep()
{
#if DC_CLIENT
    for(int i=0; i<this->num; i++)
    {
        a[i].prep();
    }

    shrapnel_vlist->buffer();
#endif
}
 

void Shrapnel_list::draw()
{
#if DC_CLIENT
    if(shrapnel_vlist->vertex_number == 0) return;

    const unsigned int stride = shrapnel_vlist->stride;

    //printf("draw: %i \n", shrapnel_vlist->vertex_number);
    //printf("draw2: %i \n", shrapnel_vlist->VBO);

    assert(particle_texture != 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);

    glBindTexture(GL_TEXTURE_2D, particle_texture);
    glBindBuffer(GL_ARRAY_BUFFER, shrapnel_vlist->VBO);

    glVertexPointer(3, GL_FLOAT, stride, (GLvoid*)0);
    glTexCoordPointer(2, GL_FLOAT, stride, (GLvoid*)12);

    glDrawArrays(GL_QUADS, 0, shrapnel_vlist->vertex_number);
    
    glDisable(GL_BLEND);

    glDisableClientState(GL_VERTEX_ARRAY);

#endif
}

}
