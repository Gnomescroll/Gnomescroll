#pragma once

#include <c_lib/objects/common/interface/policy.hpp>
#include <c_lib/objects/common/interface/layers.hpp>

#include <c_lib/items/inventory/packets.hpp>
#include <c_lib/items/inventory/contents.hpp>

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

class Inventory;

// wrap InventoryContents into a game object
template <class InventoryContents>
class BaseInventory: public InventoryObjectInterface
{
    protected:
        InventoryContents contents;
        
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

        bool swap(int slota, int slotb)
        {
            bool swapped = this->contents.swap(slota, slotb);
            return swapped;
        }

    public:

    explicit BaseInventory<InventoryContents>(int id)
    {
        this->_state.id = id;
        this->owned_properties.owner = NO_AGENT;
    }

    ~BaseInventory<InventoryContents>()
    {
    }

};
