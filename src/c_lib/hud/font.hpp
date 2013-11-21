/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/compat_gl.h>

#include <options/client_options.hpp>

namespace HudFont
{

struct Glyph
{
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
    int size;
    int bold;
    int italic;
    int line_height;
    int scaleW, scaleH;
    char file[32];
    char name[32];
    char png[32];
    int num_glyphs_defined;
};

class Font
{
    private:
    void add_glyph(int c, float x, float y, float xoff, float yoff,
                   float w, float h, float xadvance);
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
    void get_string_pixel_dimension(const char* str, int *length, int *height);

    int size()
    {
        return data.size;
    }

    explicit Font(const char* fn);
};

void reset_default();
void set_properties(int size, int bold=0, int italic=0);

void init();
void teardown();

extern Font* font;

void start_font_draw(GLenum blend_func);
void start_font_draw();
void end_font_draw();
void start_world_font_draw();
void end_world_font_draw();
void set_texture();


}   // HudFont
