#pragma once

#include <common/template/object_list.hpp>
#include <common/color.hpp>

namespace HudText
{

void draw_string(const char* text, const float x, const float y, const float depth, const float scale);
void draw_string(const char* text, const unsigned int start, const unsigned int len, const float x, const float y, const float depth, const float scale);

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
    protected:
        unsigned int text_len;
        unsigned int format_len;
        int formatted_extra_len;
        bool formatted;
        char* set_string(const char* text, char* this_text, unsigned int* this_len);
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

        void set_text(const char* text);
        void set_format(const char* format);
        void set_format_extra_length(int size);
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

void* set_and_update_array_range(unsigned int n, unsigned int* max, unsigned int* n_final, void* arr, size_t obj_size)
{
	if (n == *max) return NULL;
	*max = n;
	
	if (n == 0)
	{
		if (arr != NULL) free(arr);
		return NULL;
	}

	if (*n_final >= *max)	// readjust count
		*n_final = (*max)-1;

	if (arr == NULL)	 	// alloc new buffer
		return malloc(n * obj_size);
	else    				// resize existing buffer
		return realloc(arr, n * obj_size);
}

class AnimatedText: public Text
{
	public:
	
		unsigned int max_colors;
		unsigned int n_colors;
		struct Color4* colors;
	
		struct CharRange
		{
			unsigned int start;
			int end;	// <0 means to the end
			unsigned int color_index;
		};
		unsigned int max_char_ranges;
		unsigned int n_char_ranges;
		struct CharRange* char_ranges;
	
		void set_color_count(unsigned int n)
		{
			this->colors = (struct Color4*)set_and_update_array_range(
				n, &this->max_colors, &this->n_colors,
				this->colors, sizeof(struct Color4));
		}
	
		int add_color(struct Color color)
		{
			struct Color4 colora;
			colora.r = color.r;
			colora.g = color.g;
			colora.b = color.b;
			colora.a = 255;
			return this->add_color(colora);
		}
		
		int add_color(struct Color4 color)
		{
			GS_ASSERT(this->n_colors < this->max_colors);
			GS_ASSERT(this->colors != NULL);
			if (this->n_colors >= this->max_colors) return -1;
			if (this->colors == NULL) return -1;
			this->colors[this->n_colors] = color;
			return this->n_colors++;
		}

		int get_color(struct Color color)
		{
			for (unsigned int i=0; i<this->n_colors; i++)
				if (this->colors[i].r == color.r
				 && this->colors[i].g == color.g
				 && this->colors[i].b == color.b)
					return i;
			return -1;
		}
		
		int get_color(struct Color4 color)
		{
			for (unsigned int i=0; i<this->n_colors; i++)
				if (this->colors[i].r == color.r
				 && this->colors[i].g == color.g
				 && this->colors[i].b == color.b
				 && this->colors[i].a == color.a)
					return i;
			return -1;
		}
		
		void set_color_index_color(int color_index, struct Color color)
		{
			struct Color4 colora;
			colora.r = color.r;
			colora.g = color.g;
			colora.b = color.b;
			colora.a = 255;
			this->set_color_index_color(color_index, colora);
		}
		
		void set_color_index_color(int color_index, struct Color4 color)
		{
			if (color_index < 0) return;
			if (color_index >= (int)this->n_colors) return;
			this->colors[color_index] = color;
		}

		void set_char_range_count(unsigned int n)
		{
			this->char_ranges = (struct CharRange*)set_and_update_array_range(
				n, &this->max_char_ranges, &this->n_char_ranges, 
				this->char_ranges, sizeof(struct CharRange));
		}
		
		int add_char_range(unsigned int char_start, int char_end)
		{
			GS_ASSERT(this->n_char_ranges < this->max_char_ranges);
			GS_ASSERT(this->char_ranges != NULL);
			if (this->n_char_ranges >= this->max_char_ranges) return -1;
			if (this->char_ranges == NULL) return -1;
			this->char_ranges[this->n_char_ranges].start = char_start;
			this->char_ranges[this->n_char_ranges].end = char_end;
			this->char_ranges[this->n_char_ranges].color_index = -1;
			return this->n_char_ranges++;
		}
		
		void update_char_range(int char_range_index, unsigned int start, int end)
		{
			if (char_range_index < 0) return;
			GS_ASSERT(char_range_index < (int)this->max_char_ranges);
			if (char_range_index >= (int)this->max_char_ranges) return;
			this->char_ranges[char_range_index].start = start;
			this->char_ranges[char_range_index].end = end;
		}
		
		void set_char_range_color(int char_range_index, int color_index)
		{
			if (color_index < 0) return;
			GS_ASSERT(color_index < (int)this->max_colors);
			if (color_index >= (int)this->max_colors) return;
			if (char_range_index < 0) return;
			GS_ASSERT(char_range_index < (int)this->max_char_ranges);
			if (char_range_index >= (int)this->max_char_ranges) return;
			this->char_ranges[char_range_index].color_index = color_index;
		}
		
		int get_color_range(int color_index)
		{
			if (color_index < 0) return -1;
			for (unsigned int i=0; i<this->n_char_ranges; i++)
				if (this->char_ranges[i].color_index == (unsigned int)color_index)
					return i;
			return -1;
		}
		
		int get_char_range(int color_index, unsigned int char_start, int char_end)
		{
			if (color_index < 0) return -1;
			for (unsigned int i=0; i<this->n_char_ranges; i++)
				if (this->char_ranges[i].color_index == (unsigned int)color_index
				 && this->char_ranges[i].start == char_start
				 && this->char_ranges[i].end == char_end)
					return i;
			return -1;
		}
		
	void draw()
	{
		if (this->text == NULL || this->text_len == 0)
			return;
		
		for (unsigned int i=0; i<this->n_char_ranges; i++)
		{
			int color_index = this->char_ranges[i].color_index;
			if (color_index < 0 || color_index >= (int)this->n_colors) continue;

			unsigned int start = this->char_ranges[i].start;
			int end = this->char_ranges[i].end;
			if (end >= 0 && end <= (int)start) continue;
			int range = end - (int)start;
			if (range < 0) range = 0;

			struct Color4 c = this->colors[this->char_ranges[i].color_index];
			glColor4ub(c.r, c.g, c.b, c.a);

			draw_string(this->text, start, range,
						 this->x, this->y, this->depth, this->scale);
		}
		
		// TODO -- draw uncovered line segments with base color
		// for now require user to manage uncovered line segments
		 
		this->reset_alignment();
	}
		
	void operator=(AnimatedText& t)
	{	// copy any data
		if (this != &t)	// handle self copy
		{	// copy colors
			this->set_color_count(t.max_colors);
			this->n_colors = 0;
			for (unsigned int i=0; i<t.n_colors; i++)
				this->add_color(t.colors[i]);
				
			// copy char ranges
			this->set_char_range_count(t.max_char_ranges);
			this->n_char_ranges = 0;
			for (unsigned int i=0; i<t.n_char_ranges; i++)
			{
				int index = this->add_char_range(t.char_ranges[i].start, t.char_ranges[i].end);
				this->set_char_range_color(index, t.char_ranges[i].color_index);
			}
		}
		Text::operator=(t);
	}
	
	AnimatedText() :
	max_colors(0), n_colors(0), colors(NULL),
	max_char_ranges(0), n_char_ranges(0), char_ranges(NULL)
	{}
	
	~AnimatedText()
	{
		if (this->colors != NULL)
		{
			free(this->colors);
			this->colors = NULL;
		}
		if (this->char_ranges != NULL)
		{
			free(this->char_ranges);
			this->char_ranges = NULL;
		}
	}
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
