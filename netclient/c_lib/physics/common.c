#include "common.h"

static inline void _adjust_vel(struct Particle* p, int* rot, int adj) {
    if(rot[0] != 0 ) {
        p->vx *= adj;
        //printf("invert vx \n");
    }
    if(rot[1] != 0) {
        p->vy *= adj;
        //printf("invert vy \n");
    }
    if(rot[2] != 0) {
        p->vz *= adj;
        //printf("invert vz \n");
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
    s = _ray_cast5(p->x, p->y, p->z, _x,_y,_z, &interval, collision, tile);

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
    s = _ray_cast5(p->x, p->y, p->z, _x,_y,_z, &interval, collision, tile);

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
        //printf("invert vx \n");
    }
    if(rot[1] != 0) {
        p->state.v.y *= adj;
        coll=1;
        //printf("invert vy \n");
    }
    if(rot[2] != 0) {
        p->state.v.z *= adj;
        coll=1;
        //printf("invert vz \n");
    }
    
    if (coll) {
        p->state.v.x *= damp;
        p->state.v.y *= damp;
        p->state.v.z *= damp;
    }
        
}

struct State inter; /* intermediate struct used for ray cast */

int* move_simple_rk4(struct Particle2* p, float damp) {
    int t = p->ttl;
    int dt = 1;

    inter.p.x = p->state.p.x;
    inter.p.y = p->state.p.y;
    inter.p.z = p->state.p.z;
    inter.v.x = p->state.v.x;
    inter.v.y = p->state.v.y;
    inter.v.z = p->state.v.z;
    
    rk4(&inter, t, dt);

    float interval;
    int* s;
    s = _ray_cast4(p->state.p.x, p->state.p.y, p->state.p.z, inter.p.x, inter.p.y, inter.p.z, &interval);

    _adjust_vel2(p, s, 0, damp);

    if ((int)interval == dt) {
        p->state = inter;
    } else {
        rk4(&(p->state), t, interval);
    }
    
    return s;   
}

int* bounce_simple_rk4(struct Particle2* p, float damp) {

    int t = p->ttl;
    int dt = 1;

    inter.p.x = p->state.p.x;
    inter.p.y = p->state.p.y;
    inter.p.z = p->state.p.z;
    inter.v.x = p->state.v.x;
    inter.v.y = p->state.v.y;
    inter.v.z = p->state.v.z;
    
    rk4(&inter, t, dt);

    float interval;
    int* s;
    //int n = _GET_MS_TIME();
    s = _ray_cast4(p->state.p.x, p->state.p.y, p->state.p.z, inter.p.x, inter.p.y, inter.p.z, &interval);
    //int n2 = _GET_MS_TIME();
    //printf("raycast took %d\n", n2-n);

    _adjust_vel2(p, s, -1, damp);
    //if (p->id == 10) {
        //printf("GRENADE\n");
        //printf("%d\n", t);
        //printf("%f\n", interval);
        //printf("%f,%f,%f, %f,%f,%f\n", j->p.x, j->p.y, j->p.z, j->v.x, j->v.y, j->v.z);
    //}

    if ((int)interval == dt) {
        p->state = inter;
    } else {
        rk4(&(p->state), t, interval);
    }

    //if (p->id == 10) {
        //j = &(p->state);
        //printf("%f,%f,%f, %f,%f,%f\n", j->p.x, j->p.y, j->p.z, j->v.x, j->v.y, j->v.z);
    //}

    return s;
}

int* bounce_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp) {

    int t = p->ttl;
    int dt = 1;

    inter.p.x = p->state.p.x;
    inter.p.y = p->state.p.y;
    inter.p.z = p->state.p.z;
    inter.v.x = p->state.v.x;
    inter.v.y = p->state.v.y;
    inter.v.z = p->state.v.z;

    rk4(&inter, t, dt);


    float interval;
    int* s;
    s = _ray_cast5(p->state.p.x, p->state.p.y, p->state.p.z, inter.p.x, inter.p.y, inter.p.z, &interval, collision, tile);

    _adjust_vel2(p, s, -1, damp);

    if ((int)interval == dt) {
        p->state = inter;
    } else {
        rk4(&(p->state), t, interval);
    }

    return s;
}

int* move_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp) {
    int t = p->ttl;
    int dt = 1;

    inter.p.x = p->state.p.x;
    inter.p.y = p->state.p.y;
    inter.p.z = p->state.p.z;
    inter.v.x = p->state.v.x;
    inter.v.y = p->state.v.y;
    inter.v.z = p->state.v.z;

    rk4(&inter, t, dt);

    float interval;
    int* s;
    s = _ray_cast5(p->state.p.x, p->state.p.y, p->state.p.z, inter.p.x, inter.p.y, inter.p.z, &interval, collision, tile);

    _adjust_vel2(p, s, 0, damp);

    if ((int)interval == dt) {
        p->state = inter;
    } else {
        rk4(&(p->state), t, interval);
    }
    
    return s;
}


/* integrator */
struct State derivatives[4];
struct State _derivative = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};

