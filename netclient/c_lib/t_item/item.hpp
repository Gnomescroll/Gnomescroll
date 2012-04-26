#pragma once

namespace t_item
{

class Item
{
    public:

    int id;
    int item_type; //stack, laser, mining_laser
    int parent_object;
    
    int sprite_index;

    Item()
    {
        item_type = 0;
        sprite_index = rand()%32;
    }

};

}

#include <c_lib/common/template/object_list.hpp>

namespace t_item
{

const int ITEM_LIST_MAX = 1024;

class ItemList: public Object_list<ItemContainer, ITEM_LIST_MAX>
{
    private:
        const char* name() { return "Item"; }
    public:
        ItemList() { print_list((char*)this->name(), this); }

        void draw() {}
        void tick() {}
};

}
