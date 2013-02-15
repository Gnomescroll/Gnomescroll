#pragma once

#include <common/template/object_list.hpp>
#include <common/color.hpp>

namespace HudText
{

const unsigned int TEXT_MAX = 1024;

void draw_string(const char* text, const float x, const float y,
                 const float depth, const float scale);
void draw_string(const char* text, const unsigned int start,
                 const unsigned int len, const float x, const float y,
                 const float depth, const float scale);

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
        size_t text_len;
        size_t format_len;
        int formatted_extra_len;
        bool formatted;
    char* set_string(const char* text, char* this_text, size_t* this_len);
    char* grow_string(size_t n, char* str, size_t *str_len);

    public:
        int id;
        float depth;
        float scale;
        Color color;
        char* text;
        char* format;
        float x,y;  // active x,y
        float refx, refy; // reference x,y
        bool shadowed;
        TextAlignment alignment;

    void reset_alignment();
    void center();
    void left();
    void right();
    void top();
    void bottom();

    bool is_formatted()
    {
        return this->formatted;
    }

    void set_text(const char* text);
    void set_format(const char* format);
    void set_format_extra_length(int size);
    void update_formatted_string(int n_args, ...);

    void set_color(Color color);

    void set_position(float x, float y);
    void set_scale(float scale);
    void set_depth(float depth);
    void draw_centered();
    int get_width();
    int get_height();
    int charcount();

    void draw();
    // draws as single character. glyph alignment offset not used:
    void draw_character_rotated(float theta);
    void draw_character_rotated_centered(float theta);

    // assignment operator
    // needed, because this object is tracked in a Simple_object_list
    // which does swaps on objects
    // and we hold a reference to dynamically allocated memory
    void operator=(Text& t)
    {
        if (this == &t) return;
        this->set_text(t.text);
        this->set_format(t.format);
        this->formatted_extra_len = t.formatted_extra_len;
        this->id = t.id;
        this->depth = t.depth;
        this->scale = t.scale;
        this->color = t.color;
        this->x = t.x;
        this->y = t.y;
        this->refx = t.refx;
        this->refy = t.refy;
        this->alignment = t.alignment;
        this->shadowed = t.shadowed;
    }

    explicit Text(int id);
    Text();
    ~Text();
};

void* set_and_update_array_range(unsigned int n, unsigned int* max,
                                 unsigned int* n_final, void* arr,
                                 size_t obj_size)
{
    if (n == *max) return NULL;
    *max = n;

    if (n == 0)
    {
        free(arr);
        return NULL;
    }

    if (*n_final >= *max)   // readjust count
        *n_final = (*max)-1;

    if (arr == NULL)        // alloc new buffer
        return malloc(n * obj_size);
    else                    // resize existing buffer
        return realloc(arr, n * obj_size);
}

class AnimatedText: public Text
{
    public:
        unsigned int max_colors;
        unsigned int n_colors;
        Color* colors;

        struct CharRange
        {
            unsigned int start;
            int end;    // <0 means to the end
            unsigned int color_index;
        };
        unsigned int max_char_ranges;
        unsigned int n_char_ranges;
        struct CharRange* char_ranges;

    void set_color_count(unsigned int n)
    {
        this->colors = (Color*)set_and_update_array_range(
            n, &this->max_colors, &this->n_colors, this->colors,
            sizeof(Color));
    }

    int add_color(Color color)
    {
        IF_ASSERT(this->n_colors >= this->max_colors) return -1;
        IF_ASSERT(this->colors == NULL) return -1;
        this->colors[this->n_colors] = color;
        return this->n_colors++;
    }

    int get_color(Color color)
    {
        for (size_t i=0; i<this->n_colors; i++)
            if (colors_equal_alpha(this->colors[i], color))
                return i;
        return -1;
    }

    void set_color_index_color(int color_index, Color color)
    {
        if (color_index < 0) return;
        if (color_index >= (int)this->n_colors) return;
        this->colors[color_index] = color;
    }

    void set_char_range_count(unsigned int n)
    {
        this->char_ranges = (struct CharRange*)
            set_and_update_array_range(n,
                                       &this->max_char_ranges,
                                       &this->n_char_ranges,
                                       this->char_ranges,
                                       sizeof(struct CharRange));
    }

    int add_char_range(unsigned int char_start, int char_end)
    {
        IF_ASSERT(this->n_char_ranges >= this->max_char_ranges) return -1;
        IF_ASSERT(this->char_ranges == NULL) return -1;
        this->char_ranges[this->n_char_ranges].start = char_start;
        this->char_ranges[this->n_char_ranges].end = char_end;
        this->char_ranges[this->n_char_ranges].color_index = -1;
        return this->n_char_ranges++;
    }

    void update_char_range(int char_range_index, unsigned int start, int end)
    {
        if (char_range_index < 0) return;
        IF_ASSERT(char_range_index >= (int)this->max_char_ranges) return;
        this->char_ranges[char_range_index].start = start;
        this->char_ranges[char_range_index].end = end;
    }

    void set_char_range_color(int char_range_index, int color_index)
    {
        if (color_index < 0) return;
        IF_ASSERT(color_index >= (int)this->max_colors) return;
        if (char_range_index < 0) return;
        IF_ASSERT(char_range_index >= (int)this->max_char_ranges) return;
        this->char_ranges[char_range_index].color_index = color_index;
    }

    int get_color_range(int color_index)
    {
        if (color_index < 0) return -1;
        for (size_t i=0; i<this->n_char_ranges; i++)
            if (this->char_ranges[i].color_index == (unsigned int)color_index)
                return i;
        return -1;
    }

    int get_char_range(int color_index, unsigned int char_start, int char_end)
    {
        if (color_index < 0) return -1;
        for (size_t i=0; i<this->n_char_ranges; i++)
            if (this->char_ranges[i].color_index == (unsigned int)color_index &&
                this->char_ranges[i].start == char_start &&
                this->char_ranges[i].end == char_end)
            {
                return i;
            }
        return -1;
    }

    void draw()
    {
        if (this->text == NULL || this->text_len == 0)
            return;

        for (size_t i=0; i<this->n_char_ranges; i++)
        {
            int color_index = this->char_ranges[i].color_index;
            if (color_index < 0 || color_index >= int(this->n_colors)) continue;

            unsigned int start = this->char_ranges[i].start;
            int end = this->char_ranges[i].end;
            if (end >= 0 && end <= int(start)) continue;
            int range = end - int(start);
            if (range < 0) range = 0;

            Color c = this->colors[this->char_ranges[i].color_index];
            glColor4ub(c.r, c.g, c.b, c.a);

            draw_string(this->text, start, range, this->x, this->y,
                        this->depth, this->scale);
        }

        // TODO -- draw uncovered line segments with base color
        // for now require user to manage uncovered line segments

        this->reset_alignment();
    }

    void operator=(AnimatedText& t)
    {   // copy any data
        if (this != &t) // handle self copy
        {   // copy colors
            this->set_color_count(t.max_colors);
            this->n_colors = 0;
            for (size_t i=0; i<t.n_colors; i++)
                this->add_color(t.colors[i]);

            // copy char ranges
            this->set_char_range_count(t.max_char_ranges);
            this->n_char_ranges = 0;
            for (size_t i=0; i<t.n_char_ranges; i++)
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
        free(this->colors);
        free(this->char_ranges);
    }
};

class TextList: public ObjectList<Text>
{
    private:

    const char* name()
    {
        return "Text";
    }

    public:

    explicit TextList(size_t capacity) : ObjectList<Text>(capacity)
    {
    }
};

extern TextList* text_list;
void init();
void teardown();

}
