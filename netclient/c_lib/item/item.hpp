#pragma once

namespace Item
{

class Item
{
    public:

    int id;
    int type; //stack, laser, mining_laser
    int item_id;
    
    int sprite_index; //temp

    int durability;
    int stack_size;
    //int sprite_index;

    Item()
    {
        type = 0;
        item_id = 0;
        durability = -1;
        stack_size = -1;
        sprite_index = rand()%32;
    }

};

}

#include <common/template/object_list.hpp>

namespace Item
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
