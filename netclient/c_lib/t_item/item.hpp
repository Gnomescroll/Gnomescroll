#pragma once

namespace t_item
{

class Item
{



};

}

#include <c_lib/template/object_list.hpp>

namespace t_item
{

const int ITEM_LIST_MAX = 1024;

class ItemList: public Object_list<ItemContainer, ITEM_LIST_MAX>
{
    private:
        const char* name() { return "Item"; }
    public:
        ItemList() { print(); }

        void draw() {}
        void tick() {}
};

}