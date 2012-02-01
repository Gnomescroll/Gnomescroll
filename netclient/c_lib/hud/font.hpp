#pragma once

namespace HudFont
{

int load_font(char* fontfile);

/* Glyphs */

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
    int available;
};

void add_glyph(
    int c,
    float x, float y,
    float xoff, float yoff,
    float w, float h,
    float xadvance
);

extern struct Glyph glyphs[128];

void set_missing_character(int cc);
struct Glyph get_missing_glyph(unsigned char c);
extern int missing_character; // "?"

extern GLuint fontTextureId;
extern float font_texture_width;
extern float font_texture_height;

void get_string_pixel_dimension(char* str, int *length, int *height);

}
