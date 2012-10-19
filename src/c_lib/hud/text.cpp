#include "text.hpp"

#include <stdio.h>
#include <stdarg.h>

namespace HudText
{

static const int SHADOW_MARGIN_X = 1;
static const int SHADOW_MARGIN_Y = 1;

TextList* text_list = NULL;

void init()
{
    GS_ASSERT(text_list == NULL);
    text_list = new TextList(TEXT_MAX);
}
void teardown()
{
    if (text_list != NULL) delete text_list;
}

void blit_character(
    float tex_x_min, float tex_x_max,
    float tex_y_min, float tex_y_max,
    float screen_x_min, float screen_x_max,
    float screen_y_min, float screen_y_max,
    float depth)
{
    glBegin(GL_QUADS);
    glTexCoord2f(tex_x_min, tex_y_max);
    glVertex3f(screen_x_min, screen_y_max, depth);
    glTexCoord2f(tex_x_min, tex_y_min);
    glVertex3f(screen_x_min, screen_y_min, depth);
    glTexCoord2f(tex_x_max, tex_y_min);
    glVertex3f(screen_x_max, screen_y_min, depth);
    glTexCoord2f(tex_x_max, tex_y_max);
    glVertex3f(screen_x_max, screen_y_max, depth);
    glEnd();
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

    glBegin(GL_QUADS);
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
    glEnd();
}

void draw_string(const char* text, const float x, const float y,
                 const float depth, const float scale)
{
    draw_string(text, 0, 0, x, y, depth, scale);
}

// 0 len means all
void draw_string(const char* text, const unsigned int start, const unsigned int len,
    const float x, const float y, const float depth, const float scale)
{
    if (HudFont::font == NULL) return;

    float tx_min, tx_max, ty_min, ty_max;
    float sx_min, sx_max, sy_min, sy_max;
    float cursor_x = 0.0f;
    float cursor_y = 0.0f;
    struct HudFont::Glyph glyph;

    unsigned int i = 0;
    char c;
    // draw up to len, if len != 0. always stop at \0
    while ((len == 0 || i < start+len) && (c = text[i++]) != '\0')
    {                               
        if (c == '\n')
        {
            cursor_x = 0.0f;
            cursor_y += HudFont::font->data.line_height;
            continue;
        }

        glyph = HudFont::font->get_glyph(c);

        if (i-1 < start)
        {
            cursor_x += glyph.xadvance;
            continue;
        }

        tx_max = glyph.x;
        tx_min = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;

        sx_max = x + (cursor_x + glyph.xoff) * scale;
        sx_min = x + (cursor_x + glyph.xoff + glyph.w) * scale;
        sy_min = y - (cursor_y + glyph.yoff) * scale;
        sy_max = y - (cursor_y + glyph.yoff + glyph.h) * scale;

        blit_character(tx_min, tx_max, ty_min, ty_max,
                       sx_min, sx_max, sy_min, sy_max, depth);

        cursor_x += glyph.xadvance;                 
    }
}

void Text::draw_centered()
{
    center();

    if (this->shadowed)
    {
        glColor4ub(0,0,0,color.a);
        draw_string(this->text, this->x-SHADOW_MARGIN_X, this->y-SHADOW_MARGIN_Y, this->depth, this->scale);
    }
    
    glColor4ub(color.r,color.g,color.b,color.a);
    draw_string(this->text, this->x, this->y, this->depth, this->scale);
}

// internal string copy
// if string is changed, char buffer will expand
// however, char buffer will never contract
char* Text::set_string(const char* text, char* this_text, unsigned int* this_len)
{
    unsigned int len = (unsigned int)strlen(text);
    if (this_text == NULL)
    {   // first time adding
        this_text = (char*)malloc(sizeof(char) * (len+1));
        *this_len = len;
    }
    else
    if (len > *this_len)
    {   // string is greater size
        char* new_this_text = (char*)realloc(this_text, sizeof(char)*(len+1));
        GS_ASSERT(new_this_text != NULL);
        if (new_this_text == NULL)
        {
            free(this_text);
            this_text = NULL;
            len = 0;
        }
        else this_text = new_this_text;
        
        *this_len = len;
    }
    // copy string over
    strcpy(this_text, text);
    return this_text;
}

void Text::draw_character_rotated(float theta)
{   // draws as single character. glyph alignment offset not used
    using HudFont::font;
    if (font == NULL)
        return;
    GS_ASSERT(this->text != NULL);
    if (this->text == NULL) return;

    char c = this->text[0];
    if (c == '\0') return;
    struct HudFont::Glyph glyph = font->get_glyph(c);

    float sx,sy, sw,sh;

    sx = this->x - glyph.w/2;
    sw = glyph.w * this->scale;
    sy = this->y - glyph.h/2;
    sh = glyph.h * this->scale;

    glColor4ub(color.r,color.g,color.b,color.a);
    //blit_character_rotated(glyph.x, glyph.x+glyph.tw, glyph.y, glyph.y+glyph.th, sx,sx+sw,sy,sy+sh, this->depth, theta);
    draw_bound_texture_rotated(sx, sy, sw, sh, glyph.x, glyph.y, glyph.tw, glyph.th, this->depth, theta);
}

void Text::draw_character_rotated_centered(float theta)
{   // draws as single character. glyph alignment offset not used
    this->center();
    this->draw_character_rotated(theta);
}

char* Text::grow_string(unsigned int n, char* str, unsigned int* str_len)
{
    GS_ASSERT(*str_len < n);
    if (*str_len >= n) return str;
    if (str == NULL)
        str = (char*)malloc(sizeof(char) * (n+1));
    else
    {
        char* new_str = (char*)realloc(str, sizeof(char) * (n+1));
        GS_ASSERT(new_str != NULL);
        if (new_str == NULL)
        {
            free(str);
            str = NULL;
            n = 0;
        }
        else str = new_str;
    }
    *str_len = n;
    return str;
}

void Text::set_text(const char* text)
{
    if (text == NULL) text = "";
    this->text = this->set_string(text, this->text, &this->text_len);
    this->reset_alignment();
}

void Text::set_format(const char* format)
{
    if (format == NULL)
    {
        format = "";
        this->formatted = false;
    }
    else
        this->formatted = true;
    this->format = this->set_string(format, this->format, &this->format_len);
}

void Text::set_format_extra_length(int size)
{
    // size large enough to fit formatted data
    // subtract format string sizes
    // eg "id=%d" -- size would be 8 (+10 for int, -2 for %d), 
    this->formatted_extra_len = size;
}

void Text::update_formatted_string(int n_args, ...)
{
    int len = this->format_len + this->formatted_extra_len + 1;
    GS_ASSERT(len > 0);
    if (len > (int)this->text_len)
    {
        unsigned int new_len = this->text_len;
        char* new_text = grow_string(len, this->text, &new_len);
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
    this->color.r = r;
    this->color.g = g;
    this->color.b = b;
}

void Text::set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    this->color.r = r;
    this->color.g = g;
    this->color.b = b;
    this->color.a = a;
}

void Text::set_color(struct Color color)
{
    this->color.r = color.r;
    this->color.g = color.g;
    this->color.b = color.b;
}

void Text::set_color(struct Color4 color)
{
    this->color = color;
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
    return (int)strlen(this->text);
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
    if (this->alignment.center) return;
    float w = this->get_width();
    this->x = this->refx - w/2;    // -/+ is weird because of the character vertex draw order
    this->alignment.center = true;
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

void Text::draw()
{
    if (this->text == NULL || this->text_len == 0)
        return;

    if (this->shadowed)
    {
        glColor4ub(0,0,0,color.a);
        draw_string(this->text, this->x-SHADOW_MARGIN_X, this->y-SHADOW_MARGIN_Y, this->depth, this->scale);
    }
    
    glColor4ub(color.r,color.g,color.b,color.a);
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
text_len(0),
format_len(0),
formatted_extra_len(0),
formatted(false),
id(id),
depth(-1.0f),
scale(1.0f),
color(color_init(255,255,255,255)),
text(NULL),
format(NULL),
x(0.0f), y(0.0f),
refx(0.0f),refy(0.0f),
shadowed(false)
{
    this->reset_alignment();
}

Text::Text()
:
text_len(0),
format_len(0),
formatted_extra_len(0),
formatted(false),
id(-1),
depth(-1.0f),
scale(1.0f),
color(color_init(255,255,255,255)),
text(NULL),
format(NULL),
x(0.0f), y(0.0f),
refx(0.0f),refy(0.0f),
shadowed(false)
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

}   // HudText
