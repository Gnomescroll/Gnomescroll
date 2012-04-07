#include "render.hpp"

#if DC_CLIENT

#include <c_lib/SDL/draw_functions.hpp>

void InventoryIconDrawList::draw()
{
    // glBindTexture(this->texture);
    if (this->ct <= 0) return;
    InventoryProperties* obj;
    for (int i=0; i<this->max; i++)
    {
        obj = this->objects[i];
        if (obj == NULL) continue;
        if (obj->id == EMPTY_SLOT) continue;
        draw_inventory_icon(obj->type, obj->slot);
    }
}

namespace InventoryRender
{
const float sprite_pixel_size = 16.0f;
const int sprites_wide = 8; // number of sprites in width
const int sprites_high = 8;
const float sprite_width = 1.0f / ((float)sprites_wide);
const float sprite_height = 1.0f / ((float)sprites_high);

InventoryIconDrawList* inventory_draw_list = NULL;

void init_draw_list()
{
    inventory_draw_list = new InventoryIconDrawList;
    inventory_draw_list->texture = &ItemSheetTexture;
}

void teardown_draw_list()
{
    if (inventory_draw_list != NULL)
        delete inventory_draw_list;
}

/* Textures */

SDL_Surface* ItemSheetSurface = NULL;
GLuint ItemSheetTexture = 0;

void init_surface()
{
    SDL_Surface* s = create_surface_from_file((char*) "media/sprites/i00.png");
    if(s == NULL)
    {
        printf("inventory_draw_init(): spitesheet load error\n");
        return;
    }
 
    glEnable(GL_TEXTURE_2D);
    glGenTextures( 1, &ItemSheetTexture );
    glBindTexture( GL_TEXTURE_2D, ItemSheetTexture  );

    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    GLuint internalFormat = GL_RGBA; //GL_RGBA;
    GLuint format = GL_RGBA;
 
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, s->w, s->h, 0, format, GL_UNSIGNED_BYTE, s->pixels );
    glDisable(GL_TEXTURE_2D);

    ItemSheetSurface = s;
}

void teardown_surface()
{
    glDeleteTextures(1, &ItemSheetTexture );
    SDL_FreeSurface(ItemSheetSurface);
}

void init()
{
    init_surface();
    init_draw_list();
}

void teardown()
{
    teardown_draw_list();
    teardown_surface();
}

void draw()
{
    glColor4ub(255,255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, InventoryRender::ItemSheetTexture);
    InventoryRender::inventory_draw_list->draw();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

}   // InventoryRender


int get_icon_spritesheet_id(Object_types type)
{
    const int ERROR_SPRITE = 0;
    switch (type)
    {
        case OBJ_TYPE_DIRT:
            return 3;
        case OBJ_TYPE_STONE:
            return 4;

        case OBJ_TYPE_SPAWNER:
            return 6;
        case OBJ_TYPE_TURRET:
            return 7;
        
        default:
            return ERROR_SPRITE;
    }
}


void draw_inventory_icon(Object_types type, int slot)
{   // lookup icon index from type, draw at slot position
//printf("drawing icon %d at %d\n", type, slot);
    // get sprite index position
    int id = get_icon_spritesheet_id(type);
    float sx = (id % InventoryRender::sprites_wide) * InventoryRender::sprite_width;
    float sy = (id / InventoryRender::sprites_wide) * InventoryRender::sprite_height;

    // get xy from slot
    //const float ix = 300; // screen pixels start
    const float ix = _xresf/3; // screen pixels start
    const float iy = _yresf - 128.0f;
    //const float iy = _yresf/2;
    
    //float x = ix + (InventoryRender::sprite_width * slot * 2);
    float x = ix + (64 * slot);
    float y = iy;
    
    // blit sprite from spritesheet
    const float z = -0.1f;
    const float w = 16.0f * 4;
    const float h = 16.0f * 4;

    const float sw = InventoryRender::sprite_width;
    const float sh = InventoryRender::sprite_height;
    draw_bound_texture_sprite2(x,y,w,h,z, sx,sy,sw,sh);
}

void register_inventory_item_draw_list(InventoryProperties* property)
{
    #if DC_CLIENT
     // map id,type to
    //int spritesheet = get_icon_spritesheet_id(property->type);
    //if (spritesheet < 0) return;
    InventoryRender::inventory_draw_list->register_object(property);
    //ClientState::draw_lists[spritesheet]->register_object(property);
    #endif
}

void unregister_inventory_item_draw_list(InventoryProperties* property)
{
    #if DC_CLIENT
     //map id,type to
    //int spritesheet = get_icon_spritesheet_id(property->type);
    //if (spritesheet < 0) return;
    InventoryRender::inventory_draw_list->unregister_object(property);
    //ClientState::draw_lists[spritesheet]->unregister_object(property);
    #endif
}
#endif


