#include "compass.hpp"

namespace Compass {

static char file[] = "media/texture/compass.png";

static const float scale = 0.5f;
static const int x_margin = 10;
static const int y_margin = 10;

static int width;
static int height;
static float x;
static float y;
static float theta = 0.5f; // rotation

static SDL_Surface* surface;
static GLuint texture;

void init() {

    surface = create_surface_from_file(file);

    if (surface == NULL) {
        printf("Compass surface init failed\n");
        return;
    }

    width = surface->w;
    height = surface->h;
    x = _xresf - ((float)width * scale) - x_margin;
    y = _yresf - ((float)height * scale) - y_margin;

    Uint32 tex_format = GL_BGRA;
    if (surface->format->Rmask == 0x000000ff)
        tex_format = GL_RGBA;

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D( GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, tex_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);

}

// rotate compass texture
void update() {
    theta = ClientState::playerAgent_state.camera_state.theta;
}

void draw() {

    if (surface == NULL) return;

    static const float z = -0.5f;

    glColor4ub(255,255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindTexture(GL_TEXTURE_2D, texture);
    draw_bound_texture_rotated(x,y, ((float)width)*scale, ((float)height)*scale, z, theta);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}


//cython
void draw_compass() {
    update();
    draw();
}

}
