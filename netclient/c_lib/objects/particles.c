#include "particles.h"

void create_particle(struct Particle* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max) {
    p->id = id;
    p->x=x;
    p->y=y;
    p->z=z;
    p->vx=vx;
    p->vy=vy;
    p->vz=vz;
    p->ttl = ttl;
    p->ttl_max = ttl_max;
    p->type = type;
}

void create_particle2(struct Particle2* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max) {
    p->state.p.x=x;
    p->state.p.y=y;
    p->state.p.z=z;
    p->state.v.x=vx;
    p->state.v.y=vy;
    p->state.v.z=vz;
    p->ttl = ttl;
    p->ttl_max = ttl_max;
    p->id = id;
    p->type = type;
}

void create_event_particle(struct EventParticle* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max, int event_ttl) {
    p->event_ttl = event_ttl;
    //struct Particle* particle = (struct Particle*)(malloc(sizeof(struct Particle)));
    create_particle(&(p->particle), id, type, x,y,z, vx,vy,vz, ttl, ttl_max);
    //p->particle = particle;
}

void create_event_particle2(struct EventParticle2* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max, int event_ttl) {
    p->event_ttl = event_ttl;
    //struct Particle* particle = (struct Particle*)(malloc(sizeof(struct Particle)));
    create_particle2(&(p->particle), id, type, x,y,z, vx,vy,vz, ttl, ttl_max);
    //p->particle = particle;
}
