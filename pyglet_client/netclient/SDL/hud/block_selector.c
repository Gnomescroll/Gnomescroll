
#include "block_selector.h"

/*
typedef struct {
    int tex;
    int w;
    int h;
} Texture;
*/

Texture hud_texture;
void _load_hud_texture(char *file) {
    SDL_Surface *surface;

    surface = _load_image(file);
    int tex = _create_hud_texture2(surface);

    hud_texture.tex = tex;
    hud_texture.w = surface->w;
    hud_texture.h = surface->h;
}

void _draw_loaded_hud_texture(int x, int y) {
    int x1, y1;

    x = x - hud_texture.w/2;
    y = y - hud_texture.h/2;

    x1 = x + hud_texture.w;
    y1 = y + hud_texture.h;

    _blit_sprite2(hud_texture.tex, x, y, x1, y1, 0);
}

Texture block_selector_texture;
void _load_block_selector_texture(char *file, int scale) {
    SDL_Surface *surface;

    surface = _load_image(file);
    int tex = _create_hud_texture2(surface);

    block_selector_texture.tex = tex;
    block_selector_texture.w = surface->w / scale;
    block_selector_texture.h = surface->h / scale;
}

void _draw_block_selector(int x, int y) {
    int x1, y1;
    x1 = block_selector_texture.h + x;
    y1 = block_selector_texture.w + y;
    _blit_sprite2(block_selector_texture.tex, x, y1, x1, y, 0);
}

int _create_block_texture(char *file) {
    return 0;
}

///ray casting stuff

/*
#define ssize 128
#define bsize 16384

float dummy;

int ray_cast(float x0,float y0,float z0, float x1,float y1,float z1) {
    float len = sqrt( (x0-x1)*(x0-x1) + (y0-y1)*(y0-y1) + (z0-z1)*(z0-z1) );
    int x,y,z;
    x = x0;
    y = y0;
    z = z0;

    int cx,cy,cz;
    cx = modf(x0, &dummy)*bsize;
    cy = modf(y0, &dummy)*bsize;
    cz = modf(z0, &dummy)*bsize;

    int dx,dy,dz;
    dx = (x1-x0)/len *bsize;
    dy = (y1-y0)/len *bsize;
    dz = (z1-z0)/len *bsize;

    int i;


    }
*/
