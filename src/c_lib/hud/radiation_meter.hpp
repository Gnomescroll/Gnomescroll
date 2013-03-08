#pragma once




class HudRadiationMeter
{

	SDL_Surface* rad_tex_surface; //_load_image(const char *file);
	SDL_Surface* grad_tex_surface; //_load_image(const char *file);

	unsigned int rad_tex;
	unsigned int Texture rad_gradient;

	HudRadiationMeter()
	{


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


	}

	~HudRadiationMeter()
	{

	}

	void init()
	{

	}
};



void Reticle::init(const char* filename)
{
    IF_ASSERT(this->inited) return;
    _load_image_create_texture(filename, &this->tex_data);
    this->align_center(_xres, _yres);
    this->inited = true;
}

void Reticle::align_center(int window_width, int window_height)
{
    float center_x = float(window_width) / 2.0f;
    float center_y = float(window_height) / 2.0f;

    this->x0 = center_x - (float(this->tex_data.w) / 2.0f);
    this->y0 = center_y - (float(this->tex_data.h) / 2.0f);
    this->x1 = center_x + (float(this->tex_data.w) / 2.0f);
    this->y1 = center_y + (float(this->tex_data.h) / 2.0f);
}

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

void init()
{

	rad_tex_surface = create_surface_from_file(MEDIA_PATH "sprites/icon/radiation_hud.png");
	grad_tex_surface = create_surface_from_file(MEDIA_PATH "sprites/gradient/heightmap_gradient_01.png");

    GL_ENABLE(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, map_textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GS_ENABLE(GL_TEXTURE_1D)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rad_tex_surface->w, rad_tex_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, rad_tex_surface->pixels);
    GL_DISABLE(GL_TEXTURE_2D)

    //GL_BGRA
    GS_ENABLE(GL_TEXTURE_1D)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_1D, 0, GL_RGBA, rad_tex_surface->w, rad_tex_surface->h, 0, tex_format, GL_UNSIGNED_BYTE, rad_tex_surface->pixels);

    GL_DISABLE(GL_TEXTURE_1D)

}
