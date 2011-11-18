#pragma once

#include <compat.h>
#ifdef DC_CLIENT
    #include <compat_gl.h>
#endif

#include <physics/common.h>
#include <ray_trace/ray_trace.h>
#include <t_map/t_map.h>
#include <t_map/t_properties.h>

#include <hud/text.h>

#define BILLBOARD_TEXT_MAX 1024
#define BILLBOARD_TEXT_TTL 90
#define BILLBOARD_TEXT_TYPE 10
#define BILLBOARD_TEXT_DAMP 0.1f
#define BILLBOARD_TEXT_TEXTURE_SCALE 0.3f


const static int max_letters = 12;

class BillboardText {

    public:
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
        Particle2 particle;
        //char text[10];
        char text[max_letters];
        int text_len;

        void set_text(char* t, int length) {
            //c=0;
            //return;
            //c = (c > 10) ? 10 : c;
            //int i;
            //for (i=0; i<c; i++) {
                //text[i] = t[i];
            //}
            //text = t;
            //sprintf(this->text, "%s", t);
            if(length > max_letters) length = max_letters;
            int i;
            for (i=0; i<length; i++) {
                text[i] = t[i];
            }
            text[length] = NULL;
            printf("Set text to %s, length= %i\n",text, length);
            text_len = length;
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
    
