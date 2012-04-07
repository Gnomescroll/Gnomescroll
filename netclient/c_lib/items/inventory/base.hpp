#pragma once

#include <c_lib/objects/common/interface/policy.hpp>
#include <c_lib/objects/common/interface/layers.hpp>
#include <c_lib/objects/common/list/list.hpp>
#include <c_lib/objects/components/sprite/sprite.hpp>

//#include <c_lib/items/inventory/render.hpp>
#include <c_lib/items/inventory/packets.hpp>
#include <c_lib/items/inventory/contents.hpp>
#if DC_CLIENT
#include <c_lib/draw/transparent.hpp>
#endif

typedef OwnedState InventoryState;

class InventoryObjectInterface: public InventoryState
{
    public:
        void tick() {}
        void draw() {}
        void update() {}
        void born() {}
        void die() {}

    ~InventoryObjectInterface() {}
    InventoryObjectInterface() {}
};

// wrap InventoryContents into a game object
class BaseInventory: public InventoryObjectInterface
{
    private:
        InventoryContents contents;
    public:

        #if DC_CLIENT
        struct {
            float x,y,z;
        } screen;
        #endif

        // TODO -- move, this is owned specialization
        void attach_to_owner();
        
        void init(int x, int y)
        {
            this->contents.init(this, x,y);
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

    explicit BaseInventory(int id);

    ~BaseInventory()
    {
    }

};
