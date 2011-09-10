#ifndef object_particles_h
#define object_particles_h

struct Particle {
    float x,y,z;
    float vx,vy,vz;
    unsigned int ttl;
    unsigned int ttl_max;
    int type;
};

#endif
