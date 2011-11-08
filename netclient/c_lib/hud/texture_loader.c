#include "./texture_loader.h"

int init_image_loader() {
    IMG_Init(IMG_INIT_PNG); // IMG_INIT_JPG|IMG_INIT_PNG
    return 0;
}

SDL_Surface* _load_image(char *file) {

    SDL_Surface *image;
    image=IMG_Load(file); //
    if(!image) {
        printf("IMG_Load: %s \n", IMG_GetError());
        // handle error
        return 0;
    }
    if(image->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

    return image;
}

int create_texture_from_surface(SDL_Surface *surface, int *tex) {
    if(surface == NULL) {
        printf("Error: texture_loader.c create_texture_from_surface, surface is null!\n");
        return 1;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, (GLuint*)tex );
    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, *tex );
    // Set the texture's stretching properties
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels ); //2nd parameter is level
    glDisable(GL_TEXTURE_2D);
    return 0;
}

int create_texture_from_file(char* filename, int* tex) {
    SDL_Surface *surface;
    surface=IMG_Load(filename); //does this need to be freed?
    if (!surface) {
        printf("Error loading texture %s, %s \n", filename, IMG_GetError());
        return 1;
    }
    if (surface->format->BytesPerPixel != 4) {
        printf("IMG_Load: image is missing alpha channel \n");
        return 2;
    }
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, (GLuint*)tex );
    glBindTexture( GL_TEXTURE_2D, *tex );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    int texture_format;
    if (surface->format->Rmask == 0x000000ff) {
        texture_format = GL_RGBA;
    } else {
        texture_format = GL_BGRA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    return 0;
}

/* COPIED FROM block_selector.cpp */
// this is only for the reticle
static Texture hud_texture;
void _load_texture(char *file) {
    SDL_Surface *surface;
    int tex = 0;

    surface = _load_image(file);
    create_texture_from_surface(surface, &tex);

    hud_texture.tex = tex;
    hud_texture.w = surface->w;
    hud_texture.h = surface->h;
}

void _draw_loaded_texture(int x, int y) {
    int x1, y1;

    x = x - hud_texture.w/2;
    y = y - hud_texture.h/2;

    x1 = x + hud_texture.w;
    y1 = y + hud_texture.h;

    _blit_sprite(hud_texture.tex, x, y, x1, y1, 0);
}
