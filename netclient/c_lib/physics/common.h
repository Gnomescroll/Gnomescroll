#ifndef physics_common_h
#define physics_common_h

#include <ray_trace/ray_trace.h>

#include <objects/particles.h>

#define FPS 30

int* bounce_simple(struct Particle* p);
int* bounce_collide_tile(struct Particle* p, int* collision, int* tile);

int* move_simple(struct Particle* p);
int* move_collide_tile(struct Particle* p, int* collision, int* tile);

#endif
