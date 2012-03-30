#include "verlet.hpp"

namespace Verlet
{

/* reference: bottom of http://www.fisica.uniud.it/~ercolessi/md/md/node21.html */
inline void VerletParticle::velocity_verlet(float dt)
{
    const Vec3 va = vec3_init(0,0,a);
    p = vec3_add(p, vec3_add(vec3_scalar_mult(v, dt), vec3_scalar_mult(va, 0.5*dt*dt)));  // r(t) + v(t)dt + (1/2)a(t)dtdt
    /* the following block is for nonconstant acceleration (spring forces) */
    //v5 = vec3_scalar_add(vv, 0.5*a*dt);
    // a(t+dt) = a [constant]
    //v = vec3_scalar_add(v5, 0.5*a*dt);
    /* end block */
    v = vec3_add(v, vec3_scalar_mult(va, 0.5*dt));
}

//void VerletParticle::position_verlet(float dt){}
//void VerletParticle::leapfrog_verlet(float dt){}

void VerletParticle::compute()
{
    old_p = p;
    old_v = v;
    velocity_verlet(dt);
    //position_verlet(dt);
    //leapfrog_verlet(dt);
}

// recomputes from old state, at a fraction of the timestep
// used by bounce methods
void VerletParticle::recompute(float fraction)
{
    p = old_p;
    v = old_v;
    velocity_verlet(dt*fraction);
    //position_verlet(dt*fraction);
    //leapfrog_verlet(dt*fraction);
}

void VerletParticle::set_position(float x, float y, float z)
{
    old_p = p;
    p.x = x;
    p.y = y;
    p.z = z;
}

void VerletParticle::set_momentum(float mx, float my, float mz)
{
    old_v = v;
    v.x = mx / mass;
    v.y = my / mass;
    v.z = mz / mass;
}

void VerletParticle::set_state(float x, float y, float z, float mx, float my, float mz)
{
    this->set_position(x,y,z);
    this->set_momentum(mx,my,mz);
}

void VerletParticle::set_mass(float mass)
{
    this->mass = mass;
}

Vec3 VerletParticle::get_momentum()
{
    Vec3 mom;
    mom.x = this->v.x * this->mass;
    mom.y = this->v.y * this->mass;
    mom.z = this->v.z * this->mass;
    return mom;
}

VerletParticle::VerletParticle(float x, float y, float z, float mx, float my, float mz, float mass)
:
mass(mass)
{
    this->set_state(x,y,z,mx,my,mz);
    old_p = p;
    old_v = v;
}

VerletParticle::VerletParticle(float x, float y, float z, float mx, float my, float mz)
{
    this->set_state(x,y,z,mx,my,mz);
    old_p = p;
    old_v = v;
}


//int* move(Vec3* v, float damp){}

bool bounce(VerletParticle* v, float damp)
{
    // save current state
    // calculate next state
    // check collision over interval
    
    // if interval < 1: bounce
    // recompute delta with fractional interval (introduces minor inaccuracies)
    // reflect
    // dampen

    bool bounced = false;
    
    v->compute();

    float interval = 0.0f;
    int *s = _ray_cast4(
        v->old_p.x, v->old_p.y, v->old_p.z,
        v->p.x, v->p.y, v->p.z,
        &interval
    );

    if (interval < 1.0f)
    {   // collision
        bounced = true;
        v->recompute(interval);
        Vec3 norm = vec3_init(s[0], s[1], s[2]);
        v->v = vec3_reflect(v->v, norm);
        v->v = vec3_scalar_mult(v->v, damp);
    }
    
    return bounced;
}

int* bounce(VerletParticle* v, int* collision, int* tile, float damp)
{   // same as simple bounce, but gets extra metadata on the bounce
    v->compute();

    float interval = 0.0f;
    int *s = _ray_cast5_capped(
        v->old_p.x, v->old_p.y, v->old_p.z,
        v->p.x, v->p.y, v->p.z,
        &interval, collision, tile
    );

    if (interval < 1.0f)
    {   // collision
        v->recompute(interval);
        Vec3 norm = vec3_init(s[0], s[1], s[2]);
        v->v = vec3_reflect(v->v, norm);
        v->v = vec3_scalar_mult(v->v, damp);
    }
    
    return s;
}

}
