#pragma once

#include <common/template/object_list.hpp>
#include <common/color.hpp>

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
        unsigned int text_len;
        unsigned int format_len;
        unsigned int formatted_extra_len;
        bool formatted;
        char* set_string(char* text, char* this_text, unsigned int* this_len);
        char* grow_string(unsigned int n, char* str, unsigned int *str_len);

    public:
        int id;
        float depth;
        float scale;

        struct Color4 color;

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

        bool is_formatted() { return this->formatted; }

        void set_text(char* text);
        void set_format(char* format);
        void set_format_extra_length(unsigned int size);
        void update_formatted_string(int n_args, ...);
        
        void set_color(unsigned char r, unsigned char g, unsigned char b);
        void set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void set_color(struct Color color);
        void set_color(struct Color4 color);
        
        void set_position(float x, float y);
        void set_scale(float scale);
        void set_depth(float depth);
        void draw_centered();
        int get_width();
        int get_height();
        int charcount();
        
        void draw();
        void draw_character_rotated(float theta);   // draws as single character. glyph alignment offset not used

		// assignment operator
		// needed, because this object is tracked in a Simple_object_list
		// which does swaps on objects
		// and we hold a reference to dynamically allocated memory
		// WARNING: the originating object t (that is being assigned from)
		// 		is altered in the process.
		void operator=(Text& t)
		{
			set_text(t.text);
			set_format(t.format);
            formatted_extra_len = t.formatted_extra_len;
            
            id = t.id;
            depth = t.depth;
            scale = t.scale;
            color = t.color;
            x = t.x;
            y = t.y;
            refx = t.refx;
            refy = t.refy;
            alignment = t.alignment;
		}

        explicit Text(int id);
        Text();
        ~Text();
};

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
