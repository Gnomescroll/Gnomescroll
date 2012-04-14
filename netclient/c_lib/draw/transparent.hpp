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
    int sprites_wide;
};

struct SpriteData
{
    int index;
    float x,y,z;
    float w,h;
};

typedef enum {
    ITEM_DRAW_LIST,
    CUBE_DRAW_LIST1,
    CUBE_DRAW_LIST2,
    NONE_DRAW_LIST
} DrawListType;

// draw lists
const int SPRITE_LIST_MAX = 1024;
class HudSpriteList: public BehaviourList
{
    public:
        // add spritesheet metadata here, like width,height
        int texture_id;
        struct SpriteData sprite_data;
        struct TextureData texture_data;
        DrawListType type;
        
        void draw();

    HudSpriteList()
    : BehaviourList(SPRITE_LIST_MAX),texture_id(-1), type(NONE_DRAW_LIST)
    {}
};

typedef PropertyListAggregate<HudSpriteList> DrawListAggregate;
extern DrawListAggregate* draw_lists;
// TODO: migrate inventory properties to standard property format

void init_spritesheets();
void teardown_spritesheets();
//void draw();
}   // Draw
