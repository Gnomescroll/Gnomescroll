#pragma once

#include <compat_gl.h>

int load_font(char* fontfile);

void start_text_draw();
void set_text_color(int r, int g, int b, int a);
void end_text_draw();
void draw_text(char *t, int len, float x, float y, float depth);
void blit_glyph(
    float tex_x_min, float tex_x_max,
    float tex_y_min, float tex_y_max,
    float screen_x_min, float screen_x_max,
    float screen_y_min, float screen_y_max,
    float depth
);


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

void set_missing_character(int cc);

extern struct Glyph glyphs[128];
extern int missing_character; // "?"

extern GLuint fontTextureId;
