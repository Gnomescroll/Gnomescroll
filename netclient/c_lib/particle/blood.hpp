#pragma once

namespace Particle
{

const int BLOOD_MAX = 4096;
const int BLOOD_TTL = 30;
const float BLOOD_DAMP = 0.6f;
const int BLOOD_TYPE = 2;
const int BLOOD_TEXTURE_ID = 54;
const float BLOOD_TEXTURE_SCALE = 0.1f;


class Blood: public ParticleMotion, public BillboardSprite
{
    public:
        void init();
        void tick();
        explicit Blood(int id);
        Blood(int id, float x, float y, float z, float mx, float my, float mz);
};

}

#include <common/template/object_list.hpp>

namespace Particle
{

class Blood_list: public Object_list<Blood, BLOOD_MAX>
{
    private:
        const char* name() { return "Blood"; }
    public:
        void draw();
        void tick();

    Blood_list() { print_list((char*)this->name(), this); }
};
    
}
