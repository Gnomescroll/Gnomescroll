#pragma once


namespace t_item
{


typedef short ItemId ;

const int EMPTY_SLOT = 0xFFFF;

class ItemContainer // dont use behaviour list unless doing the registration model
{
    public:

    	int id;

        int xdim;
        int ydim;

        int slot_max;
        int container_type;

        ItemId* slot;

        ItemContainer(int _xdim, int _ydim)
        {
        	xdim = _xdim;
        	ydim = _ydim;

        	slot_max = xdim*ydim;

        	slot = new ItemId[slot_max];
        	for(int i=0; i<slot_max; i++) slot[i] = EMPTY_SLOT;
        }

	    ~ItemContainer()
	    {
	       delete[] slot;
	    }

        bool is_full()
        {
            for(int i=0; i<slot_max; i++) { if(slot[i] == EMPTY_SLOT) return true; }
            return false;
        }

        bool is_valid_grid_position(int x, int y)
        {
            return (x < 0 || x >= xdim || y < 0 || y >= ydim) ? false : true;
        }

        int get_empty_slot()
        {
            for(int i=0; i<slot_max; i++) { if(slot[i] == EMPTY_SLOT) return i; }
            return -1;
        }

        void insert_item(int item_id, int x, int y)
        {
        	assert(is_valid_grid_position(x,y));
        	slot[y*xdim + x] = item_id;
            printf("inserted item %d at %d,%d in inventory %d \n", item_id, x, y, id);
        }

        bool clear_slot(int x, int y)
        {
        	assert(is_valid_grid_position(x,y));
            slot[y*xdim + x] = EMPTY_SLOT;
        }
/*
        void sendToClient(int inventory_id, int client_id);
*/
};


}

#include <c_lib/template/object_list.hpp>

namespace t_item
{

const int ITEM_CONTAINER_MAX = 1024;

class ItemContainerList: public Object_list<ItemContainer, FREE_ITEM_MAX>
{
    private:
        const char* name() { return "ItemContainer"; }
    public:
        ItemContainerList() { print(); }

        //void draw() {}
        //void tick() {}
};

}

#if 0

// wrap InventoryContents into a game object
class Inventory: public InventoryObjectInterface
{
    private:
        InventoryContents contents;
    public:

        // TODO -- move, this is owned specialization
        void attach_to_owner();
        
        void init(int x, int y)
        {
            this->contents.init(x,y);
        }

        bool type_allowed(Object_types type)
        {   // Restrict types here
            if (type == OBJ_TYPE_AGENT
              || type == OBJ_TYPE_SLIME
              || type == OBJ_TYPE_NONE)
                return false;
            return true;
        }

        bool dimension_mismatch(int x, int y)
        {
            if (this->contents.x != x || this->contents.y != y)
                return true;
            return false;
        }

        void sendContentsToClient(int client_id)
        {
            this->contents.sendToClient(this->_state.id, client_id);
        }

        bool can_add(Object_types type)
        {
            if (!this->type_allowed(type))
                return false;
            return this->contents.can_add();
        }

        bool can_add(Object_types type, int slot)
        {
            if (!this->type_allowed(type))
                return false;
            return this->contents.can_add(slot);
        }

        void sendToClientAdd(int id, Object_types type, int slot);
        void broadcastAdd(int id, Object_types type, int slot);
        void sendToClientRemove(int slot);
        void broadcastRemove(int slot);

        bool add(int id, Object_types type)
        {
            int slot = this->contents.get_empty_slot();
            if (slot < 0) return false;
            return this->add(id, type, slot);
        }

        bool add(int id, Object_types type, int slot)
        {
            bool added = this->contents.add(id,type,slot);
            return added;
        }

        bool remove(int slot)
        {
            bool removed = this->contents.remove(slot);
            return removed;
        }

        #if DC_SERVER
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
        #endif

        #if DC_CLIENT
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
        #endif

        /* Network API */
        void sendToClientCreate(int client_id);
        void broadcastCreate();
        void sendToClientState(int client_id);
        void broadcastState();
        void broadcastDeath();

    explicit Inventory(int id)
    {
        this->_state.id = id;
        this->owned_properties.owner = NO_AGENT;
    }

    ~Inventory()
    {
    }

};

#endif