#pragma once

#include <particles/particles.hpp>

#define BILLBOARD_TEXT_MAX 1024
#define BILLBOARD_TEXT_TTL 300
#define BILLBOARD_TEXT_TYPE 10
#define BILLBOARD_TEXT_DAMP 0.2f
#define BILLBOARD_TEXT_TEXTURE_SCALE 0.2f

namespace Particles
{


const static int BILLBOARD_TEXT_MAX_LETTERS = 12;

class BillboardText: public ParticleMotion
{
    public:
        unsigned char r,g,b,a;
        char text[BILLBOARD_TEXT_MAX_LETTERS+1];

        bool gravity;
        bool should_draw;
        float size;
        
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a);
        void set_gravity(bool grav);
        void set_text(char* t);
        void set_draw(bool draw);
        void set_size(float size);

        void draw();
        void tick();

        explicit BillboardText(int id);
        BillboardText(int id, float x, float y, float z, float mx, float my, float mz);
};

}

#include <c_lib/template/object_list.hpp>

namespace Particles
{

class BillboardText_list: public Object_list<BillboardText, BILLBOARD_TEXT_MAX>
{
    private:
        const char* name() { return "BillboardText"; }
    public:
        void draw();
        void tick();

        BillboardText_list() { print_list((char*)this->name(), this); }
};

}
