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
        int alt_xdim;
        int alt_ydim;

        int slot_max;
        int slot_count;

        int* slot_type;
        int* slot_stack;
        int* slot_durability;
        
        bool is_full()
        {
            GS_ASSERT(this->slot_count <= this->slot_max && this->slot_count >= 0);
            return (this->slot_count >= this->slot_max);
        }

        bool is_valid_slot(int slot)
        {
            GS_ASSERT(this->slot_max > 0);
            return (slot != NULL_SLOT && slot >= 0 && slot < this->slot_max);
        }

        int get_slot_durability(int slot)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return NULL_DURABILITY;
            return this->slot_durability[slot];
        }

        int get_slot_stack(int slot)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return 1;
            return this->slot_stack[slot];
        }

        int get_slot_type(int slot)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return NULL_ITEM_TYPE;
            return this->slot_type[slot];
        }

        void insert_item(int slot, int item_type, int stack_size, int durability)
        {
            GS_ASSERT(item_type != NULL_ITEM_TYPE);
            GS_ASSERT(stack_size > 0);
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return;
            this->slot_type[slot] = item_type;
            this->slot_stack[slot] = stack_size;
            this->slot_durability[slot] = durability;
            this->slot_count++;
        }

        void remove_item(int slot)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return;
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

        virtual bool can_insert_item(int slot, int item_type)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_type == NULL_ITEM_TYPE) return false;

            return true;
        }

        virtual int get_empty_slot()
        {
            for (int i=0; i<this->slot_max; i++)
                if (this->slot_type[i] == NULL_ITEM_TYPE)
                    return i;
            return NULL_SLOT;
        }

        virtual void init(ItemContainerType type, int xdim, int ydim)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            GS_ASSERT(this->slot_max < NULL_SLOT);
            this->slot_type = new int[this->slot_max];
            this->slot_stack = new int[this->slot_max];
            this->slot_durability = new int[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot_type[i++] = NULL_ITEM_TYPE);
            for (int i=0; i<this->slot_max; this->slot_stack[i++] = 1);
            for (int i=0; i<this->slot_max; this->slot_durability[i++] = NULL_DURABILITY);
        }

        void set_alt_parameters(int xdim, int ydim)
        {
            this->alt_xdim = xdim;
            this->alt_ydim = ydim;
        }

        virtual ~ItemContainerUIInterface()
        {
           if (this->slot_type != NULL) delete[] this->slot_type;
           if (this->slot_stack != NULL) delete[] this->slot_stack;
           if (this->slot_durability != NULL) delete[] this->slot_durability;
        }

        explicit ItemContainerUIInterface(int id)
        : id(id), type(CONTAINER_TYPE_NONE),
        xdim(0), ydim(0),
        alt_xdim(0), alt_ydim(0),
        slot_max(0), slot_count(0),
        slot_type(NULL), slot_stack(NULL), slot_durability(NULL)
        {}
};

class ItemContainerUI: public ItemContainerUIInterface
{
    public:

        explicit ItemContainerUI(int id)
        : ItemContainerUIInterface(id)
        {}
};

class ItemContainerHandUI: public ItemContainerUIInterface
{
    public:

        int get_item_durability()
        {
            return ItemContainerUIInterface::get_slot_durability(0);
        }

        int get_item_stack()
        {
            return ItemContainerUIInterface::get_slot_stack(0);
        }

        int get_item_type()
        {
            return ItemContainerUIInterface::get_slot_type(0);
        }

        void insert_item(int item_type, int stack_size, int durability)
        {
            ItemContainerUIInterface::insert_item(0, item_type, stack_size, durability);
        }

        void remove_item()
        {
            ItemContainerUIInterface::remove_item(0);
        }

        void init(ItemContainerType type, int xdim, int ydim)
        {
            GS_ASSERT(type == AGENT_HAND);
            ItemContainerUIInterface::init(type, xdim, ydim);
        }

        explicit ItemContainerHandUI(int id)
        : ItemContainerUIInterface(id)
        {}
};

class ItemContainerEnergyTanksUI: public ItemContainerUIInterface
{
    public:

        int energy_tank_type;

        bool can_insert_item(int slot, int item_type)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_type == NULL_ITEM_TYPE) return false;
            
            return (item_type == this->energy_tank_type);
        }

        void init(ItemContainerType type, int xdim, int ydim)
        {
            this->energy_tank_type = Item::get_item_type("energy_tank");
            GS_ASSERT(this->energy_tank_type != NULL_ITEM_TYPE);

            GS_ASSERT(type == AGENT_ENERGY_TANKS);
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            GS_ASSERT(this->slot_max < NULL_SLOT);
            
            this->slot_type = new int[this->slot_max];
            this->slot_stack = new int[this->slot_max];
            this->slot_durability = new int[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot_type[i++] = NULL_ITEM_TYPE);
            for (int i=0; i<this->slot_max; this->slot_stack[i++] = 1);
            for (int i=0; i<this->slot_max; this->slot_durability[i++] = NULL_DURABILITY);
        }

        explicit ItemContainerEnergyTanksUI(int id)
        : ItemContainerUIInterface(id), energy_tank_type(NULL_ITEM_TYPE)
        {}
};

