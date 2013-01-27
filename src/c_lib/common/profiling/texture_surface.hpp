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
    private:
    int xdim;
    int ydim;

    struct PIXEL* pixels;

    bool needs_update;

    unsigned int tex[2]; //texture ids
    int tex_index;

    public:
    float scale;

    float screen_width()
    {
        return this->xdim * this->scale;
    }
    float screen_height()
    {
        return this->ydim * this->scale;
    }

    Texture_surface(int x, int y)
    :
    scale(4.0f)
    {
        xdim = x;
        ydim = y;

        //x/y must be a power of two

        tex_index = 0;
        needs_update = false;

        //surface = create_surface_from_nothing(x, y);

        //pixels = (struct PIXEL*) calloc(x*y, sizeof(struct PIXEL));
        pixels = (struct PIXEL*) malloc(x*y*sizeof(struct PIXEL));

        for(int i = 0; i < x*y; i++) pixels[i].color = 0;


        for(int i = 0; i < xdim; i++)
        {
            set_pixel(i,0, 255, 0,0,150);
            set_pixel(i,ydim-1, 255, 0,0,150);
        }

        for(int i = 0; i < ydim; i++)
        {
            set_pixel(0, i, 255, 0,0,150);
            set_pixel(xdim-1,i, 255, 0,0,150);
        }
    /*
        for(int i = 0; i < x*y; i++)
        {
            //this->set_pixel(i, 14, 254,0,0,120);
            pixels[i].r = 254;

        }
    */

        glEnable(GL_TEXTURE_2D);
        glGenTextures( 2, (GLuint*) &tex );

        glBindTexture( GL_TEXTURE_2D, tex[0] );

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xdim, ydim, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels );

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
        glDeleteTextures( 2, (GLuint*) &tex );
    }


    void draw(int x, int y)
    {
        glColor3ub(255,255,255);

        glEnable(GL_TEXTURE_2D);

        glEnable(GL_BLEND);

        //glBlendFunc (GL_SRC_ALPHA, GL_ONE);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindTexture(GL_TEXTURE_2D, tex[0]);

        const float z = -0.4f;

        float x0 = x;
        float x1 = x+scale*xdim;
        float y0 = y;
        float y1 = y+scale*ydim;

        glBegin(GL_QUADS);

        glTexCoord2f(0.0f,0.0f);
        glVertex3f(x0,y0, z);  // Top left

        glTexCoord2f(1.0f,0.0f);
        glVertex3f(x1, y0, z);  // Top right

        glTexCoord2f(1.0f,1.0f);
        glVertex3f(x1, y1, z);  // Bottom right

        glTexCoord2f(0.0f,1.0f);
        glVertex3f(x0, y1, z);  // Bottom left

        glEnd();


        if(needs_update == true) update();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);

    }

    void update()
    {
        glBindTexture(GL_TEXTURE_2D, tex[0] );    //A texture you have already created with glTexImage2D
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, xdim, ydim, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

        needs_update = false;
    }

    void clear()
    {
        for(int i = 0; i < xdim*ydim; i++) pixels[i].color = 0;
        needs_update = true;
    }

    void set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        needs_update = true;

        struct PIXEL p;

        p.r = r;
        p.g = g;
        p.b = b;
        p.a = a;

        pixels[y*xdim + x].color = p.color;
    }
};