static inline void accelerate(struct State* state, float t, float dt) {
    //const float k = 10;
    //const float b = 1;
    state->v.z -= 1.25 * dt;

    //state->v.x = -k * state->p.x - b*state->v.x;
    //state->v.y = -k * state->p.y - b*state->v.y;
    //state->v.z = -k * state->p.z - b*state->v.z;

}

struct State step_inter;
void rk4_step(struct State* state, struct State* i, struct State* d, float t, float dt)
{
   //printf("RK4STEP\n");
    //printf("%f,%f,%f, %f,%f,%f\n", state->p.x, state->p.y, state->p.z, state->v.x, state->v.y, state->v.z);

     step_inter.p.x = state->p.x + d->p.x*dt;
     step_inter.p.y = state->p.y + d->p.y*dt;
     step_inter.p.z = state->p.z + d->p.z*dt;
     step_inter.v.x = state->v.x + d->v.x*dt;
     step_inter.v.y = state->v.y + d->v.y*dt;
     step_inter.v.z = state->v.z + d->v.z*dt;
     
    //printf("STEP STATE (should be constant x4)\n");   // it is constant
    //printf("%f,%f,%f, %f,%f,%f\n", state->p.x, state->p.y, state->p.z, state->v.x, state->v.y, state->v.z);
    i->p = step_inter.v;
     accelerate(&step_inter, t+dt, dt);
     i->v = step_inter.v;
    //printf("ACCEL\n");
    //printf("%f,%f,%f, %f,%f,%f\n", state->p.x, state->p.y, state->p.z, state->v.x, state->v.y, state->v.z);

}

void rk4(struct State* state, int _t, int _dt)
{
    float t = _t / 30.0f;
    float dt = _dt / 30.0f;
    //printf("RK4\n");
    //printf("%f\n", t);
    //printf("%f\n", dt);
    //printf("%f,%f,%f, %f,%f,%f\n", state->p.x, state->p.y, state->p.z, state->v.x, state->v.y, state->v.z);

     rk4_step(state, &derivatives[0], &_derivative, t, 0.0f);
     rk4_step(state, &derivatives[1], &derivatives[0], t+dt*0.5f, dt*0.5f);
     rk4_step(state, &derivatives[2], &derivatives[1], t+dt*0.5f, dt*0.5f);
     rk4_step(state, &derivatives[3], &derivatives[2], t+dt, dt);

    //printf("%f,%f,%f, %f,%f,%f\n", derivatives[0].p.x, derivatives[0].p.y, derivatives[0].p.z, derivatives[0].v.x, derivatives[0].v.y, derivatives[0].v.z);
    //printf("%f,%f,%f, %f,%f,%f\n", derivatives[1].p.x, derivatives[1].p.y, derivatives[1].p.z, derivatives[1].v.x, derivatives[1].v.y, derivatives[1].v.z);
    //printf("%f,%f,%f, %f,%f,%f\n", derivatives[2].p.x, derivatives[2].p.y, derivatives[2].p.z, derivatives[2].v.x, derivatives[2].v.y, derivatives[2].v.z);
    //printf("%f,%f,%f, %f,%f,%f\n", derivatives[3].p.x, derivatives[3].p.y, derivatives[3].p.z, derivatives[3].v.x, derivatives[3].v.y, derivatives[3].v.z);


     const float dxdt = 1.0f/6.0f * (derivatives[0].p.x + 2.0f*(derivatives[1].p.x + derivatives[2].p.x) + derivatives[3].p.x);
     const float dydt = 1.0f/6.0f * (derivatives[0].p.y + 2.0f*(derivatives[1].p.y + derivatives[2].p.y) + derivatives[3].p.y);
     const float dzdt = 1.0f/6.0f * (derivatives[0].p.z + 2.0f*(derivatives[1].p.z + derivatives[2].p.z) + derivatives[3].p.z);
     
     const float dvxdt = 1.0f/6.0f * (derivatives[0].v.x + 2.0f*(derivatives[1].v.x + derivatives[2].v.x) + derivatives[3].v.x);
     const float dvydt = 1.0f/6.0f * (derivatives[0].v.y + 2.0f*(derivatives[1].v.y + derivatives[2].v.y) + derivatives[3].v.y);
     const float dvzdt = 1.0f/6.0f * (derivatives[0].v.z + 2.0f*(derivatives[1].v.z + derivatives[2].v.z) + derivatives[3].v.z);
    //printf("%f,%f,%f, %f,%f,%f\n", dxdt, dydt, dzdt, dvxdt, dvydt, dvzdt);

     state->p.x = state->p.x + dxdt * dt;
     state->p.y = state->p.y + dydt * dt;
     state->p.z = state->p.z + dzdt * dt;
     
     state->v.x = state->v.x + dvxdt * dt;
     state->v.y = state->v.y + dvydt * dt;
     state->v.z = state->v.z + dvzdt * dt;

    //printf("%f,%f,%f, %f,%f,%f\n", state->p.x, state->p.y, state->p.z, state->v.x, state->v.y, state->v.z);

}
