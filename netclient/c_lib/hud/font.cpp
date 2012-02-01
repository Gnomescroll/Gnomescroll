#include "font.hpp"

#include <compat_gl.h>

namespace HudFont
{

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



/* Glyph store */

struct Glyph glyphs[128];
int missing_character = '?';

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

}
