#include "v.hpp"

#include <libnoise/noise.h>
#include <SDL/draw_functions.h>
#include <SDL/SDL_functions.h>

namespace vn {

using namespace noise;

static module::Voronoi V;

static const int width = 800;
static const int height = 600;
static const float widthf = (float)width;
static const float heightf = (float)height;


static SDL_Surface* surface;
static GLuint texture;

void init() {
    /* Init blank map surface */
    surface = create_surface_from_nothing(width, height);
    if (surface==NULL) {
        printf("Voronoi blank surface is NULL\n");
        return;
    }

    Uint32 tex_format = GL_BGRA;
    if (surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;
    
    // texture
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //GL_BGRA
    glTexImage2D( GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, tex_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);

    SDL_LockSurface(surface);
    set_pixels();
    SDL_UnlockSurface(surface);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
    glDisable(GL_TEXTURE_2D);
}

void set_pixels() {
    int i,j;
    double x,y;
    double val;
    Uint32 pix;
    for (i=0; i<width; i++) {
        x = ((float)i)/widthf;
        for (j=0; j<height; j++) {
            y = ((float)j)/heightf;
            val = V.GetValue(x,y, 0.5);
            pix = (Uint32)(val*128 +128);
            //pix = (Uint32)((unsigned char)(val));
            //printf("%d\n", pix);
            ((Uint32*)surface->pixels)[i + j*width] = SDL_MapRGBA(surface->format, pix,pix,pix,255);
            //((Uint32*)surface->pixels)[i + j*width] = SDL_MapRGB(surface->format, pix,pix,pix);
        }
        //p_index += width;
    }
}

void draw() {

    static const float z = -0.5f;
    static const int x = 0;
    static const int y = 0;
    static const int w = width;
    static const int h = height;

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texture);
    draw_bound_texture(x,y,w,h,z);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

}


//cython
void draw_vn() {
    draw();
}

}
