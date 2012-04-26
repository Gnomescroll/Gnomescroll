#pragma once

// TODO:

/*  Keep adding various interfaces here (t_mech, t_map, agent etc)
 *  When time is right, create server/ folder and split interfaces
 *
 */

#if DC_SERVER

#include <inventory/inventory.hpp>

class Inventory: public BaseInventory
{
    public:

        bool add_action(int id, ObjectType type, int stack_size)
        {
            int slot = this->get_empty_slot();
            bool added = this->add_action(id, type, stack_size, slot);
            if (!added) return false;
            if (owner != NO_AGENT)
                this->sendToClientAdd(id, type, stack_size, slot);
            else
                this->broadcastAdd(id, type, stack_size, slot);
            return added;
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

        // dumps contents
        void remove_all_action();

        bool swap_action(int slota, int slotb)
        {
            bool swapped = this->swap(slota, slotb);
            if (owner != NO_AGENT)
                this->sendToClientSwap(slota, slotb);
            else
                this->broadcastSwap(slota, slotb);
            return swapped;
        }

        bool merge_stack_action(int slota, int slotb, int count)
        {
            bool merged = this->merge_stack(slota, slotb, count);
            if (owner != NO_AGENT)
                this->sendToClientMergeStack(slota, slotb, count);
            else
                this->broadcastMergeStack(slota, slotb, count);
            return merged;
        }

        /* Network API */

        // create, delete
        void sendToClientCreate(int client_id);
        void broadcastCreate();
        void broadcastDeath();

        // contents
        void sendContentsToClient(int client_id);

        // actions/events
        void sendToClientAdd(int id, ObjectType type, int stack_size, int slot);
        void broadcastAdd(int id, ObjectType type, int stack_size, int slot);
        void sendToClientRemove(int slot);
        void broadcastRemove(int slot);
        void sendToClientSwap(int slota, int slotb);
        void broadcastSwap(int slota, int slotb);
        void sendToClientMergeStack(int slota, int slotb, int count);
        void broadcastMergeStack(int slota, int slotb, int count);

    explicit Inventory(int id)
    : BaseInventory(id)
    {}
};

#endif
