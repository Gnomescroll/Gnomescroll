#pragma once

namespace Draw
{

// texture name -> id map
int get_texture_id(char* path);

// load textures

// draw lists
void draw_sprite(int sprite_sprite_index, float scale) {}
class SpriteList: public BehaviourList
{
    public:
        // add spritesheet metadata here, like width,height
        int texture_id;
        
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
