#pragma once

#include <agent/constants.hpp>

#include <item/common/enum.hpp>
#include <item/common/constant.hpp>
#include <item/properties.hpp>

namespace ItemContainer
{

// init
void init_container(class ItemContainerInterface* container);

class ItemContainerInterface
{
    public:
        int id;
        ItemContainerType type;

        int xdim;
        int ydim;

        int slot_max;
        int slot_count;
        ItemID* slot;

        int owner;
        int chunk;  // TODO -- move to subclass

        bool attached_to_agent; // true for containers permanently attached to agents (inventory, synthesizer)

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

        ItemID get_item(int slot)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return NULL_ITEM;
            return this->slot[slot];
        }

        void assign_owner(int owner)
        {
            GS_ASSERT(this->attached_to_agent); // should only use this for attached containers
            this->owner = owner;
        }

        void print()
        {
            for (int i=0; i<this->slot_max; printf("%d ", this->slot[i++]));
            printf("\n");
        }

        int get_stackable_slot(int item_type, int stack_size)
        {
            for (int i=0; i<this->slot_max; i++)
            {
                if (this->slot[i] == NULL_ITEM) continue;
                if (Item::get_item_type(this->slot[i]) == item_type   // stacks
                && Item::get_stack_space(this->slot[i]) >= stack_size) // stack will fit
                    return i;
            }
            return NULL_SLOT;
        }

        #if DC_CLIENT
        void clear()
        {
            for (int i=0; i<this->slot_max; i++)
                this->slot[i] = NULL_ITEM;
        }
        #endif

        virtual void insert_item(int slot, ItemID item_id)= 0;
        virtual void remove_item(int slot) = 0;

        virtual bool can_insert_item(int slot, ItemID item_id) = 0;

        virtual int get_empty_slot() = 0;

        virtual void init(int xdim, int ydim) = 0;

        virtual bool can_be_opened_by(int agent_id)
        {
            GS_ASSERT(!this->attached_to_agent || this->owner != NO_AGENT);  // agent should be assigned
            if (this->attached_to_agent && this->owner != agent_id) return false;
            return (this->owner == agent_id || this->owner == NO_AGENT);
        }
        
        virtual bool lock(int agent_id)
        {
            ASSERT_VALID_AGENT_ID(agent_id);
            GS_ASSERT(this->can_be_opened_by(agent_id));
            if (!this->can_be_opened_by(agent_id)) return false;
            GS_ASSERT(!this->attached_to_agent);
            if (this->attached_to_agent) return false;
            this->owner = agent_id;
            return true;
        }

        virtual bool unlock(int agent_id)
        {
            ASSERT_VALID_AGENT_ID(agent_id);
            GS_ASSERT(this->owner != NO_AGENT);
            GS_ASSERT(this->owner == agent_id);
            GS_ASSERT(!this->attached_to_agent);
            if (this->attached_to_agent) return false;
            if (this->owner != agent_id) return false;
            this->owner = NO_AGENT;
            return true;
        }

        virtual ~ItemContainerInterface()
        {
           if (this->slot != NULL) delete[] this->slot;
        }

        ItemContainerInterface(ItemContainerType type, int id)
        : id(id), type(type),
        xdim(0), ydim(0),
        slot_max(0), slot_count(0), slot(NULL),
        owner(NO_AGENT), chunk(0xffff),
        attached_to_agent(false)
        {}
};

class ItemContainer: public ItemContainerInterface
{
    public:

        bool can_insert_item(int slot, ItemID item_id)
        {
            if (!this->is_valid_slot(slot)) return false;
            if (item_id == NULL_ITEM) return false;
            return true;
        }

        int get_empty_slot()
        {
            for (int i=0; i<this->slot_max; i++)
                if (this->slot[i] == NULL_ITEM)
                    return i;
            return NULL_SLOT;
        }

        void insert_item(int slot, ItemID item_id);
        void remove_item(int slot);

        /* initializers */

        void init(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            GS_ASSERT(this->slot_max > 0);
            GS_ASSERT(this->slot_max < NULL_SLOT);
            if (this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }
        
        ItemContainer(ItemContainerType type, int id)
        : ItemContainerInterface(type, id)
        {}
};

class ItemContainerCryofreezer: public ItemContainer
{
    public:

        void insert_item(int slot, ItemID item_id);
        
        ItemContainerCryofreezer(ItemContainerType type, int id)
        : ItemContainer(type,id)
        {}
};

class ItemContainerSynthesizer: public ItemContainerInterface
{
    public:
    
		int shopping_xdim;
		int shopping_ydim;

		ItemID get_coins()
		{
			return this->get_item(0);
		}

