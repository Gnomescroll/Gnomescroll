#include "map.hpp"

#include <c_lib/t_map/t_map.hpp>
#include <c_lib/state/client_state.hpp>

namespace Map {

// pull these from loaded map dimensions (not available yet)
static const int width = 128;
static const int height = 128;

static const int num_cells = width*height;
static unsigned char cells[num_cells];

static SDL_Surface* surface;
static GLuint texture;

static SDL_Surface* gradient_surface;

void init_surface() {
    // taken from http://sdl.beuc.net/sdl.wiki/SDL_CreateRGBSurface
    printf("init: hud_map \n");

    Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    const int grad_num = 4;
    const char grad_fmt[] = "media/texture/heightmap_gradient_%02d.png";
    char grad_str[strlen(grad_fmt) -2 +1];
    sprintf(grad_str, grad_fmt, grad_num);
    gradient_surface =IMG_Load(grad_str);

    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
    
    if(surface == NULL) {
        fprintf(stderr, "CreateRGBSurface failed: %s\n", SDL_GetError());
        return;
    }

    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    //GL_BGRA
    glTexImage2D( GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
}

void init() {
    init_surface();
}

Uint32 get_agent_pixel(int *x, int *y) {
    *x = (int)ClientState::playerAgent_state.camera_state.x;
    *y = (int)ClientState::playerAgent_state.camera_state.y;

    static const Uint8
        a(255),
        r(255),
        g(10),
        b(10);
    return SDL_MapRGBA(surface->format, b,g,r,a); // bgra, red
}

void update_surface() {
    SDL_LockSurface(surface);
    
    int i;
    Uint32 pix;
    Uint8 r,g,b,a;
    for (i=0; i<num_cells; i++) {
        pix = ((Uint32*)gradient_surface->pixels)[cells[i]];
        SDL_GetRGBA(pix, gradient_surface->format, &r, &g, &b, &a);
        ((Uint32*)surface->pixels)[i] = SDL_MapRGBA(surface->format, r,g,b,a);
    }

    // set agent pixel
    int x=0,*_x=&x;
    int y=0,*_y=&y;
    pix = get_agent_pixel(_x,_y);
    if (x >= 0 && x < width && y >= 0 && y < height)    // only draw in bounds (or could segfault)
        ((Uint32*)surface->pixels)[x + width*y] = pix;
    
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
    for (i=0; i < width; i++) {
        for (j=0; j < height; j++) {
            h = get_height_at(i,j);
            cells[i + width*j] = (unsigned char)2*h;
        }
    }
}

void update() {
    update_heightmap();
    update_surface();
    update_texture();
}

void draw() {
    static unsigned int update_counter = 0;

    update_counter++;
    if(update_counter % 30 == 0)
        update();

    glColor3ub(255,255,255);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    
    static const float z = -0.5f;
    static const int x = 50;
    static const int y = 512+50;
    static const int x_size = 512;
    static const int y_size = 512;

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
