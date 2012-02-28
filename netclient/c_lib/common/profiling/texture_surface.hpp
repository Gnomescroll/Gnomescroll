#pragma once

struct PIXEL
{
	union
	{
		struct 
		{
			unsigned char r,g,b,a;
		};
		unsigned int color;
	};
};

class Texture_surface
{
	int xdim;
	int ydim;

	SDL_Surface* surface;

	struct PIXEL pixels*;

	bool update;

	int tex[2]; //textures
	int tex_index;

	Texture_surface(int x, int y)
	{
		xdim = x;
		ydim = y;

		//x/y must be a power of two

		tex_index = 0;

		surface = create_surface_from_nothing(x, y);
		pixels = (struct PIXEL) calloc(x*y*sizeof(uint));

		glEnable(GL_TEXTURE_2D);
    	glGenTextures( 2, &tex );

    	glBindTexture( GL_TEXTURE_2D, tex[0] );
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, xdim, ydim, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels );

    	glBindTexture( GL_TEXTURE_2D, tex[1] );
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, xdim, ydim, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels );

	}

	~Texture_surface()
	{
		SDL_FreeSurface(surface);
		free(pixels);

	}

	void update()
	{

		glBindTexture(GL_TEXTURE_2D, tex[0] );    //A texture you have already created with glTexImage2D
   		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, xdim, ydim, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

	}
	void draw(int x, int y)
	{


	}

	void set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
    block_surface_pixel_format = block_surface->format;
    must_lock_lock_surface = SDL_MUSTLOCK(block_surface);
    block_surface_width = (int)block_surface->w;
    block_surface_height = (int)block_surface->h;

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &block_texture );
    glBindTexture( GL_TEXTURE_2D, block_texture );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    int texture_format = GL_BGRA;

    //formats: GL_BGRA_EXT, GL_SRGB_ALPHA_EXT, GL_SRGBA_EXT

    //internal format, input format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8_EXT, block_surface->w, block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );
    //texture without gamma correction
    glGenTextures( 1, &block_texture_no_gamma_correction );
    glBindTexture( GL_TEXTURE_2D, block_texture_no_gamma_correction );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, block_surface->w,block_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, block_surface->pixels );

    glDisable(GL_TEXTURE_2D);

	}
};