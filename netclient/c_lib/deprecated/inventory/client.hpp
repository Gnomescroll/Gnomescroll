#pragma once

#if DC_CLIENT

#include <inventory/inventory.hpp>

class Inventory: public BaseInventory
{
    public:

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

        void remove_action(int slot)
        {
            bool can_remove = this->can_remove(slot);
            if (!can_remove) return;
            remove_item_from_inventory_CtoS msg;
            msg.inventory_id = this->id;
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

        bool merge_stack_action(int slota, int slotb, int count)
        {
            bool merged = this->can_merge_stack(slota, slotb, count);
            if (!merged) return false;
            merge_stack_in_inventory_CtoS msg;
            msg.inventory_id = this->id;
            msg.slota = slota;
            msg.slotb = slotb;
            msg.count = count;
            msg.send();
            return merged;
        }

    explicit Inventory(int id)
    : BaseInventory(id)
    {}
    
};

#endif
