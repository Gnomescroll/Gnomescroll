#pragma once

#include <common/template/object_list.hpp>

#include <inventory/inventory.hpp>

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
