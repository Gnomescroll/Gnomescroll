#ifndef object_particles_h
#define object_particles_h

struct Particle {
    unsigned int id;
    float x,y,z;
    float vx,vy,vz;
    unsigned int ttl;
    unsigned int ttl_max;
    int type;
};

struct EventParticle {
    struct Particle *particle;
    unsigned int event_ttl;
};


void create_particle(struct Particle* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max);
void create_event_particle(struct EventParticle* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max, int event_ttl);

void free_event_particle(struct EventParticle* p);

#endif
