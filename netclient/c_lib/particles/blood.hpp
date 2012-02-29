#pragma once

#include <physics/common.hpp>

#define BLOOD_MAX 4096
#define BLOOD_TTL 30
#define BLOOD_DAMP 0.6f
#define BLOOD_TYPE 2
#define BLOOD_TEXTURE_ID 54
#define BLOOD_TEXTURE_SCALE 0.1f


class Blood: public Particle
{
    public:
        void draw();
        void tick();
        Blood(int id);
        Blood(int id, float x, float y, float z, float mx, float my, float mz);
};

#include <c_lib/template/object_list.hpp>

class Blood_list: public Object_list<Blood, BLOOD_MAX>
{
    private:
        const char* name() { return "Blood"; }
    public:
        void draw();
        void tick();
};
    
