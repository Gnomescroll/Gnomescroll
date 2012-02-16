#pragma once


#include "struct.hpp"

namespace font_loader
{
    

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

//// DELETE THIS
struct _glyph
{
    float x, y, tw, th;
    float w, h;
    float xadvance;
    float xoff, yoff;

    //// temporary
    unsigned char* bitmap;
};

//extern void add_glyph(int c,  float x, float y,  float xoff, float yoff,   float w, float h, float xadvance     );
//extern glyph glyphs[];

class CFontSys
{
    private:
        void GenerateTexture(int n_glyphs);
        //bool make_char(class FT_Face* face, unsigned char ch);
        bool make_char(void* face, unsigned char ch);
        
        void add_glyph(
            int c,
            float x, float y,
            float xoff, float yoff,
            float w, float h,
            float xadvance
        );
        struct _glyph* glyphs;
    public:
        CFontSys(struct _font_meta* meta);
        ~CFontSys();

        int error;

        void GenerateFont(const char* path,int fontsize);
        //struct SDL_Surface* GenerateSDL_Surface();
        //void save_font_to_file(char* filename);

        // Font Bitmap values
        // READ ONLY!
        unsigned char* bitmap; // 32 bit bitmap RGBA
        int font_texture_width;
        int font_texture_height;
        int segment_width;
        int segment_height;
};

}