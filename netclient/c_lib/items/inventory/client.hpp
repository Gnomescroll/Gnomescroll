#pragma once

// TODO:

/*  Keep adding various interfaces here (t_mech, t_map, agent etc)
 *  When time is right, create client/ folder and split interfaces
 *
 */

#if DC_CLIENT

#include <c_lib/items/inventory/base.hpp>

class ClientInventory: public BaseInventory
{
    public:

    void attach_to_owner();

    void remove_any_action()
    {
        for (int i=0; i<this->contents.max; i++)
        {
            if (this->contents.objects[i].item_id == EMPTY_SLOT)
                continue;
            this->remove_action(i);
            break;
        }
    }

    void remove_action(int slot)
    {
        bool can_remove = this->contents.can_remove(slot);
        if (!can_remove) return;
        remove_item_from_inventory_CtoS msg;
        msg.inventory_id = this->_state.id;
        msg.slot = slot;
        msg.send();
    }

    void add_action(int id, Object_types type)
    {
        int slot = this->contents.get_empty_slot();
        this->add_action(id, type, slot);
    }
    
    void add_action(int id, Object_types type, int slot)
    {
        bool can_add = this->can_add(type, slot);
        if (!can_add) return;
        add_item_to_inventory_CtoS msg;
        msg.inventory_id = this->_state.id;
        msg.id = id;
        msg.type = type;
        msg.slot = slot;
        msg.send();
    }

    explicit ClientInventory(int id)
    : BaseInventory(id) {}
};

#endif
