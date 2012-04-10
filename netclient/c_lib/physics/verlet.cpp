#include "verlet.hpp"

namespace Verlet
{

const float dt = 1/30.0f;
const float gravity = -9.8f;

static inline void velocity_integrate(Vec3* p, Vec3* v, float dt)
{
    const Vec3 va = vec3_init(0,0,gravity);
    *p = vec3_add(*p, vec3_add(vec3_scalar_mult(*v, dt), vec3_scalar_mult(va, 0.5*dt*dt)));  // r(t) + v(t)dt + (1/2)gravity(t)dtdt
    /* the following block is for nonconstant gravity (spring forces) */
    //v5 = vec3_scalar_add(vv, 0.5*gravity*dt);
    // gravity(t+dt) = gravity [constant]
    //v = vec3_scalar_add(v5, 0.5*gravity*dt);
    /* end block */
    *v = vec3_add(*v, vec3_scalar_mult(va, 0.5*dt));
}

bool bounce(Vec3* old_position, Vec3* old_velocity, Vec3* position, Vec3* velocity, float damp)
{
    // save current state
    // calculate next state
    // check collision over interval
    
    // if interval < 1: bounce
    // recompute delta with fractional interval (introduces minor inaccuracies)
    // reflect
    // dampen

    // TODO -- reduce pointer derefences

    bool bounced = false;
    
    *old_position = *position;
    *old_velocity = *velocity;
    velocity_integrate(position, velocity, dt);

    float interval = 0.0f;
    int *s = _ray_cast4(
        (*old_position).x, (*old_position).y, (*old_position).z,
        (*position).x, (*position).y, (*position).z,
        &interval
    );

    if (interval < 1.0f)
    {   // collision
        bounced = true;
        *position = *old_position;
        *velocity = *old_velocity;
        velocity_integrate(position, velocity, dt*interval);
        Vec3 norm = vec3_init(s[0], s[1], s[2]);
        *velocity = vec3_reflect(*velocity, norm);
        *velocity = vec3_scalar_mult(*velocity, damp);
    }
    
    return bounced;
}

int* bounce(Vec3* old_position, Vec3* old_velocity, Vec3* position, Vec3* velocity, float damp, int* collision, int* tile)
{   // same as simple bounce, but gets extra metadata on the bounce
    *old_position = *position;
    *old_velocity = *velocity;
    velocity_integrate(position, velocity, dt);
    
    float interval = 0.0f;
    int *s = _ray_cast5_capped(
        (*old_position).x, (*old_position).y, (*old_position).z,
        (*position).x, (*position).y, (*position).z,
        &interval, collision, tile
    );

    if (interval < 1.0f)
    {   // collision
        *position = *old_position;
        *velocity = *old_velocity;
        velocity_integrate(position, velocity, dt*interval);
        Vec3 norm = vec3_init(s[0], s[1], s[2]);
        *velocity = vec3_reflect(*velocity, norm);
        *velocity = vec3_scalar_mult(*velocity, damp);
    }
    
    return s;
}

}
