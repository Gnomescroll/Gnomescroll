#pragma once

#if DC_CLIENT

#include <c_lib/hud/constants.hpp>
#include <c_lib/items/inventory/inventory.hpp>
#include <c_lib/items/constants.hpp>

class Inventory: public BaseInventory
{
    public:

    int selected_slot;
    HudElementType hud;

    InventorySlot* selected_item()
    {
        if (this->selected_slot < 0) return NULL;
        return get_slot_item(this->selected_slot);
    }
    
    InventorySlot* get_slot_item(int slot)
    {
        if (!this->contents.is_valid_slot(slot)) return NULL;
        return &this->contents.objects[slot];
    }

    void select_slot(int slot)
    {
        if (slot == this->selected_slot)
        {   // unselect
            this->unselect_slot();
            return;
        }
        if (this->selected_slot > 0)
        {   // attempt swap
            this->swap_action(this->selected_slot, slot);
            this->unselect_slot();
            return;
        }

        // only select if not empty
        InventorySlot* item = this->get_slot_item(slot);
        if (item != NULL && !item->empty())
            this->selected_slot = slot;
        
    }

    void select_slot(int row, int col)
    {
        int slot = row * this->contents.x + col;
        this->select_slot(slot);
    }

    void unselect_slot()
    {
        this->selected_slot = -1;
    }
    
    bool selected()
    {
        //static int t = 0;
        //t++;
        //if (t % 5 == 0) this->selected_slot++;
        //this->selected_slot %= this->contents.max;
        //printf("slot %d\n", this->selected_slot);
        return (this->selected_slot >= 0);
    }

    void remove_selected_action()
    {
        this->remove_action(this->selected_slot);
    }

    void remove_action(int slot)
    {
        bool can_remove = this->contents.can_remove(slot);
        if (!can_remove) return;
        remove_item_from_inventory_CtoS msg;
        msg.inventory_id = this->id;
        msg.slot = slot;
        msg.send();
    }

    void add_action(int id, ObjectType type)
    {
        int slot = this->contents.get_empty_slot();
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

    void select_and_remove_action(int row, int col)
    {   // removes item at input-selected row,col
        this->unselect_slot();
        int slot = row * this->contents.x + col;
        this->remove_action(slot);
    }

    /* Expose API here */
    bool dimension_mismatch(int x, int y)
    {
        return BaseInventory::dimension_mismatch(x,y);
    }
    bool can_add(ObjectType type)
    {
        return BaseInventory::can_add(type);
    }
    bool can_add(ObjectType type, int slot)
    {
        return BaseInventory::can_add(type, slot);
    }
    bool add(int id, ObjectType type, int stack_size, int slot)
    {
        return BaseInventory::add(id, type, stack_size, slot);
    }
    bool can_remove(int slot)
    {
        return BaseInventory::can_remove(slot);
    }
    bool remove(int slot)
    {
        return BaseInventory::remove(slot);
    }
    bool swap(int slota, int slotb)
    {
        return BaseInventory::swap(slota, slotb);
    }
    void init(int x, int y)
    {
        BaseInventory::init(x,y);
    }

    explicit Inventory(int id)
    : BaseInventory(id),
    selected_slot(-1), hud(HUD_ELEMENT_NONE)
    {
    }
};

#endif
