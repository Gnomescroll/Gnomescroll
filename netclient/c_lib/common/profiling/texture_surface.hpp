#pragma once

struct PIXEL
{
	union
	{
		struct 
		{
			unsigned char b,g,r,a; //GL_BGRA
		};
		unsigned int color;
	};
};

class Texture_surface
{
	int xdim;
	int ydim;

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
		update = false;

		//surface = create_surface_from_nothing(x, y);
		pixels = (struct PIXEL) calloc(x*y*sizeof(uint));

		glEnable(GL_TEXTURE_2D);
    	glGenTextures( 2, &tex );

    	glBindTexture( GL_TEXTURE_2D, tex[0] );
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, xdim, ydim, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels );

    	/*
    	glBindTexture( GL_TEXTURE_2D, tex[1] );
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    	glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, xdim, ydim, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels );
		*/

		glDisable(GL_TEXTURE_2D);
	}

	~Texture_surface()
	{
		free(pixels);
		glDeleteTextures( 2, &tex );
	}

	void update()
	{
		glBindTexture(GL_TEXTURE_2D, tex[0] );    //A texture you have already created with glTexImage2D
   		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, xdim, ydim, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

	}

	void draw(int x, int y)
	{

	    glColor3ub(255,255,255);

	    //glEnable(GL_TEXTURE_2D);
	    glBindTexture(GL_TEXTURE_2D, tex[0]);
	    
	    //glEnable(GL_BLEND);
	    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	   
	    glBegin(GL_QUADS);
	    {
   			const float z = -0.5;

	        glTexCoord2f(0.0,0.0);
	        glVertex3f(x, y, z);  // Top left

	        glTexCoord2f(1.0,0.0);
	        glVertex3f(x+xdim, y, z);  // Top right

	        glTexCoord2f(1.0,1.0);
	        glVertex3i(x+xdim, y-ydim, z);  // Bottom right

	        glTexCoord2f(0.0,1.0);
	        glVertex3i(x, y-ydim, z);  // Bottom left
	    }
	    glEnd();


	    if(update == true) update();
	}

	void set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		update = true;

		struct PIXEL p;

		p.r = r;
		p.g = g;
		p.b = b;
		p.a = a;

		pixels[y*xdim + x].color = p.color;
	}
};