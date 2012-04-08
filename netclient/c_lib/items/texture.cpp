#include "texture.hpp"

#include <c_lib/SDL/texture_loader.hpp>

GLuint item_texture;

void begin_item_draw()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    glBindTexture(GL_TEXTURE_2D, item_texture);

    glBegin(GL_QUADS);
}
void end_item_draw()
{
    glEnd();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
}


void init_item_texture()
{
    const char* item_spritesheet_filename = "./media/sprites/i01_256.png";
    create_texture_from_file((char*)item_spritesheet_filename, &item_texture);
}

void teardown_item_texture()
{
}

