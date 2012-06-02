#pragma once

#include <common/template/object_list.hpp>

namespace HudText
{

void draw_string(char* text, float x, float y, float depth, float scale);

typedef struct
{
    bool center;
    bool left;
    bool right;
    bool top;
    bool bottom;
} TextAlignment;

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
        float refx, refy; // reference x,y

        TextAlignment alignment;
        void reset_alignment();
        void center();
        void left();
        void right();
        void top();
        void bottom();

        void set_text(char* text);
        void set_format(char* format);
        void set_format_extra_length(int size);
        void update_formatted_string(int n_args, ...);
        
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_position(float x, float y);
        void set_scale(float scale);
        void set_depth(float depth);
        void draw_centered();
        int get_width();
        int get_height();
        int charcount();
        
        void draw();
        void draw_character_rotated(float theta);   // draws as single character. glyph alignment offset not used

        explicit Text(int id);
        Text();
        ~Text();
};

void init();

const int TEXT_MAX = 1024;
class Text_list: public Object_list<Text,TEXT_MAX>
{
    private:
        const char* name() { return "Text"; }

    public:
        Text_list() { print_list((char*)this->name(), this); }
};

extern Text_list* text_list;
void init();
void teardown();

}
