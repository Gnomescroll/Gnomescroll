#pragma once

#if DC_SERVER
# error Don't include this file in the server
#endif

#include <particle/particles.hpp>

namespace Particle
{

const int BILLBOARD_TEXT_MAX = 1024;
const int BILLBOARD_TEXT_TTL = 250;
const int BILLBOARD_TEXT_TYPE = 10;
const float BILLBOARD_TEXT_DAMP = 0.2f;
const float BILLBOARD_TEXT_TEXTURE_SCALE = 0.2f;

const int BILLBOARD_TEXT_BOUNCE_EXPLODE_LIMIT = 2;

class BillboardText: public TextParticle
{
    private:
        int bounce_count;
    public:
        bool gravity;
        bool should_draw;
        bool permanent;

    void set_position(Vec3 p)
    {
        ParticleMotionCore::set_position(p);
    }

    void set_gravity(bool grav);
    void set_draw(bool draw);

    void draw();
    void tick();

    inline void reset();

    BillboardText();
};

}   // Particles

#include <common/template/object_list.hpp>

namespace Particle
{

class BillboardTextList: public SimpleObjectList<BillboardText, BILLBOARD_TEXT_MAX>
{
    private:
    const char* name()
    {
        return "BillboardText";
    }
    public:
    void draw();
    void tick();
};

}   // Particles
