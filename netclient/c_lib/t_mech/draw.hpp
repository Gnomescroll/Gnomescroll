#pragma once

#include <c_lib/SDL/texture_loader.hpp>

namespace t_mech
{

SDL_Surface* surface = NULL;

GLuint mech_sheet = 0 ;

void draw_init()
{

	printf("t_mech::draw_init() \n");

	surface = create_surface_from_file((char*) "./media/texture/mechanism/wires.png");

	int tex_format;
    if (surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;
    else
        tex_format = GL_BGRA;

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &mech_sheet);
    glBindTexture(GL_TEXTURE_2D, mech_sheet);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,  //target
        0,  // level
        4,  // internalformat, use 4 for 4 bytes per pixel
        surface->w, surface->h, //width, height
        0,  // border
        tex_format, // format
        GL_UNSIGNED_BYTE,   // type
        surface->pixels // pixels
    );

    glDisable(GL_TEXTURE_2D);

}

void draw_teardown()
{

}
	
}