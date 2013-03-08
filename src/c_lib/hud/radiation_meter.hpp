#pragma once


namespace Hud
{


class HudRadiationMeter
{
	public:
	struct SDL_Surface* rad_tex_surface; //_load_image(const char *file);
	struct SDL_Surface* grad_tex_surface; //_load_image(const char *file);

	unsigned int rad_tex;
	unsigned int rad_gradient_tex;

	HudRadiationMeter()
	{
/*

    glEnable(GL_TEXTURE_2D);
    glGenTextures(2, map_textures);
    for (int i=0; i<2; i++)
    {
        GS_ASSERT(map_textures[i] != 0);
        if (map_textures[i] == 0) continue;
        glBindTexture(GL_TEXTURE_2D, map_textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //GL_BGRA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, map_surface->w, map_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, map_surface->pixels);
    }
    glDisable(GL_TEXTURE_2D);
    CHECK_GL_ERROR();
*/

	}

	~HudRadiationMeter()
	{

	}

	void init()
	{
    	const GLenum tex_format = GL_BGRA;

		rad_tex_surface  = create_surface_from_file(MEDIA_PATH "sprites/icons/radiation_hud.png");
		grad_tex_surface = create_surface_from_file(MEDIA_PATH "sprites/gradient/heightmap_gradient_01.png");

	    glEnable(GL_TEXTURE_2D);
	    glGenTextures(1, &rad_tex);
	    glBindTexture(GL_TEXTURE_2D, rad_tex);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rad_tex_surface->w, rad_tex_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, rad_tex_surface->pixels);
	    glDisable(GL_TEXTURE_2D);

	    //GL_BGRA
	    glDisable(GL_TEXTURE_1D);
	    glGenTextures(1, &rad_gradient_tex);
	    glBindTexture(GL_TEXTURE_1D, rad_gradient_tex);
	    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, grad_tex_surface->w, 0, tex_format, GL_UNSIGNED_BYTE, grad_tex_surface->pixels);

	    glDisable(GL_TEXTURE_1D);
	}

	void draw(int x, int y)
	{

		static int c = 0;
		c++;

		int h = 96;

		float x0 = x;
		float x1 = x + h;
		float y0 = y;
		float y1 = y + h;

	    const float z = -0.1f;

	    glEnable(GL_TEXTURE_2D);
	    glBindTexture(GL_TEXTURE_2D, rad_tex);
	    //glEnable(GL_BLEND);
	    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    
	    int i = (c /15) % grad_tex_surface->w;
	    int _r = ((unsigned char*)grad_tex_surface->pixels)[4*i + 0];
	    int _g = ((unsigned char*)grad_tex_surface->pixels)[4*i + 1];
	    int _b = ((unsigned char*)grad_tex_surface->pixels)[4*i + 2];

	    glColor3ub(_r,_g,_b);


	    glBegin(GL_QUADS);
	    glTexCoord2i(0, 0);
	    glVertex3f(x0, y0, z);
	    glTexCoord2i(1, 0);
	    glVertex3f(x1, y0, z);
	    glTexCoord2i(1, 1);
	    glVertex3f(x1, y1, z);
	    glTexCoord2i(0, 1);
	    glVertex3f(x0, y1, z);
	    glEnd();

	    glDisable(GL_TEXTURE_2D);
	    glColor3ub(255, 255, 255);
	}
};


}
/*
void Reticle::draw()
{
    IF_ASSERT(!this->inited) return;
    IF_ASSERT(this->tex_data.tex == 0) return;

    const float z = -0.1f;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->tex_data.tex);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3ub(255, 0, 185);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex3f(x0, y0, z);
    glTexCoord2i(1, 0);
    glVertex3f(x1, y0, z);
    glTexCoord2i(1, 1);
    glVertex3f(x1, y1, z);
    glTexCoord2i(0, 1);
    glVertex3f(x0, y1, z);
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glColor3ub(255, 255, 255);
}
*/
