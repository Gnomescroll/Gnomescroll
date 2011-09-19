#ifndef object_particles_h
#define object_particles_h
//#include <physics/common.h>

struct Particle {
    unsigned int id;
    float x,y,z;
    float vx,vy,vz;
    unsigned int ttl;
    unsigned int ttl_max;
    int type;
};

struct Particle2 {
    unsigned int id;
    //float x,y,z;
    //float vx,vy,vz;
    struct Particle state;
    unsigned int ttl;
    unsigned int ttl_max;
    int type;
};

struct EventParticle {
    struct Particle particle;
    unsigned int event_ttl;
};

struct EventParticle2 {
    struct Particle2 particle;
    unsigned int event_ttl;
};

void create_particle(struct Particle* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max);
void create_particle2(struct Particle2* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max);
void create_event_particle(struct EventParticle* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max, int event_ttl);
void create_event_particle2(struct EventParticle2* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max, int event_ttl);

#endif
