#pragma once

#include <c_lib/template/object_list.hpp>

namespace HudText
{
    
void start_text_draw();
void end_text_draw();

void draw_string(char* text, float x, float y, float depth, float scale);

class Text
{
    private:
        float width,height;
        int text_len;
        int format_len;
        int formatted_extra_len;
        void set_string(char* text, char** this_text, int* this_len);
        void resize_string(int n, char** str, int *str_len);

    public:
        int id;
        bool inited;
        float depth;
        float scale;
        bool formatted;
        
        unsigned char r,g,b,a;

        char* text;
        char* format;
        
        float x,y;  // active x,y
        float xoff,yoff; // reference x,y

        void set_text(char* text);
        void set_format(char* format);
        void set_format_extra_length(int size);
        void update_formatted_string(int n_args, ...);
        
        void set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_position(float x, float y);
        void set_scale(float scale);
        void set_depth(float depth);

        int get_width();
        int get_height();
        
        void draw();

        Text(int id);
        ~Text();
};

void init();

const int TEXT_MAX = 512;
class Text_list: public Object_list<Text,TEXT_MAX>
{
    private:
        const char* name() { return "Text"; }
};

extern Text_list text_list;

}
