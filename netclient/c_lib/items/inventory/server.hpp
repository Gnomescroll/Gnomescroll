#pragma once

// TODO:

/*  Keep adding various interfaces here (t_mech, t_map, agent etc)
 *  When time is right, create server/ folder and split interfaces
 *
 */

#if DC_SERVER

#include <c_lib/items/inventory/inventory.hpp>

class Inventory: public BaseInventory
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

    InventorySlot* get_slot_item(int slot)
    {
        if (!this->contents.is_valid_slot(slot)) return NULL;
        return &this->contents.objects[slot];
    }

    InventorySlot* get(int slot)
    {
        return this->contents.get(slot);
    }

    // no broadcast
    bool remove_silent(int slot)
    {
        return this->remove(slot);
    }

    bool remove_action(int slot)
    {
        bool removed = this->remove(slot);
        if (!removed) return false;
        if (owner != NO_AGENT)
            this->sendToClientRemove(slot);
        else
            this->broadcastRemove(slot);
        return true;
    }

    // no broadcast
    bool add_silent(int id, ObjectType type, int stack_size, int slot)
    {
        return this->add(id, type, slot);
    }

    bool add_action(int id, ObjectType type, int stack_size, int slot)
    {
        bool added = this->add(id, type, stack_size, slot);
        if (!added) return false;
        if (owner != NO_AGENT)
            this->sendToClientAdd(id, type, stack_size, slot);
        else
            this->broadcastAdd(id, type, stack_size, slot);
        return added;
    }

    bool add_action(int id, ObjectType type, int stack_size)
    {
        int slot = this->contents.get_empty_slot();
        bool added = this->add_action(id, type, stack_size, slot);
        if (!added) return false;
        if (owner != NO_AGENT)
            this->sendToClientAdd(id, type, stack_size, slot);
        else
            this->broadcastAdd(id, type, stack_size, slot);
        return added;
    }

    bool swap_action(int slota, int slotb)
    {
        bool swapped = this->swap(slota, slotb);
        if (owner != NO_AGENT)
            this->sendToClientSwap(slota, slotb);
        else
            this->broadcastSwap(slota, slotb);
        return swapped;
    }

    /* Network API */
    void sendContentsToClient(int client_id)
    {
        this->contents.sendToClient(this->id, client_id);
    }

    void init(int x, int y)
    {
        BaseInventory::init(x,y);
    }
    
    void sendToClientCreate(int client_id);
    void broadcastCreate();
    void sendToClientState(int client_id);
    void broadcastState();
    void broadcastDeath();
    void sendToClientAdd(int id, ObjectType type, int stack_size, int slot);
    void broadcastAdd(int id, ObjectType type, int stack_size, int slot);
    void sendToClientRemove(int slot);
    void broadcastRemove(int slot);
    void sendToClientSwap(int slota, int slotb);
    void broadcastSwap(int slota, int slotb);

    explicit Inventory(int id)
    : BaseInventory(id)
    {}
};

#endif
