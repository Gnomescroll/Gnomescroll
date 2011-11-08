#include "./texture_loader.h"

int _init_image_loader(void) {
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

int _create_texture(SDL_Surface *surface) {
    if(surface == NULL) {
        printf("Error: texture_loader.c create_texture, surface is null!\n");
    }
    GLuint texture;

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &texture );
    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, texture );
    // Set the texture's stretching properties
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?
    // Edit the texture object's image data using the information SDL_Surface gives us
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels ); //2nd parameter is level
    glDisable(GL_TEXTURE_2D);

    return texture;
}

int _init_texture(const char* filename, int* tex) {
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
