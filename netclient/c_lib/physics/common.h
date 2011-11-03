#pragma once

#include <c_lib/physics/vector.h>

struct State {
    struct Vector p;
    struct Vector v;
};

#include <c_lib/ray_trace/ray_trace.h>
#include <c_lib/objects/particles.hpp>

#define FPS 30.0f

int* bounce_simple(struct Particle* p);
int* bounce_collide_tile(struct Particle* p, int* collision, int* tile);
int* move_simple(struct Particle* p);
int* move_collide_tile(struct Particle* p, int* collision, int* tile);

int* bounce_simple_rk4(struct Particle2* p, float damp);
int* bounce_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp);
int* move_simple_rk4(struct Particle2* p, float damp);
int* move_collide_tile_rk4(struct Particle2* p, int* collision, int* tile, float damp);


/* integrator */
void rk4(struct State* state, float dt);
