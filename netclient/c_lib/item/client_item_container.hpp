#pragma once

#if DC_CLIENT

namespace Item
{

/*
 * Simulate an ItemContainer for rendering/predictive purpose
 */

class ItemContainerUI
{
    public:
        int id;
        ItemContainerType type;

        int xdim;
        int ydim;

        int slot_max;
        int slot_count;

        int* slot_type;
        int* slot_stack;

        bool is_full()
        {
            assert(this->slot_count <= this->slot_max && this->slot_count >= 0);
            return (this->slot_count >= this->slot_max);
        }

        bool is_valid_slot(int slot)
        {
            return (slot >= 0 && slot < this->slot_max);
        }

        int get_stackable_slot(int item_type, int stack_size)
        {
            for (int i=0; i<this->slot_max; i++)
            {
                if (this->slot_type[i] == NULL_ITEM_TYPE) continue;
                if (this->slot_type[i] == item_type   // stacks
                && (get_stack_max(this->slot_type[i]) - this->slot_stack[i]) >= stack_size) // stack will fit
                    return i;
            }
            return NULL_SLOT;
        }

        int get_empty_slot()
        {
            for (int i=0; i<this->slot_max; i++)
                if (this->slot_type[i] == NULL_ITEM_TYPE)
                    return i;
            return NULL_SLOT;
        }

        int get_slot_stack(int slot)
        {
            assert(this->is_valid_slot(slot));
            return this->slot_stack[slot];
        }

        int get_slot_type(int slot)
        {
            assert(this->is_valid_slot(slot));
            return this->slot_type[slot];
        }

        void insert_item(int slot, int item_type, int stack_size)
        {
            assert(item_type != NULL_ITEM_TYPE);
            assert(stack_size > 0);
            assert(this->is_valid_slot(slot));
            this->slot_type[slot] = item_type;
            this->slot_stack[slot] = stack_size;
            this->slot_count++;
        }

        void remove_item(int slot)
        {
            assert(this->is_valid_slot(slot));
            this->slot_type[slot] = NULL_ITEM_TYPE;
            this->slot_stack[slot] = 1;
            this->slot_count--;
        }

        /* initializers */

        void load_data(ItemID* slots)
        {
            for (int i=0; i<this->slot_max; i++)
            {
                if (slots[i] == NULL_ITEM)
                {
                    this->slot_type[i] = NULL_ITEM_TYPE;
                    this->slot_stack[i] = 1;
                }
                else
                {
                    this->slot_type[i] = get_item_type(slots[i]);
                    this->slot_stack[i] = get_stack_size(slots[i]);
                }
            }
        }

        void init(ItemContainerType type, int xdim, int ydim)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            this->slot_type = new int[this->slot_max];
            this->slot_stack = new int[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot_type[i++] = NULL_ITEM_TYPE);
            for (int i=0; i<this->slot_max; this->slot_stack[i++] = 1);
        }

        ~ItemContainerUI()
        {
           if (this->slot_type != NULL) delete[] this->slot_type;
           if (this->slot_stack != NULL) delete[] this->slot_stack;
        }

        ItemContainerUI(int id)
        : id(id), type(CONTAINER_TYPE_NONE),
        xdim(0), ydim(0),
        slot_max(0), slot_count(0), slot_type(NULL), slot_stack(NULL)
        {}
};


} // Item

#endif
