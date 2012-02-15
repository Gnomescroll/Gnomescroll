#pragma once

/*
    Usage: create CFontSys instance and call GenerateFont as many times as you want
    then use CFontSys::bitmap to make texture. its in RGBARGBARGBA... format (actually values are same)

    error can be checked by error int value
    0 = NO_ERROR
    1 = FT_Init_FreeType failed
    2 = FT_New_Face failed (there is probably a problem with your font file)
    3 = FT_Load_Glyph failed
    4 = FT_Get_Glyph failed

    REQUIRES:
    add_glyph function
    add_glyph(int c, float x, float y, float xoff, float yoff, float w, float h, float xadvance )

*/

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <c_lib/hud/font.hpp>

//// DELETE THIS
//struct glyph
//{
    //float x, y, tw, th;
    //float w, h;
    //float xadvance;
    //float xoff, yoff;

    //// temporary
    //unsigned char* bitmap;
//};

//extern void add_glyph(int c,  float x, float y,  float xoff, float yoff,   float w, float h, float xadvance     );
//extern glyph glyphs[];

class CFontSys
{
    private:
        FT_Library library;
        bool make_char(FT_Face face,unsigned char ch);
        void GenerateTexture(int n_glyphs);

    public:
        CFontSys();
        ~CFontSys();

        int error;

        void GenerateFont(const char* path,int fontsize);

        // Font Bitmap values
        // READ ONLY!
        unsigned char* bitmap; // 32 bit bitmap RGBA
        int font_texture_width;
        int font_texture_height;
        int segment_width;
        int segment_height;
};

void load_fonts();
