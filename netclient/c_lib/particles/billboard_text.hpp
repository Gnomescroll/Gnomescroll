#pragma once

#include <physics/common.hpp>

#define BILLBOARD_TEXT_MAX 1024
#define BILLBOARD_TEXT_TTL 300
#define BILLBOARD_TEXT_TYPE 10
#define BILLBOARD_TEXT_DAMP 0.2f
#define BILLBOARD_TEXT_TEXTURE_SCALE 0.3f


const static int max_letters = 12;

class BillboardText {

    public:
        unsigned char r,g,b,a;
        Particle2 particle;
        char text[max_letters+1];

        bool gravity;
        bool should_draw;
        
        void set_text(char* t) {
            int i;
            for (i=0; t[i] != '\0' && i < max_letters; i++)
            {
                text[i] = t[i];
            }
            text[i] = '\0';
        }

        void set_color(unsigned char r, unsigned char g, unsigned char b) {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        void set_color(unsigned char r, unsigned char g, unsigned char b,  unsigned char a) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }

        void set_gravity(bool grav);
        void set_draw(bool draw);
        void set_ttl(int ttl);
        void set_state(float x, float y, float z, float vx, float vy, float vz);
        
        void draw();
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
        void tick();
};

