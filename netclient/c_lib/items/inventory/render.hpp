#pragma once

#if DC_CLIENT

#include <c_lib/objects/common/list/list.hpp>


//forward decl
class InventoryProperties;

int get_icon_spritesheet_id(Object_types type);
void draw_inventory_icon(Object_types type, int slot);

const int ICON_DRAW_LIST_MAX = 512;
class InventoryIconDrawList: public PropertyList<InventoryProperties>
{
    private:
        const char* name() { return "InventoryIconDrawList"; };
    public:
        int spritesheet_id;
        GLuint* texture; // TODO -- move, look up texture from spritesheet id

        void draw();
        void init(int n){}
        
        InventoryIconDrawList()
        : PropertyList<InventoryProperties>(ICON_DRAW_LIST_MAX)
        { print(); }
};

void register_inventory_item_draw_list(InventoryProperties* property);
void unregister_inventory_item_draw_list(InventoryProperties* property);

namespace InventoryRender
{
    extern InventoryIconDrawList* inventory_draw_list;
    extern SDL_Surface* ItemSheetSurface;
    extern GLuint ItemSheetTexture;

    void init();
    void teardown();

}   // Inventory

class DrawListAggregate: public PropertyListAggregate<InventoryIconDrawList>
{
    public:
};

#endif
