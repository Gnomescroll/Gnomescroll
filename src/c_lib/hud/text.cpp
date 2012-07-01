#include "text.hpp"

#include <stdio.h>
#include <stdarg.h>

namespace HudText
{

void blit_character(
    float tex_x_min, float tex_x_max,
    float tex_y_min, float tex_y_max,
    float screen_x_min, float screen_x_max,
    float screen_y_min, float screen_y_max,
    float depth)
{
    glTexCoord2f(tex_x_min, tex_y_max);
    glVertex3f(screen_x_min, screen_y_max, depth);
    glTexCoord2f(tex_x_min, tex_y_min);
    glVertex3f(screen_x_min, screen_y_min, depth);
    glTexCoord2f(tex_x_max, tex_y_min);
    glVertex3f(screen_x_max, screen_y_min, depth);
    glTexCoord2f(tex_x_max, tex_y_max);
    glVertex3f(screen_x_max, screen_y_max, depth);
}

void blit_character_rotated(
    float tex_x_min, float tex_x_max,
    float tex_y_min, float tex_y_max,
    float screen_x_min, float screen_x_max,
    float screen_y_min, float screen_y_max,
    float depth, float theta)
{
    theta *= kPI;
    float cx,cy;
    cx = (screen_x_max - screen_x_min) / 2 + screen_x_min;
    cy = (screen_y_max - screen_y_min) / 2 + screen_y_min;

    float rx,ry;
    
    glTexCoord2f(tex_x_min+cx, tex_y_max);
    rotate_point(screen_x_min-cx, screen_y_max-cy, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, depth);

    glTexCoord2f(tex_x_min, tex_y_min);
    rotate_point(screen_x_min-cx, screen_y_min-cy, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, depth);

    glTexCoord2f(tex_x_max, tex_y_min);
    rotate_point(screen_x_min-cx, screen_y_min-cy, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, depth);

    glTexCoord2f(tex_x_max, tex_y_max);
    rotate_point(screen_x_max-cx, screen_y_max-cy, theta, &rx, &ry);
    glVertex3f(rx+cx, ry+cy, depth);
}

void draw_string(char* text, float x, float y, float depth, float scale)
{
    if (HudFont::font == NULL)
        return;
    int i = 0;
    char c;
    struct HudFont::Glyph glyph;

    float tx_min, tx_max, ty_min, ty_max;
    float sx_min, sx_max, sy_min, sy_max;
    float cursor_x = 0.0f;
    float cursor_y = 0.0f;

    while ((c = text[i++]) != '\0')
    {
        if (c == '\n')
        {
            cursor_y += HudFont::font->data.line_height;
            cursor_x = 0.0f;
            continue;
        }
        
        glyph = HudFont::font->get_glyph(c);

        tx_max = glyph.x;
        tx_min = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;

        sx_max = x + (cursor_x + glyph.xoff) * scale;
        sx_min = x + (cursor_x + glyph.xoff + glyph.w) * scale;
        sy_min = y - (cursor_y + glyph.yoff) * scale;
        sy_max = y - (cursor_y + glyph.yoff + glyph.h) * scale;
        blit_character(tx_min, tx_max, ty_min, ty_max, sx_min, sx_max, sy_min, sy_max, depth);

        cursor_x += glyph.xadvance;
    }
}

void Text::draw_centered()
{
    center();
    glColor4ub(r,g,b,a);
    draw_string(this->text, this->x, this->y, this->depth, this->scale);
}

// internal string copy
// if string is changed, char buffer will expand
// however, char buffer will never contract
void Text::set_string(char* text, char** this_text, unsigned int* this_len)
{
    unsigned int len = strlen(text);
    if (*this_text == NULL)
    {   // first time adding
        *this_text = (char*)malloc(sizeof(char) * (len+1));
        strcpy(*this_text, text);
        *this_len = len;
    }
    else
    {
        if (len > *this_len)
        {   // string is greater size
            *this_text = (char*)realloc(*this_text, sizeof(char)*(len+1));
            *this_len = len;
        }
        strcpy(*this_text, text);
    }
}

void Text::draw_character_rotated(float theta)
{   // draws as single character. glyph alignment offset not used
    using HudFont::font;
    if (font == NULL)
        return;

    char c = this->text[0];
    if (c == '\0') return;
    struct HudFont::Glyph glyph = font->get_glyph(c);

    float sx,sy, sw,sh;

    sx = this->x - glyph.w/2;
    sw = glyph.w * scale;
    sy = this->y - glyph.h/2;
    sh = glyph.h * scale;

    glColor4ub(r,g,b,a);
    draw_bound_texture_rotated(sx, sy, sw, sh, glyph.x, glyph.y, glyph.tw, glyph.th, this->depth, theta);
}

char* Text::resize_string(unsigned int n, char* str, unsigned int* str_len)
{
	GS_ASSERT(*str_len < n);
    if (str == NULL)
        str = (char*)malloc(sizeof(char) * (n+1));
    else
        str = (char*)realloc(str, sizeof(char) * (n+1));
    *str_len = n;
    return str;
}

void Text::set_text(char* text)
{
	GS_ASSERT(text != NULL);
	if (text == NULL) text = (char*)"";
    this->set_string(text, &this->text, &this->text_len);
}

void Text::set_format(char* format)
{
    this->set_string(format, &this->format, &this->format_len);
    this->formatted = true;
}

void Text::set_format_extra_length(unsigned int size)
{
    // size large enough to fit formatted data
    // subtract format string sizes
    // eg "id=%d" -- size would be 8 (+10 for int, -2 for %d), 
    this->formatted_extra_len = size;
}

void Text::update_formatted_string(int n_args, ...)
{
    unsigned int len = this->format_len + this->formatted_extra_len + 1;
    if (len > this->text_len)
    {
		unsigned int new_len = this->text_len;
        char* new_text = resize_string(len, this->text, &new_len);
        if (new_text != NULL)
        {
			this->text = new_text;
			this->text_len = new_len;
		}
	}
    va_list args;
    va_start(args, n_args);
    vsprintf(this->text, this->format, args);
    va_end(args);
}

void Text::set_color(unsigned char r, unsigned char g, unsigned char b)
{
    this->r = r;
    this->g = g;
    this->b = b;
}

void Text::set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void Text::set_color(struct Color color)
{
    this->r = color.r;
    this->g = color.g;
    this->b = color.b;
}

void Text::set_position(float x, float y)
{
    this->x = x;
    this->y = y;
    this->refx = x;
    this->refy = y;
}

void Text::set_scale(float scale)
{
    this->scale = scale;
}

void Text::set_depth(float depth)
{
    this->depth = depth;
}

int Text::charcount()
{
    if (this->text == NULL)
        return 0;
    return strlen(this->text);
}

void Text::reset_alignment()
{
    this->alignment.center = false;
    this->alignment.left = false;
    this->alignment.right = false;
    this->alignment.top = false;
    this->alignment.bottom = false;
    this->x = this->refx;
    this->y = this->refy;
}
void Text::center()
{
    if (!this->alignment.center)
    {
        float w = this->get_width();
        float h = this->get_height();
        this->x = this->refx - w/2;    // -/+ is weird because of the character vertex draw order
        this->y = this->refy + h/2;
        this->alignment.center = true;
    }
}
void Text::left()
{
    this->alignment.left = true;
}
void Text::right()
{
    this->alignment.right = true;
}
void Text::top()
{
    this->alignment.top = true;
}
void Text::bottom()
{
    this->alignment.bottom = true;
}

void Text::Text::draw()
{
    if (this->text == NULL || this->text_len == 0)
        return;
    glColor4ub(r,g,b,a);
    draw_string(this->text, this->x, this->y, this->depth, this->scale);
    this->reset_alignment();
}

int Text::get_width()
{
    if (this->text == NULL || this->text_len == 0 || HudFont::font == NULL)
        return 0;
    char* buffer = (char*)calloc(this->text_len+1, sizeof(char));

    // check length of each line, return longest
    int len = 0, h = 0;
    int longest = 0;
    int i = 0;
    int j = 0;
    char c;
    while ((c = this->text[i++]) != '\0')
    {
        if (c == '\n')
        {
            buffer[j++] = '\0';
            j = 0;
            HudFont::font->get_string_pixel_dimension(buffer, &len, &h);
            if (len > longest)
                longest = len;
            continue;
        }
        buffer[j++] = c;
    }
    buffer[j++] = '\0';
    HudFont::font->get_string_pixel_dimension(buffer, &len, &h);
    if (len > longest)
        longest = len;

    free(buffer);
    return longest;
}

int Text:: get_height()
{
    if (this->text == NULL || this->text_len == 0 || HudFont::font == NULL)
        return 0;

    // count number of newlines with !isspace chars after them + 1
    // multiply by font height
    int i = 0;
    int n = 1;
    bool check = false;
    char c;
    while ((c = this->text[i++]) != '\0')
    {
        if (check && !isspace(c))
        {
            check = false;
            n++;
        }
        if (c == '\n')
            check = true;    
    }

    int len=0,h=0;
    HudFont::font->get_string_pixel_dimension((char*)"X", &len, &h);
    return n * h;
}

Text::Text(int id)
:
width(10.0f),height(10.0f),
text_len(0),
format_len(0),
formatted_extra_len(0),
id(id),
depth(-1.0f),
scale(1.0f),
formatted(false),
r(255),g(255),b(255),a(255),
text(NULL),
format(NULL),
x(0.0f), y(0.0f),
refx(0.0f),refy(0.0f)

{
    this->reset_alignment();
}

Text::Text()
:
width(10.0f),height(10.0f),
text_len(0),
format_len(0),
formatted_extra_len(0),
id(-1),
depth(-1.0f),
scale(1.0f),
formatted(false),
r(255),g(255),b(255),a(255),
text(NULL),
format(NULL),
x(0.0f), y(0.0f),
refx(0.0f),refy(0.0f)
{
    this->reset_alignment();
}

Text::~Text()
{
    if (this->text != NULL)
        free(this->text);
    if (this->format != NULL)
        free(this->format);
}

void init()
{
    if (text_list == NULL)
    text_list = new Text_list;
}
void teardown()
{
    if (text_list != NULL)
        delete text_list;
}

Text_list* text_list = NULL;

}
