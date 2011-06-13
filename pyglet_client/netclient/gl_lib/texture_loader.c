#include "./texture_loader.h"

int _init_image_loader(void) {
IMG_Init(IMG_INIT_PNG); // IMG_INIT_JPG|IMG_INIT_PNG
return 0;
}

// SDL_Surface *IMG_Load(const char *file)

//(surface->w & (surface->w - 1)) != 0 )
//(surface->h & (surface->h - 1)) != 0 )

SDL_Surface* _load_image(char *file) {

SDL_Surface *image;
image=IMG_Load(file); //
if(!image) {
    printf("IMG_Load: %s \n", IMG_GetError());
    // handle error
    return 0;
} else {
    if(image->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}
    return image;
    }
}

int _create_texture(SDL_Surface* surface) {
    GLuint texture;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    return texture;
}

int _create_hud_texture(char *file) {

    SDL_Surface* surface;
    surface = _load_image("./texture/target.png");

    GLuint texture;

    glGenTextures( 1, &texture );
    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, texture );
    // Set the texture's stretching properties
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?
    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels ); //2nd parameter is level

    return texture;
}

int _create_block_texture(char *file) {
    return 0;
}
