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
    protected:
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
