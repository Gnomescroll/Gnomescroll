#include "text.hpp"

#include <stdio.h>
#include <stdarg.h>

namespace HudText
{
    
void start_text_draw() {
    if (HudFont::font == NULL)
    {
        printf("No font loaded\n");
        return;
    }

    if (HudFont::font->alpha) {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, HudFont::font->texture);
    glBegin(GL_QUADS);
}

void end_text_draw() {
    glEnd();
    glDisable(GL_TEXTURE_2D);
    if (HudFont::font->alpha) {
        glDisable(GL_BLEND);
    }
}

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



void draw_string(char* text, float x, float y, float depth, float scale, float line_height)
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
        if (c == '\n') {
            cursor_y += line_height;
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

// internal string copy
// if string is changed, char buffer will expand
// however, char buffer will never contract
void Text::set_string(char* text, char** this_text, int* this_len)
{
    int len = strlen(text);
    if (*this_text == NULL)
    {
        // first time adding
        *this_text = (char*)malloc(sizeof(char) * (len+1));
        strcpy(*this_text, text);
        *this_len = len;
    }
    else
    {
        if (len > *this_len)
        {
            // string is greater size
            *this_text = (char*)realloc(*this_text, sizeof(char)*(len+1));
            *this_len = len;
        }
        strcpy(*this_text, text);
    }
}

void Text::resize_string(int n, char** str, int* str_len)
{
    if (*str_len == n) return;
    if (*str == NULL)
        *str = (char*)malloc(sizeof(char) * (n+1));
    else
        *str = (char*)realloc(*str, sizeof(char) * (n+1));
    *str_len = n;
}

void Text::set_text(char* text)
{
    this->set_string(text, &this->text, &this->text_len);
}

void Text::set_format(char* format)
{
    this->set_string(format, &this->format, &this->format_len);
    this->formatted = true;
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
    if (len > this->text_len)
        resize_string(len, &this->text, &this->text_len);
    va_list args;
    va_start(args, n_args);
    vsprintf(this->text, this->format, args);
    va_end(args);
}

void Text::set_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}

void Text::set_position(float x, float y)
{
    static int first = 0;
    this->x = x;
    this->y = y;
    if (!first)
    {
        this->xoff = x;
        this->yoff = y;
    }
    first++;
}

void Text::set_scale(float scale)
{
    this->scale = scale;
}

void Text::set_depth(float depth)
{
    this->depth = depth;
}

void Text::draw()
{
    if (this->text == NULL || this->text_len == 0)
        return;
    glColor4ub(r,g,b,a);
    draw_string(this->text, this->x, this->y, this->depth, this->scale, this->line_height);
}


Text::Text(int id)
:
width(10),height(10),
line_height(18),
text_len(0),
format_len(0),
formatted_extra_len(0),
id(id),
inited(false),
depth(-0.1),
scale(1.0),
formatted(false),
r(255),g(255),b(255),a(255),
text(NULL),
format(NULL),
x(0), y(0),
xoff(0),yoff(0)
{}

Text::~Text()
{
    if (this->text != NULL)
        free(this->text);
    if (this->format != NULL)
        free(this->format);
}

Text_list text_list;

}
