#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace ItemContainer
{

/*
 * Simulate an ItemContainer for rendering/predictive purpose
 */

class ItemContainerUIInterface
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
        int* slot_durability;
        
        bool is_full()
        {
            assert(this->slot_count <= this->slot_max && this->slot_count >= 0);
            return (this->slot_count >= this->slot_max);
        }

        bool is_valid_slot(int slot)
        {
            return (slot >= 0 && slot < this->slot_max);
        }

        int get_slot_durability(int slot)
        {
            assert(this->is_valid_slot(slot));
            return this->slot_durability[slot];
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

        void insert_item(int slot, int item_type, int stack_size, int durability)
        {
            assert(item_type != NULL_ITEM_TYPE);
            assert(stack_size > 0);
            assert(this->is_valid_slot(slot));
            this->slot_type[slot] = item_type;
            this->slot_stack[slot] = stack_size;
            this->slot_durability[slot] = durability;
            this->slot_count++;
        }

        void remove_item(int slot)
        {
            assert(this->is_valid_slot(slot));
            this->slot_type[slot] = NULL_ITEM_TYPE;
            this->slot_stack[slot] = 1;
            this->slot_durability[slot] = NULL_DURABILITY;
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
                    this->slot_durability[i] = NULL_DURABILITY;
                }
                else
                {
                    this->slot_type[i] = Item::get_item_type(slots[i]);
                    this->slot_stack[i] = Item::get_stack_size(slots[i]);
                    this->slot_durability[i] = Item::get_item_durability(slots[i]);
                }
            }
        }

        virtual bool can_insert_item(int slot, int item_type) = 0;
        virtual int get_stackable_slot(int item_type, int stack_size) = 0;
        virtual int get_empty_slot() = 0;

        virtual void init(ItemContainerType type, int xdim, int ydim) = 0;

        virtual ~ItemContainerUIInterface()
        {
           if (this->slot_type != NULL) delete[] this->slot_type;
           if (this->slot_stack != NULL) delete[] this->slot_stack;
           if (this->slot_durability != NULL) delete[] this->slot_durability;
        }

        explicit ItemContainerUIInterface(int id)
        : id(id), type(CONTAINER_TYPE_NONE),
        xdim(0), ydim(0),
        slot_max(0), slot_count(0),
        slot_type(NULL), slot_stack(NULL), slot_durability(NULL)
        {}
};

class ItemContainerUI: public ItemContainerUIInterface
{
    public:

        bool can_insert_item(int slot, int item_type)
        {
            return true;
        }

        int get_stackable_slot(int item_type, int stack_size)
        {
            for (int i=0; i<this->slot_max; i++)
            {
                if (this->slot_type[i] == NULL_ITEM_TYPE) continue;
                if (this->slot_type[i] == item_type   // stacks
                && (Item::get_max_stack_size(this->slot_type[i]) - this->slot_stack[i]) >= stack_size) // stack will fit
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

        void init(ItemContainerType type, int xdim, int ydim)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            assert(this->slot_max < NULL_SLOT);
            this->slot_type = new int[this->slot_max];
            this->slot_stack = new int[this->slot_max];
            this->slot_durability = new int[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot_type[i++] = NULL_ITEM_TYPE);
            for (int i=0; i<this->slot_max; this->slot_stack[i++] = 1);
            for (int i=0; i<this->slot_max; this->slot_durability[i++] = NULL_DURABILITY);
        }

        explicit ItemContainerUI(int id)
        : ItemContainerUIInterface(id)
        {}
};

class ItemContainerNaniteUI: public ItemContainerUIInterface
{
    public:

        int get_food_type()
        {
            return this->get_slot_type(0);
        }
        
        int get_food_stack()
        {
            return this->get_slot_stack(0);
        }

        int get_coin_type()
        {
            return this->get_slot_type(this->slot_max-1);
        }
        
        int get_coin_stack()
        {
            return this->get_slot_stack(this->slot_max-1);
        }

        bool can_insert_item(int slot, int item_type)
        {
            assert(this->is_valid_slot(slot));
            if (slot == 0)
            {   // check against nanite's food
                return Item::get_nanite_edibility(item_type);
            }
            else if (slot == this->slot_max-1)
            {   // nanite coins only
                if (item_type == Item::get_item_type((char*)"nanite_coin")) return true;
                return false;
            }
            return false;   // no other slots accept insertions
        }

        int get_stackable_slot(int item_type, int stack_size)
        {
            // check food slot
            if (this->slot_type[0] == item_type
            && (Item::get_max_stack_size(this->slot_type[0]) - this->slot_stack[0]) >= stack_size)
                return 0;
            // check coin slot
            if (this->slot_type[this->slot_max-1] == item_type
            && (Item::get_max_stack_size(this->slot_type[this->slot_max-1]) - this->slot_stack[this->slot_max-1]) >= stack_size)
                return this->slot_max-1;
            return NULL_SLOT;
        }

        int get_empty_slot()
        {
            // only food slot can be empty slot
            if (this->slot_type[0] == NULL_ITEM_TYPE) return 0;
            return NULL_SLOT;
        }

        void init(ItemContainerType type, int xdim, int ydim)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim + 1; // +1 for the food slot
            assert(this->slot_max < NULL_SLOT);
            this->slot_type = new int[this->slot_max];
            this->slot_stack = new int[this->slot_max];
            this->slot_durability = new int[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot_type[i++] = NULL_ITEM_TYPE);
            for (int i=0; i<this->slot_max; this->slot_stack[i++] = 1);
            for (int i=0; i<this->slot_max; this->slot_durability[i++] = NULL_DURABILITY);
        }

        explicit ItemContainerNaniteUI(int id)
        : ItemContainerUIInterface(id)
        {}
};

} // Item
