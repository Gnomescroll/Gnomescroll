#include "render.hpp"

#if DC_CLIENT

int get_icon_spritesheet_id(Object_types type)
{
    switch (type)
    {
        case OBJ_TYPE_GRENADE_REFILL:
            return 1;
        default:
            return 0;
    }
}

void draw_inventory_icon(Object_types type, int slot)
{   // lookup icon index from type, draw at slot position
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

void InventoryIconDrawList::draw()
{
    // glBindTexture(this->texture);
    InventoryProperties* obj;
    for (int i=0; i<this->max; i++)
    {
        obj = this->objects[i];
        draw_inventory_icon(obj->type, obj->slot);
    }
}

namespace InventoryRender
{

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

}

#endif
