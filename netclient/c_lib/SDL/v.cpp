#include "v.hpp"

#include <libnoise/noise.h>
#include <c_lib/SDL/draw_functions.h>
#include <c_lib/SDL/SDL_functions.h>
#include <c_lib/SDL/pngfuncs.h>

namespace vn {

using namespace noise;

static module::Voronoi V;
static module::Turbulence F;

//static const int width = 800;
//static const int height = 600;
static int width = 1280;
static int height = 800;
//static const int width = 256;
//static const int height = 256;
//static const int width = 1024;
//static const int height = 1024;

static float widthf = (float)width;
static float heightf = (float)height;

static int grad_num = 1;


static SDL_Surface* surface;
static GLuint texture;

static SDL_Surface* gradient_surface;
//static GLuint gradient_texture;

void init() {

    // load gradient surface
    const char grad_fmt[] = "media/texture/heightmap_gradient_%02d.png";
    char grad_str[strlen(grad_fmt) -2 +1];
    sprintf(grad_str, grad_fmt, grad_num);
    gradient_surface = create_surface_from_file(grad_str);
    if (gradient_surface==NULL) {
        printf("Voronoi gradient surface is NULL\n");
        return;
    }
    
    /* Init blank draw surface */
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

    glTexImage2D( GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, tex_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);

    //// generate surface
    //SDL_LockSurface(surface);
    //set_pixels();
    //SDL_UnlockSurface(surface);

    //// bind surface to texture
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, texture);
    //glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
    //glDisable(GL_TEXTURE_2D);
}

void init_vn(int w, int h, int grad) {
    width = w;
    height = h;
    widthf = (float)w;
    heightf = (float)h;
    grad_num = grad;
    init();
}

const int ripple_phase = 0;
void set_pixels(double z) {
    int i,j;
    double x,y;
    double val;
    Uint32 pix;
    Uint8 r,g,b,a;
    unsigned char c;
    for (i=0; i<width; i++) {
        x = ((double)i)/widthf;
        for (j=0; j<height; j++) {
            y = ((double)j)/heightf;
            val = V.GetValue(x,y,z);
            //val = F.GetValue(x,y,z);
            c = abs(((int)(val*1000))%255);
            //c = ((int)(val*((255+ripple_phase)/2))+((255+ripple_phase)/2)) %255;
            //c = (val*128)+127;
            pix = ((Uint32*)gradient_surface->pixels)[c];
            SDL_GetRGBA(pix, gradient_surface->format, &r, &g, &b, &a);
            ((Uint32*)surface->pixels)[i + j*width] = SDL_MapRGBA(surface->format, r,g,b,a);
        }
        //p_index += width;
    }
}
void set_pixels_turbulent(double z) {
    int i,j;
    double x,y;
    double val;
    Uint32 pix;
    Uint8 r,g,b,a;
    unsigned char c;
    for (i=0; i<width; i++) {
        x = ((double)i)/widthf;
        for (j=0; j<height; j++) {
            y = ((double)j)/heightf;
            //val = V.GetValue(x,y,z);
            val = F.GetValue(x,y,z);
            c = abs(((int)(val*1000))%255);
            //c = ((int)(val*((255+ripple_phase)/2))+((255+ripple_phase)/2)) %255;
            //c = (val*128)+127;
            pix = ((Uint32*)gradient_surface->pixels)[c];
            SDL_GetRGBA(pix, gradient_surface->format, &r, &g, &b, &a);
            ((Uint32*)surface->pixels)[i + j*width] = SDL_MapRGBA(surface->format, r,g,b,a);
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

static double z_step = 0.0013;
static bool use_turbulence = false;
void set_params(double dz, double frequency, int seed, bool distance, bool turbulence_enabled, double turbulence_frequency, double turbulence_power) {
//void set_params(double dz, double frequency, int seed, bool distance, double turbulence_frequency, double turbulence_power) {
    z_step = dz;
    // set voronoi parameters
    V.SetFrequency(frequency);
    //V.SetFrequency(8);
    V.SetSeed(seed);
    V.EnableDistance(distance);

    if (turbulence_enabled) {
        F.SetSourceModule(0, V);
        F.SetFrequency (turbulence_frequency);
        F.SetPower (turbulence_power);
    }
    use_turbulence = turbulence_enabled;
}

void generate_frames(float seconds) {
    const int fps = 30;
    int frames = (int)((float)fps * seconds);

    int i;
    double z = 0.1;
    char fn_fmt[] = "media/gif/voronoi-%03d.png";
    char fn[strlen(fn_fmt)-1 +1];
    for (i=0; i<frames; i++) {
        //z = (double)i/(double)frames;
        z += z_step;
        if (use_turbulence) {
            set_pixels_turbulent(z);
        } else {
            set_pixels(z);
        }
        sprintf(fn, fn_fmt, i);
        png_save_surface(fn, surface);  // lock first?
    }
}

}
