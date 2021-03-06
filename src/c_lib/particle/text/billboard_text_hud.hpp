/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if DC_SERVER
# error Do not include this file in the server
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
    public:
        bool should_draw;
        bool permanent;

    inline void set_draw(bool draw);
    void set_position(float x, float y)
    {
        HudText::Text::set_position(x,y);
    }
    void draw();
    void tick();

    inline void reset();

    BillboardTextHud();

    private:
    inline void init_properties();
};

}   // Particle

#include <common/template/object_list.hpp>

namespace Particle
{

class BillboardTextHudList: public SimpleObjectList<BillboardTextHud, BILLBOARD_TEXT_HUD_MAX>
{
    public:
    void draw();
    void tick();

    private:
    const char* name()
    {
        return "BillboardTextHud";
    }
};

}   // Particle
