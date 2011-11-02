#ifndef physics_common_h
#define physics_common_h

#include <ray_trace/ray_trace.h>
#include "physics/vector.h"

#define FPS 30.0f

struct State {
    struct Vector p;
    struct Vector v;
};

#include <objects/particles.hpp>

int* bounce_simple(struct Particle* p);
int* bounce_collide_tile(struct Particle* p, int* collision, int* tile);
int* move_simple(struct Particle* p);
int* move_collide_tile(struct Particle* p, int* collision, int* tile);

int* bounce_simple_rk4(struct Particle2* p, float damp);
int* bounce_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp);
int* move_simple_rk4(struct Particle2* p, float damp);
int* move_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp);


/* integrator */
void rk4(struct State* state, int t, int dt);


#endif
