#pragma once

// TODO:

/*  Keep adding various interfaces here (t_mech, t_map, agent etc)
 *  When time is right, create server/ folder and split interfaces
 *
 */

#if DC_SERVER

#include <c_lib/items/inventory/base.hpp>

class InventoryProperties: public BaseInventoryProperties
{
    public:

        void load(int id, Object_types type)
        {
            this->item_id = id;
            this->item_type = type;
            printf("Loaded inventory item %d,%d\n", id,type);
        }
        
    InventoryProperties()
    :
    BaseInventoryProperties()
    {
    }
};

class InventoryContents: public BaseInventoryContents<InventoryProperties>
{
    public:
        void sendToClient(int inventory_id, int client_id);
};

typedef BaseInventory<InventoryContents> BaseInventoryServer;

class Inventory: public BaseInventoryServer
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

    bool swap_action(int slota, int slotb)
    {
        bool swapped = this->swap(slota, slotb);
        if (this->get_owner() != NO_AGENT)
        {
            this->sendToClientAdd(this->contents.objects[slota].item_id, this->contents.objects[slota].item_type, slota);
            this->sendToClientAdd(this->contents.objects[slotb].item_id, this->contents.objects[slotb].item_type, slotb);
        }
        else
        {
            this->broadcastAdd(this->contents.objects[slota].item_id, this->contents.objects[slota].item_type, slota);
            this->broadcastAdd(this->contents.objects[slotb].item_id, this->contents.objects[slotb].item_type, slotb);
        }
        return swapped;
    }

    /* Network API */
    void sendContentsToClient(int client_id)
    {
        this->contents.sendToClient(this->_state.id, client_id);
    }

    /* Expose Inventory API here */
    void init(int x, int y)
    {
        BaseInventoryServer::init(x,y);
    }
    
    bool can_add(Object_types type)
    {
        return BaseInventoryServer::can_add(type);
    }
    
    void sendToClientCreate(int client_id);
    void broadcastCreate();
    void sendToClientState(int client_id);
    void broadcastState();
    void broadcastDeath();
    void sendToClientAdd(int id, Object_types type, int slot);
    void broadcastAdd(int id, Object_types type, int slot);
    void sendToClientRemove(int slot);
    void broadcastRemove(int slot);

    
    explicit Inventory(int id)
    : BaseInventoryServer(id) {}
};

#endif
