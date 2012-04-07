#pragma once

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

        void draw();
        
        InventoryIconDrawList()
        : PropertyList<InventoryProperties>(ICON_DRAW_LIST_MAX)
        { print(); }
};

void register_inventory_item_draw_list(InventoryProperties* property);
void unregister_inventory_item_draw_list(InventoryProperties* property);
