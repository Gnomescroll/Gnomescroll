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

    explicit Item(int id)
    : id(id)
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

class ItemList: public Object_list<Item, ITEM_LIST_MAX>
{
    private:
        const char* name() { return "Item"; }
    public:
        ItemList() { print_list((char*)this->name(), this); }

        Item* create_type(int item_type)
        {
            Item* item = this->create();
            if (item == NULL) return NULL;
            item->type = item_type;
            return item;
        }
        
        Item* create_type(int item_type, int item_id)
        {
            Item* item = this->create(item_id);
            if (item == NULL) return NULL;
            item->type = item_type;
            return item;
        }
        
        Item* get_or_create_type(int item_type, int item_id)
        {
            Item* item = this->get_or_create(item_id);
            if (item == NULL) return NULL;
            item->type = item_type;
            return item;
        }

        void draw() {}
        void tick() {}
};

}
