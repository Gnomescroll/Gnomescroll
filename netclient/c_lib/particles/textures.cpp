#include "textures.hpp"

#ifdef DC_CLIENT

#include <compat_gl.h>

#include <c_lib/SDL/texture_loader.h>

static GLuint particle_texture_id;

int init_particles() {
    //int i;
    //i = create_texture_from_file( (char*) "./media/texture/particles_01.png", &particle_texture_id);
    //if (i) {
        //printf("init_particles failed with code %d\n", i);
        //return 1;
    //}

    SDL_Surface *surface = IMG_Load((char*)"./media/texture/particles_01.png");

    if(!surface) { printf("text.init_test(): surface load error, %s \n", IMG_GetError()); return 0;}
    int tex_alpha = 1;
    if(surface->format->BytesPerPixel != 4) {
        printf("Font Image File: image is missing alpha channel \n");
        tex_alpha = 0;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,&particle_texture_id);
    glBindTexture(GL_TEXTURE_2D,particle_texture_id);

    if (tex_alpha) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, //rgb
                     GL_UNSIGNED_BYTE, surface->pixels);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, //rgb
                     GL_UNSIGNED_BYTE, surface->pixels);
    }

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);

    glDisable(GL_TEXTURE_2D);

    return 0;
}

#endif
