#include "text.hpp"

#include <stdio.h>


namespace HudText
{

void draw_string(char* text, float x, float y, float depth, float scale, float line_height)
{
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
        
        glyph = HudFont::glyphs[(unsigned int)c];

        if (! glyph.available) {
            glyph = HudFont::get_missing_glyph(c);
        }

        tx_max = glyph.x;
        tx_min = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;

        sx_max = x + (cursor_x + glyph.xoff) * scale;
        sx_min = x + (cursor_x + glyph.xoff + glyph.w) * scale;
        sy_min = y - (cursor_y + glyph.yoff) * scale;
        sy_max = y - (cursor_y + glyph.yoff + glyph.h) * scale;
        HudFont::blit_glyph(tx_min, tx_max, ty_min, ty_max, sx_min, sx_max, sy_min, sy_max, depth);

        cursor_x += glyph.xadvance;
    }
}

void Text::set_text(char* text)
{
    int len = strlen(text);
    if (len == 0) {
        this->str_len = 0;
        if (this->text != NULL)
        {
            this->text[0] = '\0';
        }
        return;
    }
    if (this->text != NULL)
    {
        if (strcmp(text, this->text))
        {
            // string is different
            if (this->str_len == 0)
            {
                // recalculate str_len
                this->str_len = strlen(this->text);
            }
            if (len != this->str_len)
            {
                // string is different size
                this->text = (char*)realloc(this->text, sizeof(char)*(len+1));
                this->str_len = len;
            }
            
            strcpy(this->text, text);
        }
    }
    else
    {
        // new
        this->text = (char*)malloc(sizeof(char) * (len+1));
        strcpy(this->text, text);
        this->str_len = len;
    }
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
    if (this->text == NULL || this->str_len == 0) return;
    glColor4ub(r,g,b,a);
    draw_string(this->text, this->x, this->y, this->depth, this->scale, this->line_height);
}


Text::Text(int id)
:
width(10),height(10),
line_height(18),
str_len(0),
id(id),
inited(false),
depth(-0.1),
scale(1.0),
r(255),g(255),b(255),a(255),
text(NULL),
x(0), y(0),
xoff(0),yoff(0)
{}

Text::~Text()
{
    if (this->text != NULL)
        free(this->text);
}

}
