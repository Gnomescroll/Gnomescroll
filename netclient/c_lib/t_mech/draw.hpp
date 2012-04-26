#pragma once
#include <SDL/texture_loader.hpp>

#include <t_mech/state.hpp>

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

/*
struct MECH
{
	int x,y,z; //position
	int type;  //type
	int direction; //direction

	bool active;
};
*/

static const int v_index[72] = 
{
    1,1,1 , 0,1,1 , 0,0,1 , 1,0,1 , //top
    0,1,0 , 1,1,0 , 1,0,0 , 0,0,0 , //bottom
    1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 , //north
    0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 , //south
    1,1,1 , 1,1,0 , 0,1,0,  0,1,1 , //west
    0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 , //east
};


static const float t_index[8] =
{
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
};


void draw()
{

	glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
	glEnable (GL_BLEND);
	
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE, GL_ONE);

    glBindTexture(GL_TEXTURE_2D, mech_sheet);

	//float tx_min, tx_max, ty_min, ty_max;

    glBegin(GL_QUADS);
/*
	for(int i=0; i<mech_num; i++)
	{

        struct MECH m = mech_array[i];

        int ti;


        ti = 0;

        glTexCoord2f( t_index[ti+0],t_index[ti+1] );
        glVertex3f(p.x, p.y, p.z);

        ti = 2;

        glTexCoord2f( t_index[ti+0],t_index[ti+1] );
        glVertex3f(p.x, p.y, p.z);

        ti = 4;

        glTexCoord2f( t_index[ti+0],t_index[ti+1] );
        glVertex3f(p.x, p.y, p.z);

        ti = 6;
        glTexCoord2f( t_index[ti+0],t_index[ti+1] );
        glVertex3f(p.x, p.y, p.z);
	}
*/
	glEnd();

}


}