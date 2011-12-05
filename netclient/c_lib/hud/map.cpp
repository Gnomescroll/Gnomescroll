#include "map.hpp"

#include <c_lib/t_map/t_map.hpp>

namespace Map {

static const int width = xmax;
static const int height = ymax;

static const int num_cells = width*height;
static char cells[num_cells];

static SDL_Surface* surface;
static GLuint texture;


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

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                                   rmask, gmask, bmask, amask);
    if(surface == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
        return;
    }
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

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

    SDL_UnlockSurface(surface);
}

void update_texture() {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels);
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

void draw() {
    //static int updated =0;
    //if (!updated) {
        update();
        //updated++;
    //}
    glColor4f(1.0f,1.0f,1.0f,1.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    static const float z = 0.0f;

    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
        glVertex3f(50.0f, 50.0f, z);
    glTexCoord2f(1.0f, 0.0f);
        glVertex3f((float)width, 50.0f, z);
    glTexCoord2f(1.0f, 1.0f);
        glVertex3f((float)width, (float)height, z);
    glTexCoord2f(1.0f, 0.0f);
        glVertex3f(50.0f, (float)height, z);

    glEnd();

    glDisable(GL_TEXTURE_2D);

}

// for cython
void update_map() {update();}
void draw_map() {draw();}

}
