#pragma once

#include <c_lib/SDL/texture_loader.hpp>

#include <c_lib/physics/vec3.hpp>
#include <c_lib/camera/camera.hpp>

namespace Skybox
{

SDL_Surface* surface;

GLuint star_sheet;

struct STAR
{
	int type;
	float brightness;
	float size;
	float phi;
	float theta;
};

struct STAR* star_list;
int star_num = 0;

void init()
{
	surface = _load_image((char*) "./media/textures/skybox/Starsheet.png");

	int tex_format;
    if (surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;
    else
        tex_format = GL_BGRA;

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &star_sheet);
    glBindTexture(GL_TEXTURE_2D, star_sheet);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

void teardown()
{


}

void draw()
{

	star_num = 256;

    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);

	for(int i; i<star_num; i++)
	{
		Vec3 v;

		v.x = (float) i;
		v.y = (float) i;
		v.z = 256.0;

		float scale = 1.0;

	    if (current_camera == NULL) return;

	    Vec3 up = vec3_init(
	        model_view_matrix[0]*scale,
	        model_view_matrix[4]*scale,
	        model_view_matrix[8]*scale
	    );
	    Vec3 right = vec3_init(
	        model_view_matrix[1]*scale,
	        model_view_matrix[5]*scale,
	        model_view_matrix[9]*scale
	    );

	    float tx_min, tx_max, ty_min, ty_max;
	/*
	    tx_min = (float)(texture_index%16)* (1.0/16.0);
	    tx_max = tx_min + (1.0/16.0);
	    ty_min = (float)(texture_index/16)* (1.0/16.0);
	    ty_max = ty_min + (1.0/16.0);
	*/
	    tx_min = 0.0f;
	    tx_max = 0.5f;
	    ty_min = 0.0f;
	    ty_max = 0.5f;

	    Vec3 p = vec3_sub(v, vec3_add(right, up));
	    glTexCoord2f(tx_min,ty_max);
	    glVertex3f(p.x, p.y, p.z);

	    p = vec3_add(v, vec3_sub(up, right));
	    glTexCoord2f(tx_max,ty_max);
	    glVertex3f(p.x, p.y, p.z);

	    p = vec3_add(v, vec3_add(up, right));
	    glTexCoord2f(tx_max,ty_min);
	    glVertex3f(p.x, p.y, p.z);

	    p = vec3_add(v, vec3_sub(right, up));
	    glTexCoord2f(tx_min,ty_min);
	    glVertex3f(p.x, p.y, p.z);

	}

	glEnd();
	//glDisable(GL_TEXTURE_2D);
}

}

extern "C"
{
	void add_star(int type, float brightness, float size, float phi, float theta)
	{

	}

	void clear_stars()
	{
		Skybox::star_num = 0;
	}
}