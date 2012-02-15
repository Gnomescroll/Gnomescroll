#pragma once

#include <compat_gl.h>

#include <c_lib/options.hpp>

namespace HudFont
{

struct Glyph {
    float x;
    float y;
    float xoff;
    float yoff;
    float w;
    float h;
    float tw;
    float th;
    float xadvance;
    bool available;
};

// metadata of a font set
struct FontData
{
    int line_height;
    int scaleW, scaleH;
    char file[32];
    char png[32];
    int num_glyphs_defined;
};

class Font
{
    private:
    void add_glyph(
        int c,
        float x, float y,
        float xoff, float yoff,
        float w, float h,
        float xadvance
    );
    void update_glyphs();
    void parse_font_file();
    void load_font_png();

    public:

    struct FontData data;

    GLuint texture;
    float tex_width;
    float tex_height;
    bool alpha;

    struct Glyph glyphs[128];
    char missing_glyph;
    struct Glyph get_missing_glyph(unsigned char c);
    struct Glyph get_glyph(unsigned char c);
    void get_string_pixel_dimension(char* str, int *length, int *height);

    Font(char* fn)
    :
    alpha(true),
    missing_glyph('?')
    {
        // init glyphs
        for (int i=0; i<128; i++)
        {
            glyphs[i].x = 0.0f;
            glyphs[i].y = 0.0f;
            glyphs[i].xoff = 0.0f;
            glyphs[i].yoff = 0.0f;
            glyphs[i].w = 0.0f;
            glyphs[i].h = 0.0f;
            glyphs[i].xadvance = 0.0f;
            glyphs[i].available = 0;
        }

        // save filename
        strcpy(data.file, fn);

        parse_font_file();  // gets all metadata (line height, png file) and glyphs
        load_font_png();    // load font png from acquired filename
        update_glyphs();    // post process glyphs (scaling texture coordinates; tab character)
    }
};

void init();
void teardown();

extern Font** fonts;
extern const int n_fonts;
extern Font* font;



}
