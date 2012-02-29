#pragma once

#include <physics/common.hpp>

#define NEUTRON_MAX 1024
#define NEUTRON_TTL 600
#define NEUTRON_DAMP 1.0f
#define NEUTRON_TYPE 0
#define NEUTRON_TEXTURE_ID 21 /* Not used. Texture id is dependent on energy */
#define NEUTRON_TEXTURE_SCALE 0.3f

class Neutron: public EventParticle
{
    public:
        int energy;

        void draw();
        void tick();
        void set_energy(int energy);
        Neutron(int id);
        Neutron(int id, float x, float y, float z, float ix, float iy, float iz);
};

#include <c_lib/template/object_list.hpp>

class Neutron_list: public Object_list<Neutron, NEUTRON_MAX>
{
    private:
        const char* name() { return "Neutron"; }
    public:
        void draw();
        void tick();
};
