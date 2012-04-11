#pragma once

namespace Animations 
{

int insect_mob_surface;
unsigned int insect_mob_texture;

void init_insect_mob_texture();
void init_insect_mob_shader();

void init_insect_mob()
{
	init_insect_mob_texture();
}


void init_insect_mob_texture()
{
	SDL_Surface* s = create_surface_from_file((char*) "./media/sprites/AlienTex3.png");

    if(s == NULL)
    {
        printf("init_insect_mob: texture load error\n");
        abort();
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &insect_mob_texture );
    glBindTexture( GL_TEXTURE_2D, insect_mob_texture  );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    GLuint internalFormat = GL_RGBA; //GL_RGBA;
    GLuint format = GL_RGBA;
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);
}

void init_insect_mob_shader()
{


}

const float _va[2*2*4] =
{
	0,1, 1,0,
	1,0, 0,-1,
	0,-1, -1,0,
	-1,0, 0,1 
};

const float _ta[2*2*4] =
{
.5,1, 1,.5,
1,.5, 0.5,0,
0.5,0, 0,.5,
0,.5 ,0.5,1.0
};

const float _va2[2*2*4] =
{
	-1,1, 1,1,
	1,1, 1,-1,
	1,-1, -1,-1, 
	-1,-1, -1,1 
};


class InsectMob
{
	public:

	float x,y,z;


	InsectMob()
	{


	}

	void draw_leg(float _x, float _y, float _z)
	{


		const float h = 0.8;
		const float l = 0.2;

	    for(int i=0; i<4; i++)
	   	{
			glTexCoord2f(0.5,0.5/2 + 0.5);
    		glVertex3f(_x, _y, _z-h);

			glTexCoord2f(_ta[4*i+0],  _ta[4*i+1]/2 + 0.5);
    		glVertex3f(_x+l*_va2[4*i+0], _y+l*_va2[4*i+1], _z);

			glTexCoord2f(_ta[4*i+2],  _ta[4*i+3]/2 + 0.5);
    		glVertex3f(_x+l*_va2[4*i+2], _y+l*_va2[4*i+3], _z);
	   	}

	}


	void draw()
	{
		//printf("draw\n");

	    glColor3ub(255,255,255);

	    glEnable(GL_TEXTURE_2D);
	    glBindTexture( GL_TEXTURE_2D, insect_mob_texture );

	    const int sides = 3;

	    const float z0 = 0;
	    const float z1 = 0.3;
	    const float z2 = -0.3;

	    const float w = 1.0;
	    const float h = 1.0;

	    glBegin( GL_TRIANGLES );

	    const float f = (1 / (float)(sides))*2*3.14159;

	    for(int i=0; i<sides; i++)
	   	{
			glTexCoord2f(0.5,0.5);
    		glVertex3f(x, y, z1+z);

			glTexCoord2f(sin(f*i)/2 + 0.5,  cos(f*i)/2 + 0.5 );
    		glVertex3f(x+w*sin(f*i), y+h*cos(f*i), z0+z);

			glTexCoord2f(sin(f*(i+1))/2 + 0.5,  cos(f*(i+1))/2 + 0.5);
    		glVertex3f(x+w*sin(f*(i+1)), y+h*cos(f*(i+1)), z0+z);
	   	}

	    for(int i=0; i<sides; i++)
	   	{
			glTexCoord2f(0.5,0.5);
    		glVertex3f(x, y, z2+z);

			glTexCoord2f(sin(f*i)/2 + 0.5,  cos(f*i)/2 + 0.5 );
    		glVertex3f(x+w*sin(f*i), y+h*cos(f*i), z0+z);

			glTexCoord2f(sin(f*(i+1))/2 + 0.5,  cos(f*(i+1))/2 + 0.5);
    		glVertex3f(x+w*sin(f*(i+1)), y+h*cos(f*(i+1)), z0+z);
	   	}

	   	draw_leg(x,y,z+3);

	    glEnd();

	}

};

/*
	    const float z0 = 0;
	    const float z1 = 0.3;

	    const float l = 1.2;

	    glBegin( GL_TRIANGLES );

	    for(int i=0; i<4; i++)
	   	{
			glTexCoord2f(0.5,0.5/2);
    		glVertex3f(x, y, z1+z);

			glTexCoord2f(_ta[4*i+0],  _ta[4*i+1]/2);
    		glVertex3f(x+_va[4*i+0], y+l*_va[4*i+1], z0+z);

			glTexCoord2f(_ta[4*i+2],  _ta[4*i+3]/2);
    		glVertex3f(x+_va[4*i+2], y+l*_va[4*i+3], z0+z);
	   	}

	    for(int i=0; i<4; i++)
	   	{
			glTexCoord2f(0.5,0.5/2 + 0.5);
    		glVertex3f(x, y, z-z1);

			glTexCoord2f(_ta[4*i+0],  _ta[4*i+1]/2 + 0.5);
    		glVertex3f(x+_va[4*i+0], y+l*_va[4*i+1], z0+z);

			glTexCoord2f(_ta[4*i+2],  _ta[4*i+3]/2 + 0.5);
    		glVertex3f(x+_va[4*i+2], y+l*_va[4*i+3], z0+z);
	   	}

	   	glEnd();
*/

InsectMob alpha;

void draw_insect()
{

	alpha.draw();
}

void set_insect_position(float x, float y, float z)
{
	printf("set position: %f, %f, %f \n", x,y,z);
	z += 3.0;
	alpha.x = x;
	alpha.y = y;
	alpha.z = z;
}

}