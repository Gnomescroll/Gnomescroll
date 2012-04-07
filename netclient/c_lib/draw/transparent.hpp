#pragma once

#include <c_lib/objects/common/list/list.hpp>

namespace Draw
{

// texture name -> id map
int get_texture_id(char* path);

// load textures

struct TextureData
{
    float sprite_width;
    float sprite_height;
};

struct SpriteData
{
    int index;
    float x,y,z;
};

// draw lists
class SpriteList: public BehaviourList
{
    public:
        // add spritesheet metadata here, like width,height
        int texture_id;
        struct SpriteData sprite_data;
        struct TextureData texture_data;
        
        void draw();

    SpriteList()
    : texture_id(-1)
    {}
};

typedef PropertyListAggregate<SpriteList> DrawListAggregate;
extern DrawListAggregate* draw_lists;
// TODO: migrate inventory properties to standard property format

void init();
void teardown();
void draw();
}   // Draw
