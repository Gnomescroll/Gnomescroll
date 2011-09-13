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
        //if (p->id == 20) {printf("COLL damp= %f\n", damp);printf("%f,%f,%f\n", p->state.v.x, p->state.v.y, p->state.v.z);}
        p->state.v.x = p->state.v.x * damp;
        p->state.v.y = p->state.v.y * damp;
        p->state.v.z = p->state.v.z * damp;
                if (p->id == 20) {printf("%f,%f,%f\n", p->state.v.x, p->state.v.y, p->state.v.z);}

    }
        
}

struct State _motion_inter = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}; /* intermediate struct used for ray cast */
struct State* motion_inter = &_motion_inter;
int* move_simple_rk4(struct Particle2* p, float damp) {
    int t = p->ttl;
    int dt = 1;

    motion_inter->p.x = p->state.p.x;
    motion_inter->p.y = p->state.p.y;
    motion_inter->p.z = p->state.p.z;
    motion_inter->v.x = p->state.v.x;
    motion_inter->v.y = p->state.v.y;
    motion_inter->v.z = p->state.v.z;
    
    rk4(motion_inter, t, dt);

    float interval;
    int* s;
    s = _ray_cast4(p->state.p.x, p->state.p.y, p->state.p.z, motion_inter->p.x, motion_inter->p.y, motion_inter->p.z, &interval);

    _adjust_vel2(p, s, 0, damp);

    if ((int)interval == dt) {
        p->state = *motion_inter;
    } else {
        rk4(&(p->state), t, interval);
    }
    
    return s;   
}

int* bounce_simple_rk4(struct Particle2* p, float damp) {

    int t = p->ttl;
    int dt = 1;

    motion_inter->p.x = p->state.p.x;
    motion_inter->p.y = p->state.p.y;
    motion_inter->p.z = p->state.p.z;
    motion_inter->v.x = p->state.v.x;
    motion_inter->v.y = p->state.v.y;
    motion_inter->v.z = p->state.v.z;
    
    rk4(motion_inter, t, dt);

    float interval;
    int* s;
    //int n = _GET_MS_TIME();
    s = _ray_cast4(p->state.p.x, p->state.p.y, p->state.p.z, motion_inter->p.x, motion_inter->p.y, motion_inter->p.z, &interval);
    //int n2 = _GET_MS_TIME();
    //printf("raycast took %d\n", n2-n);

    //_adjust_vel2(p, s, -1, damp);
    struct Vector norm = {s[0], s[1], s[2]};
    //norm->x = (float)s[0];
    //norm->y = (float)s[1];
    //norm->z = (float)s[2];
    struct Vector v = reflect(&(p->state.v), &norm);
    p->state.v = *(mult_vec_scalar(&v, damp));
    //if (s[0] || s[1] || s[2]) {
            //if (p->id == 4) {printf("%f,%f,%f\n", p->state.v.x, p->state.v.y, p->state.v.z);}}

    //if (p->id == 10) {
        //printf("GRENADE\n");
        //printf("%d\n", t);
        //printf("%f\n", interval);
        //printf("%f,%f,%f, %f,%f,%f\n", j->p.x, j->p.y, j->p.z, j->v.x, j->v.y, j->v.z);
    //}

    if ((int)interval == dt) {
        p->state = *motion_inter;
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
    //float fdt = 1.0f;

    motion_inter->p.x = p->state.p.x;
    motion_inter->p.y = p->state.p.y;
    motion_inter->p.z = p->state.p.z;
    motion_inter->v.x = p->state.v.x;
    motion_inter->v.y = p->state.v.y;
    motion_inter->v.z = p->state.v.z;

    rk4(motion_inter, t, dt);


    float interval;
    int* s;

    //int n = _GET_MS_TIME();
    s = _ray_cast5(p->state.p.x, p->state.p.y, p->state.p.z, motion_inter->p.x, motion_inter->p.y, motion_inter->p.z, &interval, collision, tile);
    //int n2 = _GET_MS_TIME();
    //if (n2-n > 10) {
        //printf("ray_cast5 :: %d\n", n2-n);
    //if (p->id == 2) {
        //printf("state:\n");
        //printf("%f,%f,%f\n", p->state.p.x, p->state.p.y, p->state.p.z);
        //printf("%f,%f,%f\n", motion_inter->p.x, motion_inter->p.y, motion_inter->p.z);
    //}

    _adjust_vel2(p, s, -1, damp);
    //if (s[0] || s[1] || s[2]) {
        //if (p->id == 20) {printf("%f,%f,%f\n", p->state.p.x, p->state.p.y, p->state.p.z);}
    //}

    //if ((int)interval == dt) {
    if (interval >= dt) {
        p->state = *motion_inter;
            //if (p->id == 2) {

        //printf("%f,%f,%f\n", p->state.p.x, p->state.p.y, p->state.p.z);
        //printf("%f,%f,%f\n", motion_inter->p.x, motion_inter->p.y, motion_inter->p.z);
    } else {
        rk4(&(p->state), t, interval);
    }

    return s;
}

int* move_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp) {
    int t = p->ttl;
    int dt = 1;

    motion_inter->p.x = p->state.p.x;
    motion_inter->p.y = p->state.p.y;
    motion_inter->p.z = p->state.p.z;
    motion_inter->v.x = p->state.v.x;
    motion_inter->v.y = p->state.v.y;
    motion_inter->v.z = p->state.v.z;

    rk4(motion_inter, t, dt);

    float interval;
    int* s;
    s = _ray_cast5(p->state.p.x, p->state.p.y, p->state.p.z, motion_inter->p.x, motion_inter->p.y, motion_inter->p.z, &interval, collision, tile);

    _adjust_vel2(p, s, 0, damp);

    if ((int)interval == dt) {
        p->state = *motion_inter;
    } else {
        rk4(&(p->state), t, interval);
    }
    
    return s;
}


