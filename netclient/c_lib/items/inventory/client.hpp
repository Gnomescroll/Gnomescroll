#pragma once

#if DC_CLIENT

#include <c_lib/t_hud/constants.hpp>
#include <c_lib/items/inventory/constants.hpp>
#include <c_lib/items/inventory/inventory.hpp>
#include <c_lib/items/constants.hpp>

class Inventory: public BaseInventory
{
    public:

    InventorySlot* get_slot_item(int slot)
    {
        if (!this->is_valid_slot(slot)) return NULL;
        return &this->objects[slot];
    }

    void remove_action(int slot)
    {
        bool can_remove = this->can_remove(slot);
        if (!can_remove) return;
        remove_item_from_inventory_CtoS msg;
        msg.inventory_id = this->id;
        msg.slot = slot;
        msg.send();
    }

    void add_action(int id, ObjectType type)
    {
        int slot = this->get_empty_slot();
        this->add_action(id, type, slot);
    }
    
    void add_action(int id, ObjectType type, int slot)
    {
        bool can_add = this->can_add(type, slot);
        if (!can_add) return;
        add_item_to_inventory_CtoS msg;
        msg.inventory_id = this->id;
        msg.id = id;
        msg.type = type;
        msg.slot = slot;
        msg.send();
    }

    bool swap_action(int slota, int slotb)
    {
        bool swapped = this->can_swap(slota, slotb);
        if (!swapped) return false;
        swap_item_in_inventory_CtoS msg;
        msg.inventory_id = this->id;
        msg.slota = slota;
        msg.slotb = slotb;
        msg.send();
        return swapped;
    }

    explicit Inventory(int id)
    : BaseInventory(id)
    {
    }
};

#endif
