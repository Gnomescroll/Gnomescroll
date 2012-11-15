#include "verlet.hpp"

namespace Verlet
{

static const struct Vec3 default_a = vec3_init(0,0,gravity);
static const struct Vec3 no_gravity = vec3_init(0.0f, 0.0f, 0.0f);

static inline void velocity_integrate(struct Vec3* p, struct Vec3* v, const struct Vec3 a, float dt)   __attribute__((always_inline));

static inline void velocity_integrate(struct Vec3* p, struct Vec3* v, const struct Vec3 a, float dt)
{
    *p = vec3_add(*p, vec3_add(vec3_scalar_mult(*v, dt), vec3_scalar_mult(a, 0.5f*dt*dt)));  // r(t) + v(t)dt + (1/2)gravity(t)dtdt
    /* the following block is for nonconstant gravity (spring forces) */
    //v5 = vec3_scalar_add(vv, 0.5*gravity*dt);
    // gravity(t+dt) = gravity [constant]
    //v = vec3_scalar_add(v5, 0.5*gravity*dt);
    /* end block */
    *v = vec3_add(*v, vec3_scalar_mult(a, 0.5f*dt));
}

static inline void velocity_integrate(struct Vec3* p, struct Vec3* v, float dt)   __attribute__((always_inline));

static inline void velocity_integrate(struct Vec3* p, struct Vec3* v, float dt)
{
    velocity_integrate(p, v, default_a, dt);
}

bool VerletComponent::bounce()
{
    bool bounced = false;
    
    struct Vec3 old_position = this->position;
    struct Vec3 old_velocity = this->velocity;
    velocity_integrate(&this->position, &this->velocity, dt);

    float interval = 0.0f;

    struct Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        this->position.x, this->position.y, this->position.z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        bounced = true;
        velocity_integrate(&old_position, &old_velocity, dt*interval);
        old_velocity = vec3_reflect(old_velocity, norm);
        this->velocity = vec3_scalar_mult(old_velocity, dampening);
        this->position = translate_position(old_position);
    }
    else
        this->position = translate_position(this->position);

    return bounced;
}

int* VerletComponent::bounce(int* collision, CubeID* tile)
{   // same as simple bounce, but gets extra metadata on the bounce
    struct Vec3 old_position = this->position;
    struct Vec3 old_velocity = this->velocity;
    velocity_integrate(&this->position, &this->velocity, dt);

    float interval = 0.0f;

    struct Vec3 norm;
    int *s = ray_cast5_capped(
        old_position.x, old_position.y, old_position.z,
        this->position.x, this->position.y, this->position.z,
        &interval, collision, tile, &norm
    );

    if (interval < 1.0f)
    {   // collision
        velocity_integrate(&old_position, &old_velocity, dt*interval);
        old_velocity = vec3_reflect(old_velocity, norm);
        this->velocity = vec3_scalar_mult(old_velocity, dampening);
        this->position = translate_position(old_position);
    }
    else
        this->position = translate_position(this->position);

    return s;
}

bool VerletComponent::bounce_box(float gravity)
{
    bool bounced = false;
    
    struct Vec3 old_position = this->position;
    struct Vec3 old_velocity = this->velocity;
    
    struct Vec3 a = vec3_init(0.0f, 0.0f, gravity);
    if (t_map::isSolid(translate_point(old_position.x + this->box_radius), old_position.y,                           old_position.z)) a.x += gravity;
    if (t_map::isSolid(translate_point(old_position.x - this->box_radius), old_position.y,                           old_position.z)) a.x -= gravity;
    if (t_map::isSolid(old_position.x,                           translate_point(old_position.y + this->box_radius), old_position.z)) a.y += gravity;
    if (t_map::isSolid(old_position.x,                           translate_point(old_position.y - this->box_radius), old_position.z)) a.y -= gravity;

    velocity_integrate(&this->position, &this->velocity, a, dt);

    float interval = 0.0f;

    struct Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        this->position.x, this->position.y, this->position.z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        bounced = true;
        velocity_integrate(&old_position, &old_velocity, dt*interval);
        old_velocity = vec3_reflect(old_velocity, norm);
        this->velocity = vec3_scalar_mult(old_velocity, dampening);
        this->position = translate_position(old_position);
    }
    else
        this->position = translate_position(this->position);
        
    return bounced;
}

bool VerletComponent::bounce_box()
{
    return this->bounce_box(gravity);
}

bool VerletComponent::bounce_box_no_gravity()
{
    return this->bounce_box(0.0f);
}

bool VerletComponent::radial(float xr, float yr)
{
    bool bounced = false;
    
    struct Vec3 old_position = this->position;
    struct Vec3 old_velocity = this->velocity;

    static const float ac = -40.8f; // wtf is this number 
    struct Vec3 a = vec3_init(ac*(position.x-xr), ac*(position.y-yr), 0.0f);

    velocity_integrate(&this->position, &this->velocity, a, dt);

    float interval = 0.0f;

    struct Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        this->position.x, this->position.y, this->position.z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        bounced = true;

        const float th = 2.0f * 3.14159f * randf();
        const float V = 5.0f * randf();

        old_velocity.x += V * sinf(th);
        old_velocity.y += V * cosf(th);
        velocity_integrate(&old_position, &old_velocity, dt*interval);

        old_velocity = vec3_reflect(old_velocity, norm);
        this->velocity = vec3_scalar_mult(old_velocity, dampening);
        this->position = translate_position(old_position);
    }
    else
        this->position = translate_position(this->position);
        
    return bounced;
}

bool VerletComponent::collide_no_gravity()
{
    bool collided = false;
    
    struct Vec3 old_position = this->position;
    struct Vec3 old_velocity = this->velocity;

    velocity_integrate(&this->position, &this->velocity, no_gravity, dt);

    float interval = 0.0f;
    struct Vec3 norm;
    _ray_cast4(
        old_position.x, old_position.y, old_position.z,
        this->position.x, this->position.y, this->position.z,
        &interval, &norm
    );

    if (interval < 1.0f)
    {   // collision
        collided = true;
        this->position = vec3_add(old_position, vec3_scalar_mult(old_velocity, interval*dt));
        this->position = translate_position(this->position);
        this->velocity = vec3_init(0.0f, 0.0f, 0.0f);
    }
    else
        this->position = translate_position(this->position);

    return collided;
}

void VerletComponent::move_no_gravity()
{
    this->position = vec3_add(this->position, vec3_scalar_mult(this->velocity, dt));
    this->position = translate_position(this->position);
}


}   // Verlet
