#include "common.hpp"

static inline void _adjust_vel(struct Particle* p, int* rot, int adj) {
    if(rot[0] != 0 ) {
        p->vx *= adj;
    }
    if(rot[1] != 0) {
        p->vy *= adj;
    }
    if(rot[2] != 0) {
        p->vz *= adj;
    }
}

int* bounce_simple(struct Particle* p) {

    float _x, _y, _z;
    _x = p->x + p->vx/FPS;
    _y = p->y + p->vy/FPS;
    _z = p->z + p->vz/FPS;

    float interval;
    int* s;
    s = _ray_cast4(p->x, p->y, p->z, _x,_y,_z, &interval);

    _adjust_vel(p, s, -1);

    p->x = p->x + interval*p->vx/FPS;
    p->y = p->y + interval*p->vy/FPS;
    p->z = p->z + interval*p->vz/FPS;

    

    return s;
}

int* move_simple(struct Particle* p) {
     float _x, _y, _z;
    _x = p->x + p->vx/FPS;
    _y = p->y + p->vy/FPS;
    _z = p->z + p->vz/FPS;

    float interval;
    int* s;
    s = _ray_cast4(p->x, p->y, p->z, _x,_y,_z, &interval);

    _adjust_vel(p, s, 0);

    p->x = p->x + interval*p->vx/FPS;
    p->y = p->y + interval*p->vy/FPS;
    p->z = p->z + interval*p->vz/FPS;

    

    return s;
}

int* bounce_collide_tile(struct Particle* p, int* collision, int* tile) {

    float _x, _y, _z;
    _x = p->x + p->vx/FPS;
    _y = p->y + p->vy/FPS;
    _z = p->z + p->vz/FPS;

    float interval;
    int* s;
    s = _ray_cast5_capped(p->x, p->y, p->z, _x,_y,_z, &interval, collision, tile);

    _adjust_vel(p, s, -1);

    p->x = p->x + interval*p->vx/FPS;
    p->y = p->y + interval*p->vy/FPS;
    p->z = p->z + interval*p->vz/FPS;

    

    return s;
}

int* move_collide_tile(struct Particle* p, int* collision, int* tile) {

    float _x, _y, _z;
    _x = p->x + p->vx/FPS;
    _y = p->y + p->vy/FPS;
    _z = p->z + p->vz/FPS;

    float interval;
    int* s;
    s = _ray_cast5_capped(p->x, p->y, p->z, _x,_y,_z, &interval, collision, tile);

    _adjust_vel(p, s, 0);

    p->x = p->x + interval*p->vx/FPS;
    p->y = p->y + interval*p->vy/FPS;
    p->z = p->z + interval*p->vz/FPS;

    

    return s;
}


/* motion methods using rk4 */
static inline void _adjust_vel2(struct Particle2* p, int* rot, int adj, float damp) {
    int coll = 0;

    if(rot[0] != 0 ) {
        p->state.v.x *= adj;
        coll=1;
    }
    if(rot[1] != 0) {
        p->state.v.y *= adj;
        coll=1;
    }
    if(rot[2] != 0) {
        p->state.v.z *= adj;
        coll=1;
    }

    if (coll) {
        p->state.v.x *= damp;
        p->state.v.y *= damp;
        p->state.v.z *= damp;
    }
}

struct State _motion_inter = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}; /* intermediate struct used for ray cast */
struct State* motion_inter = &_motion_inter;
int* move_simple_rk4(struct Particle2* p, float damp) {

    float dt = 1.0f;

    motion_inter->p.x = p->state.p.x;
    motion_inter->p.y = p->state.p.y;
    motion_inter->p.z = p->state.p.z;
    motion_inter->v.x = p->state.v.x;
    motion_inter->v.y = p->state.v.y;
    motion_inter->v.z = p->state.v.z;

    rk4(motion_inter, dt);

    float interval;
    int* s;
    s = _ray_cast4(p->state.p.x, p->state.p.y, p->state.p.z, motion_inter->p.x, motion_inter->p.y, motion_inter->p.z, &interval);

    _adjust_vel2(p, s, 0, damp);

    if (interval >= dt) {
        p->state = *motion_inter;
    } else {
        rk4(&(p->state), interval);
    }
    

    return s;
}

int* bounce_simple_rk4(struct Particle2* p, float damp) {


    float dt = 1.0f;

    motion_inter->p.x = p->state.p.x;
    motion_inter->p.y = p->state.p.y;
    motion_inter->p.z = p->state.p.z;
    motion_inter->v.x = p->state.v.x;
    motion_inter->v.y = p->state.v.y;
    motion_inter->v.z = p->state.v.z;

    rk4(motion_inter, dt);

    float interval;
    int* s;
    s = _ray_cast4(
        p->state.p.x, p->state.p.y, p->state.p.z,
        motion_inter->p.x, motion_inter->p.y, motion_inter->p.z,
        &interval
    );

    if (interval >= dt) {
        p->state = *motion_inter;
    } else {
        // collision
        rk4(&(p->state), interval);
        struct Vector norm = {(float)s[0], (float)s[1], (float)s[2]};
        struct Vector v = reflect(&(p->state.v), &norm);
        p->state.v = *(mult_vec_scalar(&v, damp));
    }

    

    return s;
}

