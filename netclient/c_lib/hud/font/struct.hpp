#pragma once

struct _glyph
{
    float x, y;
    float tw, th;
    float w, h;
    float xadvance;
    float xoff, yoff;
};

struct _font_meta
{
    unsigned short bitmap_width;
    unsigned short bitmap_height;
    struct _glyph glyphs[256];
};