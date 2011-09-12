#ifndef physics_common_h
#define physics_common_h

#include <ray_trace/ray_trace.h>


#define FPS 30

struct State {
    struct Vector p;
    struct Vector v;
};

#include <objects/particles.h>

int* bounce_simple(struct Particle* p);
int* bounce_collide_tile(struct Particle* p, int* collision, int* tile);
int* move_simple(struct Particle* p);
int* move_collide_tile(struct Particle* p, int* collision, int* tile);

int* bounce_simple_rk4(struct Particle2* p, float damp);
int* bounce_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp);
int* move_simple_rk4(struct Particle2* p, float damp);
int* move_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp);


/* integrator */
//void accelerate(struct State* state, float t, float dt);
//void rk4_step(struct State* state, struct State* i, struct State* d, float t, float dt);
void rk4(struct State* state, int t, int dt);


#endif
