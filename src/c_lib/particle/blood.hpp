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
        inline void reset();
        void tick();
        Blood();
};

}

#include <common/template/object_list.hpp>

namespace Particle
{

class Blood_list: public Simple_object_list<Blood, BLOOD_MAX>
{
    private:

    const char* name()
    {
        return "Blood";
    }

    public:

    void draw();
    void tick();
};
    
}   // Particle
