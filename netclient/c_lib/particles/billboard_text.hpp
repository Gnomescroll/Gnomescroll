#pragma once

#include <physics/common.hpp>

#define BILLBOARD_TEXT_MAX 1024
#define BILLBOARD_TEXT_TTL 300
#define BILLBOARD_TEXT_TYPE 10
#define BILLBOARD_TEXT_DAMP 0.2f
#define BILLBOARD_TEXT_TEXTURE_SCALE 0.2f

const static int max_letters = 12;

namespace Billboard {
    enum Projection {
        AXIS_ALIGNED,
        AXIS,
        HUD
    };
    Projection DEFAULT = AXIS_ALIGNED;
}

class BillboardText {

    public:
        unsigned char r,g,b,a;
        Particle2 particle;
        char text[max_letters+1];

        bool gravity;
        bool should_draw;
        float size;
        
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a);
        void set_ttl(int ttl);
        void set_gravity(bool grav);
        void set_state(float x, float y, float z, float vx, float vy, float vz);
        void set_text(char* t);
        void set_draw(bool draw);
        void set_size(float size);

        Billboard::Projection projection_type;
        void draw();
        void draw_axis_aligned();
        //void draw_axis();
        void draw_hud();

        void tick();
        BillboardText(int id);
        BillboardText(int id, float x, float y, float z, float vx, float vy, float vz);
};

#include <c_lib/template/object_list.hpp>

class BillboardText_list: public Object_list<BillboardText, BILLBOARD_TEXT_MAX>
{
    private:
        const char* name() { return "BillboardText"; }
    public:
        void draw();
        void draw_hud();
        void tick();
};

