#pragma once

#include <c_lib/template/object_list.hpp>

#include <c_lib/items/inventory/inventory.hpp>

namespace Items
{

const int MAX_INVENTORY = 512;
class InventoryList: public Object_list<Inventory, MAX_INVENTORY>
{
    private:
        const char* name() { return "InventoryList"; }
    public:
    
    ~InventoryList() {}
    InventoryList() { print_list((char*)this->name(), this); }
};

}   // Items
