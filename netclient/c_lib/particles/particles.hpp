#pragma once

#include <c_lib/physics/common.hpp>
#include <c_lib/physics/verlet.hpp>

using Verlet::VerletParticle;

class CParticle
{
    public:
        int id;
        int ttl;
        int ttl_max;
        int type;

        VerletParticle* vp;

        void set_ttl(int ttl)
        {   // set to a negative number for no timeout
            this->ttl = ttl;
        }


    CParticle(int id, float x, float y, float z, float vx, float vy, float vz)
    :
    id(id),
    ttl(0)
    {
        vp = new VerletParticle(x,y,z,vx,vy,vz);
    }

    ~CParticle()
    {
        delete vp;
    }
};

class CEventParticle: public CParticle
{
    public:
        int event_ttl;

    CEventParticle(int id, float x, float y, float z, float vx, float vy, float vz)
    :
    CParticle(id, x,y,z,vx,vy,vz)
    {}
};

struct Particle {
    int id;
    float x,y,z;
    float vx,vy,vz;
    int ttl;
    int ttl_max;
    int type;
};

struct Particle2 {
    unsigned int id;
    struct State state;
    int ttl;
    int ttl_max;
    int type;
};

struct EventParticle {
    struct Particle particle;
    int event_ttl;
};

struct EventParticle2 {
    struct Particle2 particle;
    int event_ttl;
};

void create_particle(struct Particle* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max);
void create_particle2(struct Particle2* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max);
void create_event_particle(struct EventParticle* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max, int event_ttl);
void create_event_particle2(struct EventParticle2* p, unsigned int id, int type, float x, float y, float z, float vx, float vy, float vz, int ttl, int ttl_max, int event_ttl);

void set_particle2_state(struct Particle2* p, float x, float y, float z, float vx, float vy, float vz);
