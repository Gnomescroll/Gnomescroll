#pragma once

#include <c_lib/entity/constants.hpp>

const int EMPTY_SLOT = 65535;
class InventorySlot
{
    public:
        int item_id;
        ObjectType item_type;
        int slot;
        int count;

        bool empty()
        {
            if (this->item_id == EMPTY_SLOT) return true;
            return false;
        }

        void load(int id, ObjectType type, int stack_size);

        #if DC_CLIENT
        // render data
        int sprite_index;
        #endif

        void print()
        {
            printf("Slot %d, item_id %d, item_type %d\n", this->slot, this->item_id, this->item_type);
        }

    InventorySlot()
    :
    item_id(EMPTY_SLOT), item_type(OBJECT_NONE),
    slot(-1),    // slot is set after allocation
    count(1)
    #if DC_CLIENT
    , sprite_index(0)
    #endif
    {
    }
};