        bool can_insert_item(int slot, ItemID item_id)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_id == NULL_ITEM) return false;
            int item_type = Item::get_item_type(item_id);
            // only allow coins
            if (item_type == Item::get_item_type((char*)"synthesizer_coin")) return true;
            return false;
        }

        int get_empty_slot()
        {
            for (int i=0; i<this->slot_max; i++)
                if (this->slot[i] == NULL_ITEM)
                    return i;
            return NULL_SLOT;
        }

        void insert_item(int slot, ItemID item_id);
        void remove_item(int slot);

        /* initializers */

		void set_shopping_parameters(int shopping_xdim, int shopping_ydim)
		{
            this->shopping_xdim = shopping_xdim;
            this->shopping_ydim = shopping_ydim;
		}

        void init(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            GS_ASSERT(this->slot_max > 0);
            GS_ASSERT(this->slot_max < NULL_SLOT);
            if (this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }
        
        ItemContainerSynthesizer(ItemContainerType type, int id)
        : ItemContainerInterface(type, id),
        shopping_xdim(0), shopping_ydim(0)
        {}
};

class ItemContainerCraftingBench: public ItemContainerInterface
{
    public:

        bool can_insert_item(int slot, ItemID item_id)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_id == NULL_ITEM) return false;
            return true;
        }

        int get_empty_slot()
        {
            return NULL_SLOT;
        }

        void insert_item(int slot, ItemID item_id);
        void remove_item(int slot);

        /* initializers */

        void init(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim;
            GS_ASSERT(this->slot_max > 0);
            GS_ASSERT(this->slot_max < NULL_SLOT);
            if (this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }
        
        ItemContainerCraftingBench(ItemContainerType type, int id)
        : ItemContainerInterface(type, id)
        {}
};


class ItemContainerSmelter: public ItemContainerInterface
{
    public:

        static const int product_xdim = 1;
        static const int product_ydim = 2;
        static const int input_xdim = 2;
        static const int fuel_slot = 0;
        
        float fuel;       // 0.0f - 1.0f
        int fuel_type;  // item type
        float burn_rate;  // for item type
        
        int recipe_id;  // recipe identifier
        float progress;   // 0.0f - 1.0f
        float progress_rate;  // for recipe type
        
        bool is_output_slot(int slot)
        {   // output slot is if xslot == xdim-1;
            int xslot = slot % (this->xdim);
            return (xslot == this->xdim-1);
        }

        bool is_fuel_slot(int slot)
        {
            return (slot == this->fuel_slot);
        }

        ItemID get_fuel()
        {
            GS_ASSERT(this->slot_max > 0);
            if (this->slot_max <= 0) return NULL_ITEM;
            return this->slot[this->fuel_slot];
        }

        #if DC_SERVER
        void burn_fuel();
        void reset_fuel();
        void fill_fuel(int fuel_type);
        void begin_smelting(int recipe_id);
        void tick_smelting();
        void reset_smelting();
        bool can_insert_outputs(int* outputs, int* output_stacks, int n_outputs);
        #endif

        unsigned int get_max_input_slots()
        {
            if (this->slot_max <= 0) return 0;
            unsigned int max = this->slot_max - 1;    // remove fuel #
            GS_ASSERT(ydim * this->product_xdim <= (int)max);
            max -= ydim * this->product_xdim; // remove product slot
            return max;
        }

        // fills *inputs with input items, sorted by type, up to max_inputs
        // return number of inputs filled
        int get_sorted_inputs(ItemID* inputs, unsigned int max_inputs)
        {
            // iterate input slots
            // inserting to *inputs, sorted
            GS_ASSERT(max_inputs > 0);
            if (max_inputs <= 0) return 0;
            GS_ASSERT(max_inputs == this->get_max_input_slots());

            int n_inputs = 0;
            //int input_types[max_inputs];
            MALLOX(int, input_types, max_inputs); //type, name, size

            for (unsigned int i=0; i<max_inputs; i++)
            {
                int slot = this->convert_input_slot(i);
                ItemID input = this->slot[slot];
                if (input == NULL_ITEM) continue;
                int input_type = Item::get_item_type(input);
                GS_ASSERT(input_type != NULL_ITEM_TYPE);
                if (input_type == NULL_ITEM_TYPE) continue;
                if (n_inputs == 0)
                {
                    inputs[0] = input;
                    input_types[0] = input_type;
                }
                else
                {   //  insert sorted
                    int j = 0;
                    for (; j<n_inputs; j++)
                    {
                        if (input_types[j] <= input_type) continue;

                        // shift forward
                        for (int k=n_inputs; k>j; k--) inputs[k] = inputs[k-1];
                        for (int k=n_inputs; k>j; k--) input_types[k] = input_types[k-1];
                        
                        // insert
                        inputs[j] = input;
                        input_types[j] = input_type;
                        break;
                    }

                    if (j == n_inputs)
                    {   // append to end
                        inputs[j] = input;
                        input_types[j] = input_type;
                    }
                }
                n_inputs++;
            }

            // Test output
            int last_type = -1;
            for (int i=0; i<n_inputs; i++)
            {
                GS_ASSERT(input_types[i] >= last_type);
                last_type = input_types[i];
            }
            
            return n_inputs;
        }

