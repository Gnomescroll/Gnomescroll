#pragma once

#include <c_lib/template/object_list.hpp>

namespace HudText
{
    
void start_text_draw();
void end_text_draw();

void draw_string(char* text, float x, float y, float depth, float scale, float line_height);

class Text
{
    private:
        float width,height;
        float line_height;
        int str_len;
    public:
        int id;
        bool inited;
        float depth;
        float scale;
        
        unsigned char r,g,b,a;

        char* text;

        float x,y;  // active x,y
        float xoff,yoff; // reference x,y

        void set_text(char* text);
        void set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_position(float x, float y);
        void set_scale(float scale);
        void set_depth(float depth);
        
        void draw();

        Text(int id);
        ~Text();
};

void init();


//CYTHON
//Text* create_text();
//void draw_text(int id);
//void set_text(int id, char* text);
//void set_color(int id, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
//void set_y(int id, float y);
//float get_yoffset(int id);
//float get_x(int id);
//float get_y(int id);

const int TEXT_MAX = 256;
class Text_list: public Object_list<Text,TEXT_MAX>
{
    private:
        const char* name() { return "Text"; }
};

}
