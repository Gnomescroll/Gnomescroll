#pragma once

#include <c_lib/objects/common/interface/policy.hpp>
#include <c_lib/objects/common/interface/interfaces.hpp>
#include <c_lib/items/inventory/packets.hpp>
#include <c_lib/items/inventory/contents.hpp>

class InventoryObjectInterface: public ObjectStateLayer
{
    public:

        OwnedComponent owned;
    
        void tick() {}
        void update() {}
        void born() {}
        void die() {}

    ~InventoryObjectInterface() {}
    InventoryObjectInterface()
    : ObjectStateLayer(Objects::create_packet_none, Objects::state_packet_none, &owned, Objects::team_none, Objects::health_none, Objects::spatial_none)
    {}
};

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

        bool type_allowed(ObjectType type)
        {   // Restrict types here
            if (type == OBJECT_AGENT
              || type == OBJECT_SLIME
              || type == OBJECT_NONE)
                return false;
            return true;
        }

        bool dimension_mismatch(int x, int y)
        {
            if (this->contents.x != x || this->contents.y != y)
                return true;
            return false;
        }

        bool can_add(ObjectType type)
        {
            if (!this->type_allowed(type))
                return false;
            return this->contents.can_add();
        }

        bool can_add(ObjectType type, int slot)
        {
            if (!this->type_allowed(type))
                return false;
            return this->contents.can_add(slot);
        }

        bool can_remove(int slot)
        {
            return this->contents.can_remove(slot);
        }

        bool can_swap(int slota, int slotb)
        {
            return this->contents.can_swap(slota, slotb);
        }

        bool add(int id, ObjectType type, int stack_size)
        {
            int slot = this->contents.get_empty_slot();
            if (slot < 0) return false;
            return this->add(id, type, stack_size, slot);
        }

        bool add(int id, ObjectType type, int stack_size, int slot)
        {
            bool added = this->contents.add(id, type, stack_size, slot);
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

        int width()
        {
            return this->contents.x;
        }

        int height ()
        {
            return this->contents.y;
        }

    explicit BaseInventory<InventoryContents>(int id)
    {
        this->_state.id = id;
        this->owned.properties.owner = NO_AGENT;
    }

    ~BaseInventory<InventoryContents>()
    {
    }

};
