#include "font_loader.hpp"

// First Power Of 2
inline int next_p2 (int a )
{
    int rval=1;
    while(rval<a)
        rval*=2;

    return rval;
}

CFontSys::CFontSys()
:
bitmap(NULL)
{
    error = 0;

    // Initializing FT library
    if(FT_Init_FreeType(&library))
        error = 1;
}

void CFontSys::GenerateFont(const char* path,int fontsize)
{
    FT_Face face;

    if(FT_New_Face(library,path,0,&face))
    {
        error = 2;
        return;
    }

    // FreeType Font Size = 1/64ths Of Pixels
    // 1 pixel = size 64
    FT_Set_Char_Size(face,fontsize*64,fontsize*64,96,96);

    bool res = true;

    // Generating HudFont::glyphs
    for(unsigned char b = 0;;b++)
    {
        res = make_char(face,b);

        if(!res)
            break;

        if(b==255)
            break;
    }

    if(res)
        GenerateTexture(256);

    FT_Done_Face(face);
}

CFontSys::~CFontSys()
{
    if (this->bitmap != NULL)
        delete[] this->bitmap;

    // free glyph bitmaps
        
    FT_Done_FreeType(library);
}

bool CFontSys::make_char(FT_Face face,unsigned char ch)
{
    if(FT_Load_Glyph(face,FT_Get_Char_Index(face,ch),FT_LOAD_DEFAULT))
    {
        error = 3;
        return false;
    }

    FT_Glyph glyph;
    if(FT_Get_Glyph(face->glyph, &glyph ))
    {
        error = 4;
        return false;
    }

    FT_Glyph_To_Bitmap(&glyph,ft_render_mode_normal,0,1);
    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

    FT_Bitmap& bitmap = bitmap_glyph->bitmap;

    //int width = next_p2( bitmap.width );
    int width = bitmap.width;
    //int height = next_p2( bitmap.rows );
    int height = bitmap.rows;

    HudFont::add_glyph(ch,
         0, 0, // those will be later filled by GenerateTexture function
         bitmap_glyph->left, bitmap_glyph->top-bitmap.rows, // bitmap.rows ??????????
         width, height,
         face->glyph->advance.x/64 // in pixels
     );

    unsigned char* expanded_data;
    if(width*height>0)
        expanded_data = new unsigned char[2*width*height];
    else
        expanded_data = NULL;
    // first = brightness  // second = alpha

    unsigned char colorr;

    if(width*height>0)
        HudFont::glyphs[ch].bitmap = new unsigned char[width*height];
    else
        HudFont::glyphs[ch].bitmap = NULL;

    for(int j=0; j <height;j++)
    {
        for(int i=0; i < width; i++)
        {
            expanded_data[2*(i+j*width)] = expanded_data[2*(i+j*width)+1] =
            (i>=bitmap.width || j>=bitmap.rows) ?
            0 : bitmap.buffer[i + bitmap.width*j];

            if(i>=bitmap.width || j>=bitmap.rows)
                colorr = 0;
            else
                colorr = bitmap.buffer[i + bitmap.width*j];

            HudFont::glyphs[ch].bitmap[i + bitmap.width*j] = colorr;
        }
    }

    delete[] expanded_data;

    return true;
}

void CFontSys::GenerateTexture(int n_glyphs)
{
    // finding lowest power2 of maximum width and height of glyph
    int max_width = 0;
    int max_height = 0;

    for(int g = 0;g<n_glyphs;g++)
    {
        if(HudFont::glyphs[g].w>max_width)
            max_width = HudFont::glyphs[g].w;

        if(HudFont::glyphs[g].h>max_height)
            max_height = HudFont::glyphs[g].h;
    }

    segment_width = next_p2(max_width);
    segment_height = next_p2(max_height);

    font_texture_width = segment_width*16;
    font_texture_height = segment_height*16;

    //printf("ft w, ft h = %d %d\n", font_texture_width, font_texture_height);

    bitmap = new unsigned char[4*font_texture_width*font_texture_height];

    int OFF_X = 0;
    int OFF_Y = 0;

    // filling texture bitmap and setting glyph values
    for(int g = 0;g<n_glyphs;g++)
    {
        OFF_X = (g%16)*segment_width;
        OFF_Y = (g/16)*segment_height;

        HudFont::glyphs[g].x = OFF_X/float(font_texture_width);
        HudFont::glyphs[g].y = OFF_Y/float(font_texture_height);
        HudFont::glyphs[g].tw = HudFont::glyphs[g].w/float(font_texture_width);
        HudFont::glyphs[g].th = HudFont::glyphs[g].h/float(font_texture_height);

        int w,h;
        w = HudFont::glyphs[g].w;
        h = HudFont::glyphs[g].h;
        if (g == '\t')
            w /= 4;

        for(int j=0;j<h;j++)
        {
            for(int i=0;i<w;i++)
            {
                unsigned char ccolor = HudFont::glyphs[g].bitmap[i + int(w)*j];
                bitmap[(OFF_X+i)*4+(OFF_Y+j)*4*font_texture_width+0] = ccolor;//r
                bitmap[(OFF_X+i)*4+(OFF_Y+j)*4*font_texture_width+1] = ccolor;//g
                bitmap[(OFF_X+i)*4+(OFF_Y+j)*4*font_texture_width+2] = ccolor;//b
                bitmap[(OFF_X+i)*4+(OFF_Y+j)*4*font_texture_width+3] = ccolor;//a
            }
        }
    }
}


static CFontSys cfont;
void load_fonts()
{
    HudFont::init_glyphs();

    char ff[] = "/usr/share/wine/fonts/tahoma.ttf";
    cfont.GenerateFont(ff, 18);
    printf("CFontSys error: %d\n", cfont.error);

    // make texture from bmp
    if (cfont.bitmap == NULL)
    {
        printf("ERROR bitmap is NULL\n");
        return;
    }
    HudFont::load_font_from_bitmap(cfont.bitmap, cfont.font_texture_width, cfont.font_texture_height);
}
