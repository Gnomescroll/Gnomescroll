#include "texture_loader.h"

int particle_texture_id;
int hud_block_selector_tex_id;

int get_particle_texture() {
    //printf("3: !!! particle sheet id= %i \n", particle_texture_id);
    return particle_texture_id;
}

int get_hud_block_selector_texture() {
    return hud_block_selector_tex_id;
}

int init_tex_particle_sheet() {

    //printf("load_particle_sheet\n");

    SDL_Surface *surface;

    surface=IMG_Load("./media/texture/particles_01.png"); ///does this need to be freed?

    if(!surface) {
        printf("_init_particle_functions, IMG_Load: %s \n", IMG_GetError());
        return 0;
    }
    if(surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}


    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, (GLuint*)&particle_texture_id );

    glBindTexture( GL_TEXTURE_2D, particle_texture_id );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int texture_format;
    if (surface->format->Rmask == 0x000000ff) texture_format = GL_RGBA; else texture_format = GL_BGRA;

    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    return 0;
}

int init_tex_hud_block_sheet() {

    printf("init tex hud block sheet\n");

    SDL_Surface *surface;

    surface=IMG_Load("./media/texture/hud_block_selector.png"); ///does this need to be freed?

    if(!surface) {
        printf("init_hud_tex_block_sheet, IMG_Load: %s \n", IMG_GetError());
        return 0;
    }
    if(surface->format->BytesPerPixel != 4) {printf("IMG_Load: image is missing alpha channel \n"); return 0;}

    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, (GLuint*)&hud_block_selector_tex_id );

    printf("tex id=%i\n", hud_block_selector_tex_id);

    glBindTexture( GL_TEXTURE_2D, hud_block_selector_tex_id );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ); ///tweak?
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ); ///tweak?

    /*
        Use mipmapps or something

    */
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    int texture_format;
    if (surface->format->Rmask == 0x000000ff) texture_format = GL_RGBA; else texture_format = GL_BGRA;

    glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels );
    glDisable(GL_TEXTURE_2D);
    return 0;
}

int init_texture_loader(){
    init_tex_particle_sheet();
    init_tex_hud_block_sheet();
    return 0;
}
