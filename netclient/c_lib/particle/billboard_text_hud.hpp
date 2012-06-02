#pragma once

#include <particle/particles.hpp>
#include <hud/text.hpp>

namespace Particle
{

const int BILLBOARD_TEXT_HUD_MAX = 1024;
const int BILLBOARD_TEXT_HUD_TTL = 300;
const int BILLBOARD_TEXT_HUD_TYPE = 10;
const float BILLBOARD_TEXT_HUD_DAMP = 0.2f;
const float BILLBOARD_TEXT_HUD_TEXTURE_SCALE = 0.2f;

const static int BILLBOARD_TEXT_HUD_MAX_LETTERS = 12;

class BillboardTextHud: public ParticleMotion
{
    public:
        HudText::Text* text;
        bool should_draw;
        int attached_to_agent;
        
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a);
        void set_text(char* t);
        void set_draw(bool draw);
        void set_size(float size);

        void draw();
        void tick();

        ~BillboardTextHud();
        BillboardTextHud(int id);
        BillboardTextHud(int id, float x, float y, float z, float mx, float my, float mz);
};

}

#include <common/template/object_list.hpp>

namespace Particle
{

class BillboardTextHud_list: public Object_list<BillboardTextHud, BILLBOARD_TEXT_HUD_MAX>
{
    private:
        const char* name() { return "BillboardTextHud"; }
    public:
        void draw();
        void tick();

        BillboardTextHud_list() { print_list((char*)this->name(), this); }
};

}
