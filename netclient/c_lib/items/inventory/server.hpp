#pragma once

// TODO:

/*  Keep adding various interfaces here (t_mech, t_map, agent etc)
 *  When time is right, create server/ folder and split interfaces
 *
 */

#if DC_SERVER

#include <c_lib/items/inventory/base.hpp>

class ServerInventory: public BaseInventory
{
    public:

    void remove_all_action()
    {
        for (int i=0; i<this->contents.max; i++)
        {
            if (this->contents.objects[i].item_id == EMPTY_SLOT)
                continue;
            this->remove_action(i);
        }
    }

    void remove_action(int slot)
    {
        bool removed = this->remove(slot);
        if (!removed) return;
        if (this->get_owner() != NO_AGENT)
            this->sendToClientRemove(slot);
        else
            this->broadcastRemove(slot);
    }

    bool add_action(int id, Object_types type, int slot)
    {
        bool added = this->add(id,type,slot);
        if (!added) return false;
        if (this->get_owner() != NO_AGENT)
            this->sendToClientAdd(id, type, slot);
        else
            this->broadcastAdd(id, type, slot);
        return added;
    }

    bool add_action(int id, Object_types type)
    {
        int slot = this->contents.get_empty_slot();
        bool added = this->add_action(id,type, slot);
        if (!added) return false;
        if (this->get_owner() != NO_AGENT)
            this->sendToClientAdd(id, type, slot);
        else
            this->broadcastAdd(id, type, slot);
        return added;
    }
    
    explicit ServerInventory(int id)
    : BaseInventory(id) {}
};

#endif