/* integrator */
struct State derivatives[4];
//struct State _derivative = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};

static inline void rk4_accelerate(struct State* inter, float t, float dt) {
    //const float k = 10;
    //const float b = 1;
//printf("ACCELMAIN\n");
    //printf("%f,%f,%f, %f,%f,%f\n", inter->p.x, inter->p.y, inter->p.z, inter->v.x, inter->v.y, inter->v.z);

    const float air_resist = 0.8f;
    //const 
    inter->v.z -= 10.0f;  // gravity

    inter->v.x *= air_resist;
    inter->v.y *= air_resist;
    inter->v.z *= air_resist;
    //printf("%f,%f,%f, %f,%f,%f\n", inter->p.x, inter->p.y, inter->p.z, inter->v.x, inter->v.y, inter->v.z);
//printf("-----------\n");
}

struct State _step_inter = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}}; /* intermediate derivative */
struct State* step_inter = &_step_inter;
int step_i = 0;
static inline void rk4_step(struct State* initial, struct State* new, struct State* old, float t, float dt)
{
        //int n = _GET_MS_TIME();

   //printf("RK4STEP\n");
    //printf("%f,%f,%f, %f,%f,%f\n", initial->p.x, initial->p.y, initial->p.z, initial->v.x, initial->v.y, initial->v.z);

     step_inter->p.x = initial->p.x + old->p.x*dt;
     step_inter->p.y = initial->p.y + old->p.y*dt;
     step_inter->p.z = initial->p.z + old->p.z*dt;
     step_inter->v.x = initial->v.x + old->v.x*dt;
     step_inter->v.y = initial->v.y + old->v.y*dt;
     step_inter->v.z = initial->v.z + old->v.z*dt;
     
    //printf("STEP STATE (should be constant x4)\n");   // it is constant
    //printf("%f,%f,%f, %f,%f,%f\n", step_inter->p.x, step_inter->p.y, step_inter->p.z, step_inter->v.x, step_inter->v.y, step_inter->v.z);
    new->p = step_inter->v;
     rk4_accelerate(step_inter, t+dt, dt);
     new->v = step_inter->v;
    //printf("ACCEL\n");
        //printf("%f,%f,%f, %f,%f,%f\n", step_inter->p.x, step_inter->p.y, step_inter->p.z, step_inter->v.x, step_inter->v.y, step_inter->v.z);
    //printf("\n");

    //int n2 = _GET_MS_TIME();
    //if (n2-n > 0) {
        //printf("RK4STEP %d ms>0 :: %d\n", step_i, n2-n);
    //}
    //step_i++;
    //step_i %= 4;
}

static inline void rk4_step0(struct State* initial, struct State* new, float t) {
            //int n = _GET_MS_TIME();

    new->p = initial->v;
    new->v.x=0.0f;
    new->v.y=0.0f;
    new->v.z=0.0f;
    rk4_accelerate(new, t, 0.0f);
    //int n2 = _GET_MS_TIME();
    //if (n2-n > 0) {
        //printf("RK4STEP0 %d ms>0 :: %d\n", step_i, n2-n);
    //}
    //step_i++;
    //step_i %= 4;
//
}

void rk4(struct State* state, int _t, int _dt)
{
    //int n = _GET_MS_TIME();
    
    float t = _t / 30.0f;
    float dt = _dt / 30.0f;
    //printf("RK4\n");
    //printf("%f\n", t);
    //printf("%f\n", dt);
    //printf("%f,%f,%f, %f,%f,%f\n", state->p.x, state->p.y, state->p.z, state->v.x, state->v.y, state->v.z);

     //rk4_step(state, &derivatives[0], &_derivative, t, 0.0f);
    rk4_step0(state, &derivatives[0],                  t                 );
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

    //int n2 = _GET_MS_TIME();
    //if (n2-n > 0) {
        //printf("RK4 ms>0 :: %d\n", n2-n);
    //}

}
