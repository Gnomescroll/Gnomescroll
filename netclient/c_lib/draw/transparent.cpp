#include "transparent.hpp"

namespace Draw
{

// globals/constants
const int n_spritesheets = 3;
char* paths[n_spritesheets] = {
    (char*)"./media/sprites/i00.png",
    (char*)"./media/sprites/t00.png",
    (char*)"./media/sprites/t01.png"
};
GLuint textures[n_spritesheets];

DrawListAggregate* draw_lists;

// texture name -> id map
int get_texture_id(char* path)
{
    for (int i=0; i<n_spritesheets; i++)
        if (strcmp(path, paths[i]) == 0)
            return i;
    return -1;
}

void load_texture(char* path, int index)
{
    SDL_Surface* s = create_surface_from_file(path);
    if (s == NULL)
    {
        printf("Draw::load_texture(): spitesheet load error for %s\n", path);
        return;
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &textures[index]);
    glBindTexture(GL_TEXTURE_2D, textures[index]);

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    GLuint internalFormat = GL_RGBA; //GL_RGBA;
    GLuint format = GL_RGBA;
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels);
    glDisable(GL_TEXTURE_2D);

    SDL_FreeSurface(s);
}

void init_draw_lists()
{
    draw_lists = new DrawListAggregate;
    draw_lists->init(n_spritesheets);
    for (int i=0; i<n_spritesheets; i++)
        draw_lists->get(i)->texture_id = i;


}

void SpriteList::draw()
{
    if (this->ct == 0) return;
    glBindTexture(GL_TEXTURE_2D, textures[this->texture_id]);
    //InventoryRender::inventory_draw_list->draw();
    SpriteProperties* obj;
    for (int i=0; i<this->max; i++)
    {
        obj = (SpriteProperties*)this->objects[i];
        if (obj == NULL) continue;
        draw_sprite(obj->sprite_index, obj->scale);
    }
}

void init_textures()
{
    for (int i=0; i<n_spritesheets; i++)
        load_texture(paths[i], i);
}

void init()
{
    init_textures();
    init_draw_lists();
}

void teardown_draw_lists()
{
    if (draw_lists != NULL)
        delete[] draw_lists;
}

void teardown_textures()
{
}

void teardown()
{
    teardown_draw_lists();
    teardown_textures();
}

void draw()
{
    glColor4ub(255,255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i=0; i<n_spritesheets; i++)
        draw_lists->get(i)->draw();
    glDisable(GL_TEXTURE_2D);   // do i need to disable before rebinding a new texture?
    glDisable(GL_BLEND);
}


}   // Draw
