#include "verlet_particle.hpp"

#include <c_lib/physics/vec3.hpp>

#include <t_map/t_properties.hpp>

namespace VerletParticle
{

static inline void velocity_integrate(Vec3* p, Vec3* v, Vec3* a, float dt) __attribute((always_inline));;

static inline void velocity_integrate(Vec3* p, Vec3* v, Vec3* a, float dt)
{
    //const Vec3 va = vec3_init(0,0,gravity);

    *p = vec3_add(*p, vec3_add(vec3_scalar_mult(*v, dt), vec3_scalar_mult(*a, 0.5*dt*dt)));  // r(t) + v(t)dt + (1/2)gravity(t)dtdt
    /* the following block is for nonconstant gravity (spring forces) */
    //v5 = vec3_scalar_add(vv, 0.5*gravity*dt);
    // gravity(t+dt) = gravity [constant]
    //v = vec3_scalar_add(v5, 0.5*gravity*dt);
    /* end block */
    *v = vec3_add(*v, vec3_scalar_mult(*a, 0.5*dt));
}

static inline void velocity_integrate(Vec3* p, Vec3* v, float dt) __attribute((always_inline));;

static inline void velocity_integrate(Vec3* p, Vec3* v, float dt)
{
    const Vec3 a = vec3_init(0,0,gravity);

    *p = vec3_add(*p, vec3_add(vec3_scalar_mult(*v, dt), vec3_scalar_mult(a, 0.5*dt*dt)));  // r(t) + v(t)dt + (1/2)gravity(t)dtdt
    /* the following block is for nonconstant gravity (spring forces) */
    //v5 = vec3_scalar_add(vv, 0.5*gravity*dt);
    // gravity(t+dt) = gravity [constant]
    //v = vec3_scalar_add(v5, 0.5*gravity*dt);
    /* end block */
    *v = vec3_add(*v, vec3_scalar_mult(a, 0.5*dt));
}

bool VerletParticle::bool_bounce()
{
    bool bounced = false;
    
    Vec3 old_position = position;
    Vec3 old_velocity = velocity;
    velocity_integrate(&position, &velocity, dt);

    float interval = 0.0f;

    Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        position.x, position.y, position.z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        bounced = true;
        position = old_position;
        velocity = old_velocity;
        velocity_integrate(&position, &velocity, dt*interval);
        //Vec3 norm; = vec3_init(s[0], s[1], s[2]);
        velocity = vec3_reflect(velocity, norm);
        velocity = vec3_scalar_mult(velocity, dampening);
    }
	return bounced;
}

void VerletParticle::bounce()
{
    //bool bounced = false;
    
    Vec3 old_position = position;
    Vec3 old_velocity = velocity;
    
    velocity_integrate(&position, &velocity, dt);

    float interval = 0.0f;

    Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        position.x, position.y, position.z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        //bounced = true;
        position = old_position;
        velocity = old_velocity;
        velocity_integrate(&position, &velocity, dt*interval);
        //Vec3 norm = vec3_init(s[0], s[1], s[2]);
        velocity = vec3_reflect(velocity, norm);
        velocity = vec3_scalar_mult(velocity, dampening);
    }
	//return bounced;
}

void VerletParticle::bounce_box(float radius)
{
    //bool bounced = false;
    
    Vec3 old_position = position;
    Vec3 old_velocity = velocity;
    
    //const float ac = 9.8;

    Vec3 a = vec3_init(0,0,gravity);

    {
        const float ac = -9.8;

        const int _z = position.z;
        const int _y = position.y;
        const int _x = position.x;

        if( isSolid( position.x + radius, _y, _z) ) a.x += ac;
        if( isSolid( position.x - radius, _y, _z) ) a.x -= ac;

        if( isSolid( _x, position.y + radius, _z) ) a.y += ac;
        if( isSolid( _x, position.y - radius, _z) ) a.y -= ac;
    }
    velocity_integrate(&position, &velocity, &a, dt);

    float interval = 0.0f;

    Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        position.x, position.y, position.z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        //bounced = true;
        position = old_position;
        velocity = old_velocity;
        velocity_integrate(&position, &velocity, dt*interval);
        //Vec3 norm = vec3_init(s[0], s[1], s[2]);
        velocity = vec3_reflect(velocity, norm);
        velocity = vec3_scalar_mult(velocity, dampening);
    }
    //return bounced;
}

}