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


Text::Text()
:
width(10),height(10),
line_height(18),
str_len(0),
id(-1),
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

static int hud_text_inited = 0;
static const int HUD_TEXT_MAX = 256;
static int text_index = 0;
static Text* hud_text[HUD_TEXT_MAX];

void init()
{
    if (hud_text_inited) return;
    for (int i=0; i<HUD_TEXT_MAX; hud_text[i++] = new Text());
    hud_text_inited++;
}

// CYTHON
Text* create_text()
{
    if (!hud_text_inited)
    {
        printf("ERROR create_text -- text not inited.\n");
        return NULL;
    }
    if (text_index >= HUD_TEXT_MAX)
    {
        printf("HUD_TEXT MAX %d exceeded\n", HUD_TEXT_MAX);
        return NULL;
    }
    
    Text* t = hud_text[text_index];
    t->id = text_index++;
    t->inited = true;
    return t;
}

void draw_text(int id)
{
    if (!hud_text_inited)
    {
        printf("ERROR draw_text -- text not inited.\n");
        return;
    }
    if (id < 0 || id >= HUD_TEXT_MAX)
    {
        printf("draw_text -- invalid text id %d\n", id);
        return;
    }

    Text* t = hud_text[id];
    if (!t->inited)
    {
        printf("text %d not inited\n", id);
        return;
    }

    t->draw();
}

void set_text(int id, char* text)
{
    if (!hud_text_inited)
    {
        printf("ERROR set_text -- text not inited.\n");
        return;
    }
    if (id < 0 || id >= HUD_TEXT_MAX)
    {
        printf("set_text -- invalid text id %d\n", id);
        return;
    }

    Text* t = hud_text[id];
    if (!t->inited)
    {
        printf("text %d not inited\n", id);
        return;
    }

    t->set_text(text);
}

void set_color(int id, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    if (!hud_text_inited)
    {
        printf("ERROR set_color -- text not inited.\n");
        return;
    }
    if (id < 0 || id >= HUD_TEXT_MAX)
    {
        printf("set_color -- invalid text id %d\n", id);
        return;
    }

    Text* t = hud_text[id];
    if (!t->inited)
    {
        printf("text %d not inited\n", id);
        return;
    }

    t->set_color(r,g,b,a);
}

void set_y(int id, float y)
{
    if (!hud_text_inited)
    {
        printf("ERROR set_y -- text not inited.\n");
        return;
    }
    if (id < 0 || id >= HUD_TEXT_MAX)
    {
        printf("set_y -- invalid text id %d\n", id);
        return;
    }

    Text* t = hud_text[id];
    if (!t->inited)
    {
        printf("text %d not inited\n", id);
        return;
    }

    t->y = y;
}

float get_yoffset(int id)
{
    if (!hud_text_inited)
    {
        printf("ERROR get_yoffset -- text not inited.\n");
        return 0.0f;
    }
    if (id < 0 || id >= HUD_TEXT_MAX)
    {
        printf("get_yoffset -- invalid text id %d\n", id);
        return 0.0f;
    }

    Text* t = hud_text[id];
    if (!t->inited)
    {
        printf("text %d not inited\n", id);
        return 0.0f;
    }

    return t->yoff;
}

float get_y(int id)
{
    if (!hud_text_inited)
    {
        printf("ERROR get_y -- text not inited.\n");
        return 0.0f;
    }
    if (id < 0 || id >= HUD_TEXT_MAX)
    {
        printf("get_y -- invalid text id %d\n", id);
        return 0.0f;
    }

    Text* t = hud_text[id];
    if (!t->inited)
    {
        printf("text %d not inited\n", id);
        return 0.0f;
    }

    return t->y;
}

float get_x(int id)
{
    if (!hud_text_inited)
    {
        printf("ERROR get_x -- text not inited.\n");
        return 0.0f;
    }
    if (id < 0 || id >= HUD_TEXT_MAX)
    {
        printf("get_x -- invalid text id %d\n", id);
        return 0.0f;
    }

    Text* t = hud_text[id];
    if (!t->inited)
    {
        printf("text %d not inited\n", id);
        return 0.0f;
    }

    return t->x;
}

}
