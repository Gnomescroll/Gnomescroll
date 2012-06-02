#pragma once

#include <particle/particles.hpp>

const int BILLBOARD_TEXT_MAX = 1024;
const int BILLBOARD_TEXT_TTL = 250;
const int BILLBOARD_TEXT_TYPE = 10;
const float BILLBOARD_TEXT_DAMP = 0.2f;
const float BILLBOARD_TEXT_TEXTURE_SCALE = 0.2f;

namespace Particle
{

const int BILLBOARD_TEXT_BOUNCE_EXPLODE_LIMIT = 2;
const int BILLBOARD_TEXT_MAX_LETTERS = 12;

class BillboardText: public ParticleMotion
{
    private:
        int bounce_count;
    public:
        unsigned char r,g,b,a;
        char text[BILLBOARD_TEXT_MAX_LETTERS+1];

        bool gravity;
        bool should_draw;
        float size;
        bool permanent;
        
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a);
        void set_color(struct Color color);
        void set_gravity(bool grav);
        void set_text(char* t);
        void set_draw(bool draw);
        void set_size(float size);

        void draw();
        void tick();

        void init();

        BillboardText();
};

}

#include <common/template/object_list.hpp>

namespace Particle
{

class BillboardText_list: public Simple_object_list<BillboardText, BILLBOARD_TEXT_MAX>
{
    private:
        const char* name() { return "BillboardText"; }
    public:
        void draw();
        void tick();

        BillboardText_list() { print_list((char*)this->name(), this); }
};

}