class ItemContainerSynthesizerUI: public ItemContainerUIInterface
{
    public:

        int get_coin_type()
        {
            return this->get_slot_type(0);
        }
        
        int get_coin_stack()
        {
            return this->get_slot_stack(0);
        }

        bool can_insert_item(int slot, int item_type)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_type == NULL_ITEM_TYPE) return false;
            
            // synthesizer coins only
            static int coin_type = Item::get_item_type("synthesizer_coin");
            GS_ASSERT(coin_type != NULL_ITEM_TYPE);
            return (item_type == coin_type);
        }

        explicit ItemContainerSynthesizerUI(int id)
        : ItemContainerUIInterface(id)
        {}
};

class ItemContainerSmelterUI: public ItemContainerUIInterface
{
    public:

        float fuel;
        int fuel_type;
        float progress;

        void tick_fuel();
        void tick_progress();

        bool is_smelter_output(int slot)
        {
            if (slot >= this->slot_max-(this->alt_xdim*this->alt_ydim)) return true;
            return false;
        }

        int get_fuel()
        {
            GS_ASSERT(this->slot_max > 0);
            if (this->slot_max <= 0) return NULL_ITEM_TYPE;
            return this->slot_type[0];
        }

        bool can_insert_item(int slot, int item_type)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_type == NULL_ITEM_TYPE) return false;

            if (slot == 0)
                return Item::is_fuel(item_type);
            else if (this->is_smelter_output(slot))
                return false;
            return true;
        }

        int get_empty_slot()
        {
            for (int i=1; i<this->slot_max; i++)    // skip fuel slot
            {
                if (this->is_smelter_output(i)) continue;
                if (this->slot_type[i] == NULL_ITEM_TYPE)
                    return i;
            }
            return NULL_SLOT;
        }

        void init(ItemContainerType type, int xdim, int ydim)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim + this->alt_xdim*this->alt_ydim + 1; // +1 for fuel
            GS_ASSERT(xdim*ydim > 0);
            GS_ASSERT(this->alt_xdim * this->alt_ydim > 0);
            GS_ASSERT(this->slot_max > 0);
            GS_ASSERT(this->slot_max < NULL_SLOT);
            if (this->slot_max <= 0) return;
            this->slot_type = new int[this->slot_max];
            this->slot_stack = new int[this->slot_max];
            this->slot_durability = new int[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot_type[i++] = NULL_ITEM_TYPE);
            for (int i=0; i<this->slot_max; this->slot_stack[i++] = 1);
            for (int i=0; i<this->slot_max; this->slot_durability[i++] = NULL_DURABILITY);
        }

    explicit ItemContainerSmelterUI(int id)
    : ItemContainerUIInterface(id),
    fuel(0.0f), fuel_type(NULL_ITEM_TYPE), progress(0.0f)
    {}
};

class ItemContainerCrusherUI: public ItemContainerUIInterface
{
    public:

        static const int input_slot = 0;

        int get_input_slot_type()
        {
            GS_ASSERT(this->is_valid_slot(this->input_slot));
            if (!this->is_valid_slot(this->input_slot)) return false;
            return this->slot_type[this->input_slot];
        }

        bool can_insert_item(int slot, int item_type)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_type == NULL_ITEM_TYPE) return false;

            if (slot == 0) return true;
            return false;
        }

        int get_empty_slot()
        {
            if (this->slot_max <= 0) return NULL_SLOT;
            if (this->slot_type[0] == NULL_ITEM_TYPE) return 0;
            return NULL_SLOT;
        }

        void init(ItemContainerType type, int xdim, int ydim)
        {
            this->type = type;
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim + this->alt_xdim * this->alt_ydim;
            GS_ASSERT(this->slot_max < NULL_SLOT);
            this->slot_type = new int[this->slot_max];
            this->slot_stack = new int[this->slot_max];
            this->slot_durability = new int[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot_type[i++] = NULL_ITEM_TYPE);
            for (int i=0; i<this->slot_max; this->slot_stack[i++] = 1);
            for (int i=0; i<this->slot_max; this->slot_durability[i++] = NULL_DURABILITY);
        }

        explicit ItemContainerCrusherUI(int id)
        : ItemContainerUIInterface(id)
        {}
};

} // Item