int* bounce_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp) {


    float dt = 1.0f;

    /* copy state */
    motion_inter->p.x = p->state.p.x;
    motion_inter->p.y = p->state.p.y;
    motion_inter->p.z = p->state.p.z;
    motion_inter->v.x = p->state.v.x;
    motion_inter->v.y = p->state.v.y;
    motion_inter->v.z = p->state.v.z;

    /* integrate */
    rk4(motion_inter, dt);

    float interval;
    int* s;

    /* Ray cast to calculated position */
    s = _ray_cast5_capped(
        p->state.p.x, p->state.p.y, p->state.p.z,
        motion_inter->p.x, motion_inter->p.y, motion_inter->p.z,
        &interval, collision, tile);
        
    _adjust_vel2(p, s, -1, damp);

    if (interval >= dt) { // no collision
        p->state = *motion_inter;
    } else {            // collided interval%. integrate this amount
        rk4(&(p->state), interval);
    }
    
    return s;
}


int* move_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp) {

    float dt = 1.0f;

    motion_inter->p.x = p->state.p.x;
    motion_inter->p.y = p->state.p.y;
    motion_inter->p.z = p->state.p.z;
    motion_inter->v.x = p->state.v.x;
    motion_inter->v.y = p->state.v.y;
    motion_inter->v.z = p->state.v.z;

    rk4(motion_inter, dt);

    float interval;
    int* s;
    s = _ray_cast5_capped(p->state.p.x, p->state.p.y, p->state.p.z, motion_inter->p.x, motion_inter->p.y, motion_inter->p.z, &interval, collision, tile);

    _adjust_vel2(p, s, 0, damp);

    if (interval >= dt) {
        p->state = *motion_inter;
    } else {
        rk4(&(p->state), interval);
    }

    return s;
}


/* integrator */
static struct State derivatives[4];

static inline void rk4_accelerate(struct State* inter, float dt) {

    //const float air_resist = 1.0f;
    //const float spring = 0.1f;
    const float gravity = 28.0f * 30.0f;
    //const float gravity = 9.8f * 2.0f;
    //printf("final g %0.2f\n", gravity*dt);
    inter->v.z -= gravity * dt;
    //inter->v.z -= gravity * dt; /* This is too fucking slow!! The ball does not drop */

    ////inter->v.x -= spring * inter->p.x;
    ////inter->v.y -= spring * inter->p.y;
    ////inter->v.z -= spring * (inter->p.z * inter->p.z);

    //inter->v.x *= air_resist;
    //inter->v.y *= air_resist;
    //inter->v.z *= air_resist;
}

static struct State _step_inter = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}; /* intermediate derivative */
static struct State* step_inter = &_step_inter;
static inline void rk4_step(struct State* initial, struct State* final, struct State* old, float dt)
{
     step_inter->p.x = initial->p.x + old->p.x*dt;
     step_inter->p.y = initial->p.y + old->p.y*dt;
     step_inter->p.z = initial->p.z + old->p.z*dt;
     step_inter->v.x = initial->v.x + old->v.x*dt;
     step_inter->v.y = initial->v.y + old->v.y*dt;
     step_inter->v.z = initial->v.z + old->v.z*dt;

    final->p = step_inter->v;
     rk4_accelerate(step_inter, dt);
     final->v = step_inter->v;
}

static inline void rk4_step0(struct State* initial, struct State* final) {
    final->p = initial->v;
    final->v.x=0.0f;
    final->v.y=0.0f;
    final->v.z=0.0f;
    rk4_accelerate(final, 0.0f);
}

void rk4(struct State* state, float dt)
{
    dt /= FPS;
    
    rk4_step0(state, &derivatives[0]);
    rk4_step(state, &derivatives[1], &derivatives[0], dt*0.5f);
    rk4_step(state, &derivatives[2], &derivatives[1], dt*0.5f);
    rk4_step(state, &derivatives[3], &derivatives[2], dt);

    const float dxdt = (1.0f/6.0f) * (derivatives[0].p.x + 2.0f*(derivatives[1].p.x + derivatives[2].p.x) + derivatives[3].p.x);
    const float dydt = (1.0f/6.0f) * (derivatives[0].p.y + 2.0f*(derivatives[1].p.y + derivatives[2].p.y) + derivatives[3].p.y);
    const float dzdt = (1.0f/6.0f) * (derivatives[0].p.z + 2.0f*(derivatives[1].p.z + derivatives[2].p.z) + derivatives[3].p.z);

    const float dvxdt = (1.0f/6.0f) * (derivatives[0].v.x + 2.0f*(derivatives[1].v.x + derivatives[2].v.x) + derivatives[3].v.x);
    const float dvydt = (1.0f/6.0f) * (derivatives[0].v.y + 2.0f*(derivatives[1].v.y + derivatives[2].v.y) + derivatives[3].v.y);
    const float dvzdt = (1.0f/6.0f) * (derivatives[0].v.z + 2.0f*(derivatives[1].v.z + derivatives[2].v.z) + derivatives[3].v.z);

    state->p.x = state->p.x + dxdt * dt;
    state->p.y = state->p.y + dydt * dt;
    state->p.z = state->p.z + dzdt * dt;

    state->v.x = state->v.x + dvxdt * dt;
    state->v.y = state->v.y + dvydt * dt;
    state->v.z = state->v.z + dvzdt * dt;
}

bool cube_intersects(
    float x, float y, float z, float w, float h, float d,
    float x2, float y2, float z2, float w2, float h2, float d2
)
{
     // do not intersect if (Mx>Px) or (Ox>Nx) or (My>Py) or (Oy>Ny) or (Mz>Pz) or (Oz>Nz).
    if
    ((x > x2+w2) || (x2 > x+w) || (y > y2+h2) || (y2 > y+h) || (z > z2+d2) || (z2 > z+d))
    {
        return false;
    }
    return true;
}
