#include "transparent.hpp"

#include <c_lib/common/common.hpp>

namespace Draw
{

// globals/constants
const int n_spritesheets = 3;
char* paths[n_spritesheets] = {
    (char*)"./media/sprites/i00.png",
    (char*)"./media/sprites/t00.png",
    (char*)"./media/sprites/t01.png"
};
int render_list_sizes[n_spritesheets] = {
    512,
    128,
    128
};
GLuint textures[n_spritesheets];

struct TextureData texture_data[n_spritesheets] = {
    { 1.0f/8.0f, 1.0f/8.0f },
    { 1.0f/4.0f, 1.0f/4.0f },
    { 1.0f/7.0f, 1.0f/14.0f},
};

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
    {
        draw_lists->get(i)->texture_id = i;
        draw_lists->get(i)->init(render_list_sizes[i]);
        draw_lists->get(i)->texture_data = texture_data[i];
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
        delete draw_lists;
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

void draw_sprite(struct TextureData texture, struct SpriteData sprite, float scale)
{
    const float x = sprite.x;
    const float y = sprite.y;
    const float z = sprite.z;

    const float w = (1.0f/texture.sprite_width) * scale;
    const float h = (1.0f/texture.sprite_height) * scale;

    const float sx = (sprite.index % 8) * texture.sprite_width;
    const float sy = (sprite.index / 8) * texture.sprite_height;
    const float sw = texture.sprite_width;
    const float sh = texture.sprite_height;
    draw_bound_texture_sprite2(x,y,w,h,z, sx,sy,sw,sh);
}


void SpriteList::draw()
{
    if (this->ct == 0) return;
    glBindTexture(GL_TEXTURE_2D, textures[this->texture_id]);
    SpriteProperties* obj;
    for (int i=0; i<this->max; i++)
    {
        obj = (SpriteProperties*)this->objects[i];
        if (obj == NULL) continue;
        obj->get_sprite_data(&this->sprite_data);
        draw_sprite(this->texture_data, this->sprite_data, obj->scale);
    }
}

}   // Draw
