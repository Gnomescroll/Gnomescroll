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
}
if(image->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

return image;

}

int _create_texture(SDL_Surface* surface) {
    GLuint texture;

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?
    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    return texture;
}

///Deprecate
int _create_hud_texture(char *file) {

    SDL_Surface* surface;
    GLuint texture;

    surface = _load_image(file);

    if(surface == NULL) {
        printf("Error: _SDL/texture_loader.c _create_hud_texture, surface is null!\n");
    }

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

int _create_hud_texture2(SDL_Surface *surface) {
    if(surface == NULL) {
        printf("Error: _SDL/texture_loader.c create_hud_texture2, surface is null!\n");
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
