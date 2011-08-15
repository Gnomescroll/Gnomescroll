
#include "block_selector.h"

typedef struct {
    int tex;
    int w;
    int h;
} Texture;

int _init_block_selector() {
    return 0;
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

int _blit_sprite2(int texture, float x0, float y0, float x1, float y1, float z) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, texture );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glColor3ub(255, 255, 255);

glBegin( GL_QUADS );
    glTexCoord2i( 0, 0 );
    glVertex3f( x0, y0, z );

    glTexCoord2i( 1, 0 );
    glVertex3f( x1, y0, z );

    glTexCoord2i( 1, 1 );
    glVertex3f( x1, y1, z );

    glTexCoord2i( 0, 1 );
    glVertex3f( x0, y1, z );
glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
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
