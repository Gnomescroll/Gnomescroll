#include "verlet.hpp"

namespace Verlet
{

const float dt = 1/30.0f;
const float gravity = -9.8f;

static inline void velocity_integrate(Vec3* p, Vec3* v, Vec3 a, float dt)
{
    *p = vec3_add(*p, vec3_add(vec3_scalar_mult(*v, dt), vec3_scalar_mult(a, 0.5*dt*dt)));  // r(t) + v(t)dt + (1/2)gravity(t)dtdt
    /* the following block is for nonconstant gravity (spring forces) */
    //v5 = vec3_scalar_add(vv, 0.5*gravity*dt);
    // gravity(t+dt) = gravity [constant]
    //v = vec3_scalar_add(v5, 0.5*gravity*dt);
    /* end block */
    *v = vec3_add(*v, vec3_scalar_mult(a, 0.5*dt));
}

static inline void velocity_integrate(Vec3* p, Vec3* v, float dt)
{
    const Vec3 a = vec3_init(0,0,gravity);
    velocity_integrate(p, v, a, dt);
}


bool bounce(Vec3* position, Vec3* velocity, float damp)
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
    
    Vec3 old_position = *position;
    Vec3 old_velocity = *velocity;
    velocity_integrate(position, velocity, dt);

    float interval = 0.0f;

    Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        position->x, position->y, position->z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        bounced = true;
        *position = old_position;
        *velocity = old_velocity;
        velocity_integrate(position, velocity, dt*interval);
        *position = translate_position(*position);
        //Vec3 norm = vec3_init(s[0], s[1], s[2]);
        *velocity = vec3_reflect(*velocity, norm);
        *velocity = vec3_scalar_mult(*velocity, damp);
    }
    
    return bounced;
}

int* bounce(Vec3* position, Vec3* velocity, float damp, int* collision, int* tile)
{   // same as simple bounce, but gets extra metadata on the bounce
    Vec3 old_position = *position;
    Vec3 old_velocity = *velocity;
    velocity_integrate(position, velocity, dt);
    
    float interval = 0.0f;

    Vec3 norm;
    int *s = _ray_cast5_capped(
        old_position.x, old_position.y, old_position.z,
        (*position).x, (*position).y, (*position).z,
        &interval, collision, tile, &norm
    );

    if (interval < 1.0f)
    {   // collision
        *position = old_position;
        *velocity = old_velocity;
        velocity_integrate(position, velocity, dt*interval);
        *position = translate_position(*position);
        //Vec3 norm = vec3_init(s[0], s[1], s[2]);
        *velocity = vec3_reflect(*velocity, norm);
        *velocity = vec3_scalar_mult(*velocity, damp);
    }
    
    return s;
}


bool bounce_box(Vec3* position, Vec3* velocity, float damp, float radius)
{
    bool bounced = false;
    Vec3 old_position = *position;
    Vec3 old_velocity = *velocity;
    
    Vec3 a = vec3_init(0,0,gravity);
    if (isSolid(old_position.x + radius, old_position.y,          old_position.z)) a.x += gravity;
    if (isSolid(old_position.x - radius, old_position.y,          old_position.z)) a.x -= gravity;
    if (isSolid(old_position.x,          old_position.y + radius, old_position.z)) a.y += gravity;
    if (isSolid(old_position.x,          old_position.y - radius, old_position.z)) a.y -= gravity;

    velocity_integrate(position, velocity, a, dt);

    float interval = 0.0f;

    Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        position->x, position->y, position->z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        bounced = true;
        velocity_integrate(&old_position, &old_velocity, dt*interval);
        old_velocity = vec3_reflect(old_velocity, norm);
        old_velocity = vec3_scalar_mult(old_velocity, damp);
        *velocity = old_velocity;
        *position = translate_position(old_position);
    }
    return bounced;
}


}
