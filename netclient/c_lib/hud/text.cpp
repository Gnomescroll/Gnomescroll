#include "text.hpp"

#include <stdio.h>

GLuint fontTextureId;
float font_texture_width = 256.0f;
float font_texture_height = 256.0f;

static int tex_alpha = 1;
static int font_loaded = 0;

void init_glyphs();

int load_font(char* fontfile) {
    SDL_Surface *font = IMG_Load(fontfile);

    if(!font) { printf("text.init_test(): font load error, %s \n", IMG_GetError()); return 0;}
    if(font->format->BytesPerPixel != 4) {
        printf("Font Image File: image is missing alpha channel \n");
        tex_alpha = 0;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,&fontTextureId);
    glBindTexture(GL_TEXTURE_2D,fontTextureId);

    if (tex_alpha) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font->w, font->h, 0, GL_RGBA, //rgb
                     GL_UNSIGNED_BYTE, font->pixels);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, font->w, font->h, 0, GL_RGB, //rgb
                     GL_UNSIGNED_BYTE, font->pixels);
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    SDL_FreeSurface(font);

    glDisable(GL_TEXTURE_2D);
    font_loaded = 1;
    printf("Loaded font %s\n", fontfile);

    init_glyphs();
    
    return 1;
}

//void set_text_color(int r, int g, int b, int a) {
    //if (tex_alpha) {
        //glColor4ub((unsigned char)r,(unsigned char)g,(unsigned char)b,(unsigned char)a); //replace with color cordinates on texture
    //} else {
        //glColor3ub((unsigned char)r,(unsigned char)g,(unsigned char)b); //replace with color cordinates on texture
    //}
//}
    
void start_text_draw() {
    
    if (!font_loaded) {
        printf("No font loaded\n");
        return;
    }

    if (tex_alpha) {
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fontTextureId);
    glBegin(GL_QUADS);
}

void blit_glyph(
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

void end_text_draw() {
    glEnd();
    glDisable(GL_TEXTURE_2D);
    if (tex_alpha) {
        glDisable(GL_BLEND);
    }
}


/* Glyph store */

struct Glyph glyphs[128];
int missing_character = 63; // "?"

void set_missing_character(int cc) {
    missing_character = cc;
}

void init_glyphs() {
    static int once = 0;
    if (once) return;
    int i;
    for (i=0; i<128; i++) {
        glyphs[i].x = 0.0f;
        glyphs[i].y = 0.0f;
        glyphs[i].xoff = 0.0f;
        glyphs[i].yoff = 0.0f;
        glyphs[i].w = 0.0f;
        glyphs[i].h = 0.0f;
        glyphs[i].xadvance = 0.0f;
        glyphs[i].available = 0;
    }
    once++;
}

void add_glyph(
    int c,
    float x, float y,
    float xoff, float yoff,
    float w, float h,
    float xadvance
) {
    glyphs[c].x = x / font_texture_width;
    glyphs[c].y = y / font_texture_height;
    glyphs[c].tw = w / font_texture_width;
    glyphs[c].th = h / font_texture_height;
    glyphs[c].w = w;
    glyphs[c].h = h;
    glyphs[c].xoff = xoff;
    glyphs[c].yoff = yoff;
    glyphs[c].xadvance = xadvance;
    glyphs[c].available = 1;

    // tab is 4 spaces
    const int TABS_IN_SPACES = 4;
    if (c == (int)'\t')
    {
        glyphs[c].w *= TABS_IN_SPACES;
        glyphs[c].xoff *= TABS_IN_SPACES;
        glyphs[c].xadvance *= TABS_IN_SPACES;
        glyphs[c].available = 1;
    }
}

struct Glyph get_missing_glyph(unsigned char c)
{
    struct Glyph glyph;

    switch (c)
    {
        default:
            glyph = glyphs[missing_character];
            break;
    }

    return glyph;
}

void draw_text(char* t, float x, float y, float depth, float scale, float line_height)
{
    int i = 0;
    char c;
    struct Glyph glyph;

    float tx_min, tx_max, ty_min, ty_max;
    float sx_min, sx_max, sy_min, sy_max;
    float cursor_x = 0.0f;
    float cursor_y = 0.0f;

    while ((c = t[i++]) != '\0')
    {
        // newline
        if (c == '\n') {
            cursor_y += line_height;
            cursor_x = 0.0f;
            continue;
        }
        
        glyph = glyphs[(unsigned int)c];

        // known glyph?
        if (! glyph.available) {
            //printf("Character unknown: %c\n", c);
            glyph = get_missing_glyph(c);
        }

        tx_max = glyph.x;
        tx_min = glyph.x + glyph.tw;
        ty_min = glyph.y;
        ty_max = glyph.y + glyph.th;

        sx_max = x + (cursor_x + glyph.xoff) * scale;
        sx_min = x + (cursor_x + glyph.xoff + glyph.w) * scale;
        sy_min = y - (cursor_y + glyph.yoff) * scale;
        sy_max = y - (cursor_y + glyph.yoff + glyph.h) * scale;
        blit_glyph(tx_min, tx_max, ty_min, ty_max, sx_min, sx_max, sy_min, sy_max, depth);

        cursor_x += glyph.xadvance;
    }
}

void get_string_pixel_dimension(char* str, int *length, int *height)
{
    char c;
    int i = 0;
    int len = 0;
    int miny = 0;
    int maxy = 0;
    Glyph g;
    while ((c = str[i++]) != '\0')
    {
        g = glyphs[(unsigned int)c];
        if (!g.available)
        {
            g = get_missing_glyph(c);
        }

        len += g.xadvance;

        if (i==1)
        {
            len += g.xoff;
        }
        if (g.yoff + g.h > maxy) maxy = g.yoff + g.h;
        if (g.yoff < miny) miny = g.yoff;
    }
    if (i > 1)
    {
        len -= g.xadvance;
        len += g.w + g.xoff;
    }
    *length = len;
    *height = maxy - miny;
}

void draw_cursor(char* buff, int x, int y)
{
    int len = 0;
    int h = 0;
    get_string_pixel_dimension(buff, &len, &h);
    int _draw_rect(int r, int g, int b, float x, float y, float w, float h);

    int r,g,b;
    r = 100;
    g = 150;
    b = 100;
    const int w = 8;
    h = 18; // magic number precalculated;
    _draw_rect(r,g,b, x + len + 4, y - h, w, h);
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
    draw_text(this->text, this->x, this->y, this->depth, this->scale, this->line_height);
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

void init_text()
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
