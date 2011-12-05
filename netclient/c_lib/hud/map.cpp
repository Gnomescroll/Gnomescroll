#include "map.hpp"

#include <c_lib/t_map/t_map.hpp>

namespace Map {

static const int width = 512;
static const int height = 512;

static const int num_cells = width*height;
static char cells[num_cells];

static SDL_Surface* surface;
static GLuint texture;

void PutPixel32_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
Uint8 * pixel = (Uint8*)surface->pixels;
pixel += (y * surface->pitch) + (x * sizeof(Uint32));
*((Uint32*)pixel) = color;
}

void init_surface() {
    // taken from http://sdl.beuc.net/sdl.wiki/SDL_CreateRGBSurface
    
    /* Create a 32-bit surface with the bytes of each pixel in R,G,B,A order,
       as expected by OpenGL for textures */
    Uint32 rmask, gmask, bmask, amask;

    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
    
    //surface = SDL_CreateRGBSurface(SDL_SWSURFACE,width,height,32,0,0,0,0);

    if(surface == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
        return;
    }


    SDL_LockSurface(surface);
    Uint32 pix = SDL_MapRGB(surface->format, 0x00, 0x00, 0xff);
    for(int i=0; i<16; i++) {
    for(int j=0; j<16; j++) {
        PutPixel32_nolock(surface, i,j, pix);
    }
    }
    SDL_UnlockSurface(surface);

    //surface = IMG_Load("media/texture/blocks_01.png");
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    glTexImage2D( GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
/*
    glTexImage2D(
        GL_TEXTURE_2D,  //target
        0,  // level
        GL_BGRA,  // internalformat
        surface->w, surface->h, //width, height
        0,  // border
        GL_BGRA, // format
        GL_UNSIGNED_BYTE,   // type
        surface->pixels // pixels
    );
*/
    glDisable(GL_TEXTURE_2D);
}

void init() {
    init_surface();
}


void update_surface() {
    SDL_LockSurface(surface);
    
    int i,j;
    Uint32 pix;
    for (i=0; i<num_cells; i++) {
        pix = SDL_MapRGB(surface->format, cells[i], cells[i], cells[i]);
        ((Uint32*)surface->pixels)[i] = pix;
    }
    

    //pix = SDL_MapRGB(surface->format, 0xff, 0x00, 0x00);
    //PutPixel32_nolock(surface, 5,5, pix);

    SDL_UnlockSurface(surface);
}

void update_texture() {
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);

    glDisable(GL_TEXTURE_2D);
}


void update_heightmap() {
    int i,j;
    int h;
    for (i=0; i < xmax; i++) {
        for (j=0; j < ymax; j++) {
            h = get_height_at(i,j);
            cells[i + xmax*j] = (char)h;
        }
    }
}


void update() {
    update_heightmap();
    update_surface();
    update_texture();
}

static int update_counter = 0;

void draw() {
    //static int updated =0;
    //if (!updated) {

    update_counter++;
    if(update_counter % 30 == 0) update();
        //updated++;
    //}
    //printf("x=%i, y=%i, pitch=%i \n", surface->w, surface->h, surface->pitch);
    //glColor3f(0.0f,1.0f,0.0f);
    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);



/*
    float _h = 150.0;
    float x_off = 50.0;
    float y_off = 50.0;

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x_off, y_off+_h, z);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x_off+_h,  y_off+_h, z);


    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x_off+_h,  y_off, z);

    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x_off,  y_off, z);
*/
    glBegin(GL_QUADS);
        static const float z = -0.5f;

        const int x = 50;
        const int y = 512+50;
        const int x_size = 512;
        const int y_size = 512;

        glTexCoord2f(0.0,0.0);
        glVertex3f(x, y, z);  // Top left

        glTexCoord2f(1.0,0.0);
        glVertex3f(x+x_size, y, z);  // Top right

        glTexCoord2f(1.0,1.0);
        glVertex3i(x+x_size, y-y_size, z);  // Bottom right

        glTexCoord2f(0.0,1.0);
        glVertex3i(x, y-y_size, z);  // Bottom left

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

// for cython
void update_map() {update();}
void draw_map() {draw();}

}
