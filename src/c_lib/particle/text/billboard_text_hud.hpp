#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <particle/particles.hpp>
#include <particle/text/text_particle.hpp>

namespace Particle
{

const int BILLBOARD_TEXT_HUD_MAX = 256;
const int BILLBOARD_TEXT_HUD_TTL = 300;
const int BILLBOARD_TEXT_HUD_TYPE = 10;
const float BILLBOARD_TEXT_HUD_DAMP = 0.2f;
const float BILLBOARD_TEXT_HUD_TEXTURE_SCALE = 0.2f;

const static int BILLBOARD_TEXT_HUD_MAX_LETTERS = 12;

class BillboardTextHud: public TextParticle
{
    private:
        inline void init_properties();
        
    public:
        bool should_draw;
        bool permanent;
        
    void set_draw(bool draw);
    void set_position(float x, float y)
    {
        HudText::Text::set_position(x,y);
    }
    void draw();
    void tick();

    inline void reset();

    BillboardTextHud();
};

}

#include <common/template/object_list.hpp>

namespace Particle
{

class BillboardTextHud_list: public Simple_object_list<BillboardTextHud, BILLBOARD_TEXT_HUD_MAX>
{
    private:
        const char* name() { return "BillboardTextHud"; }
    public:
        void draw();
        void tick();
};

}