        int convert_input_slot(int input_slot)
        {
            int yslot = input_slot /  this->input_xdim;
            int slot = input_slot + (yslot * this->product_xdim) + 1;
            return slot;
        }

        int convert_product_slot(int product_slot)
        {   // translate product_slot to native slot
            // calculate yslot with xdim = 1
            int yslot = product_slot / this->product_xdim;
            int slot = xdim * (yslot + 1);
            return slot;
        }

        bool is_smelter_output(int slot)
        {
            int xslot = (slot-1) % this->xdim;  // -1 to offset fuel slot
            return (xslot == this->xdim - 1);   // in last column
        }

        bool can_insert_item(int slot, ItemID item_id)
        {
            GS_ASSERT(this->is_valid_slot(slot));
            if (!this->is_valid_slot(slot)) return false;
            if (item_id == NULL_ITEM) return false;

            // check fuel slot
            if (slot == this->fuel_slot)
                return Item::is_fuel(Item::get_item_type(item_id));
            else if (this->is_smelter_output(slot))
                // last row of x is a fuel slot
                // we can't insert anything here through an action.
                // the insert can only be done by server with special function
                return false;
            return true;
        }

        int get_empty_slot()
        {
            for (int i=1; i<this->slot_max; i++)    // skip fuel slot
            {
                if (this->is_smelter_output(i)) continue;
                if (this->slot[i] == NULL_ITEM)
                    return i;
            }
            return NULL_SLOT;
        }

        void insert_item(int slot, ItemID item_id);
        void remove_item(int slot);

		void remove_fuel() { this->remove_item(this->fuel_slot); }

        void init(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim + 1; // +1 for fuel
            GS_ASSERT(this->slot_max > 0);
            GS_ASSERT(this->slot_max < NULL_SLOT);
            if (this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
            this->slot = new ItemID[this->slot_max];
            for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
        }

    ItemContainerSmelter(ItemContainerType type, int id)
    : ItemContainerInterface(type, id),
    fuel(0.0f),
    fuel_type(NULL_ITEM_TYPE),
    burn_rate(1.0f/30.0f),
    recipe_id(NULL_SMELTING_RECIPE),
    progress(0.0f),
    progress_rate(1.0f/30.0f)
    {}
};

}   // ItemContainer

#include <common/template/dynamic_multi_object_list.hpp>

namespace ItemContainer
{

ItemContainerInterface* create_item_container_interface(int type, int id)
{
    switch (type)
    {
        case AGENT_CONTAINER:
        case AGENT_TOOLBELT:
        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
            return new ItemContainer((ItemContainerType)type, id);

        case AGENT_SYNTHESIZER:
            return new ItemContainerSynthesizer((ItemContainerType)type, id);

        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            return new ItemContainerCraftingBench((ItemContainerType)type, id);

        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
            return new ItemContainerCryofreezer((ItemContainerType)type, id);

        case CONTAINER_TYPE_SMELTER_ONE:
            return new ItemContainerSmelter((ItemContainerType)type, id);

        default:
            printf("ERROR -- %s -- type %d unhandled\n", __FUNCTION__, type);
            GS_ASSERT(false);
            return NULL;
    }
    return NULL;
}

const int ITEM_CONTAINER_MAX = 1024;
const int ITEM_CONTAINER_HARD_MAX = 0xffff;

class ItemContainerList: public DynamicMultiObjectList<ItemContainerInterface, ITEM_CONTAINER_MAX, ITEM_CONTAINER_HARD_MAX>
{
    private:
        const char* name() { return "ItemContainer"; }
    public:

        #if DC_CLIENT
        ItemContainerInterface* create(int type)
        {
            printf("must create item container with id\n");
            GS_ASSERT(false);
            return NULL;
        }

        ItemContainerInterface* create(int type, int id)
        {
            return DynamicMultiObjectList<ItemContainerInterface, ITEM_CONTAINER_MAX, ITEM_CONTAINER_HARD_MAX>::create(type, id);
        }
        #endif

        ItemContainerList()
        : DynamicMultiObjectList<ItemContainerInterface, ITEM_CONTAINER_MAX, ITEM_CONTAINER_HARD_MAX>(create_item_container_interface)
        {
            print_list((char*)this->name(), this);
        }
};

}
