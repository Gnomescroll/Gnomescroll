#include "particles.hpp"

#include <c_lib/camera/camera.hpp>

void Particle::set_ttl(int ttl)
{   // set to a negative number for no timeout
    this->ttl = ttl;
}

void Particle::draw()
{
    #ifdef DC_CLIENT
    if (current_camera == NULL
    || !current_camera->in_view(this->vp->p.x, this->vp->p.y, this->vp->p.z))
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

    Vec3 p = vec3_sub(this->vp->p, vec3_add(right, up));    // Bottom left
    glTexCoord2f(tx_min,ty_max);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(this->vp->p, vec3_sub(up, right));         // Top left
    glTexCoord2f(tx_min,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(this->vp->p, vec3_add(up, right));         // Top right
    glTexCoord2f(tx_max,ty_min);
    glVertex3f(p.x, p.y, p.z);

    p = vec3_add(this->vp->p, vec3_sub(right, up));         // Bottom right
    glTexCoord2f(tx_max,ty_max);
    glVertex3f(p.x, p.y, p.z);
    #endif    
}

Particle::Particle(int id, float x, float y, float z, float mx, float my, float mz, float mass)
:
id(id),
ttl(0),
scale(1.0f),
texture_index(0)
{
    vp = new VerletParticle(x,y,z,mx,my,mz, mass);
}

Particle::~Particle()
{
    delete vp;
}

EventParticle::EventParticle(int id, float x, float y, float z, float mx, float my, float mz, float mass)
:
Particle(id, x,y,z,mx,my,mz, mass)
{}
