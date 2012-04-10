#pragma once

#include <particles/particles.hpp>
#include <hud/text.hpp>

namespace Particles
{

#define BILLBOARD_TEXT_HUD_MAX 1024
#define BILLBOARD_TEXT_HUD_TTL 300
#define BILLBOARD_TEXT_HUD_TYPE 10
#define BILLBOARD_TEXT_HUD_DAMP 0.2f
#define BILLBOARD_TEXT_HUD_TEXTURE_SCALE 0.2f

const static int BILLBOARD_TEXT_HUD_MAX_LETTERS = 12;

class BillboardTextHud: public Particle
{
    public:
        HudText::Text* text;
        bool should_draw;
        
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a);
        void set_text(char* t);
        void set_draw(bool draw);
        void set_size(float size);

        void draw();
        void tick();
        
        BillboardTextHud(int id);
        BillboardTextHud(int id, float x, float y, float z, float mx, float my, float mz);
};

}

#include <c_lib/template/object_list.hpp>

namespace Particles
{

class BillboardTextHud_list: public Object_list<BillboardTextHud, BILLBOARD_TEXT_HUD_MAX>
{
    private:
        const char* name() { return "BillboardTextHud"; }
    public:
        void draw();
        void tick();

        BillboardTextHud_list() { print(); }
};

}
