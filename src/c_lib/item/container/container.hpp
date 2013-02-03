#pragma once

#include <agent/constants.hpp>

#include <item/common/enum.hpp>
#include <item/common/constants.hpp>
#include <item/properties.hpp>
#include <item/container/config/_interface.hpp>

namespace ItemContainer
{

class ItemContainerInterface
{
    public:
        ItemContainerID id;
        ItemContainerType type;

        int xdim;
        int ydim;
        int alt_xdim;
        int alt_ydim;

        int slot_max;
        int slot_count;
        ItemID* slot;

        AgentID owner;
        int chunk;  // TODO -- move to subclass for container blocks

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
        IF_ASSERT(!this->is_valid_slot(slot)) return NULL_ITEM;
        return this->slot[slot];
    }

    void assign_owner(AgentID owner)
    {
        GS_ASSERT(this->attached_to_agent); // should only use this for attached containers
        this->owner = owner;
    }

    void print()
    {
        for (int i=0; i<this->slot_max; printf("%d ", this->slot[i++]));
        printf("\n");
    }

    int get_stackable_slot(ItemType item_type, int stack_size)
    {
        for (int i=0; i<this->slot_max; i++)
        {
            if (this->slot[i] == NULL_ITEM) continue;
            if (Item::get_item_type(this->slot[i]) == item_type   // it stacks
            && Item::get_stack_space(this->slot[i]) >= stack_size) // stack will fit
                return i;
        }
        return NULL_SLOT;
    }

    // Add alt actions here
    void set_alt_parameters(int xdim, int ydim)
    {
        this->alt_xdim = xdim;
        this->alt_ydim = ydim;
    }

    #if DC_CLIENT
    void clear()
    {
        for (int i=0; i<this->slot_max; i++)
            this->slot[i] = NULL_ITEM;
    }

    virtual void was_assigned()
    {   // callback triggered when container is assigned
        // intentionally blank. only need to override this if
        // the container cares about the event
    }
    #endif

    virtual int insert_item(int slot, ItemID item_id);  // returns slot it was inserted in, or NULL_SLOT on error
    virtual void remove_item(int slot);

    virtual bool can_insert_item(int slot, ItemID item_id)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return false;
        return (item_id != NULL_ITEM);
    }

    virtual int get_empty_slot()
    {
        for (int i=0; i<this->slot_max; i++)
            if (this->slot[i] == NULL_ITEM)
                return i;
        return NULL_SLOT;
    }

    virtual bool can_be_opened_by(AgentID agent_id)
    {
        IF_ASSERT(this->attached_to_agent && this->owner != agent_id) return false;
        return (this->owner == agent_id || this->owner == NULL_AGENT);
    }

    virtual bool lock(AgentID agent_id)
    {
        GS_ASSERT(isValid(agent_id));
        IF_ASSERT(!this->can_be_opened_by(agent_id)) return false;
        IF_ASSERT(this->attached_to_agent) return false;
        this->owner = agent_id;
        return true;
    }

    virtual bool unlock(AgentID agent_id)
    {
        GS_ASSERT(isValid(agent_id));
        GS_ASSERT(this->owner != NULL_AGENT);
        IF_ASSERT(this->attached_to_agent) return false;
        IF_ASSERT(this->owner != agent_id) return false;
        this->owner = NULL_AGENT;
        return true;
    }

    virtual void init(int xdim, int ydim)
    {
        this->init_slot_max(xdim, ydim);
        this->init_slots();
    }

    virtual ~ItemContainerInterface()
    {
       if (this->slot != NULL) delete[] this->slot;
    }

    ItemContainerInterface(ItemContainerType type, ItemContainerID id) :
        id(id), type(type),
        xdim(0), ydim(0),
        alt_xdim(0), alt_ydim(0),
        slot_max(0), slot_count(0), slot(NULL),
        owner(NULL_AGENT), chunk(0xFFFF),
        attached_to_agent(false)
    {}

    protected:

    virtual void init_slot_max(int xdim, int ydim)
    {
        this->xdim = xdim;
        this->ydim = ydim;
        this->slot_max = xdim*ydim;
    }

    virtual void init_slots()
    {
        GS_ASSERT(this->slot == NULL);
        IF_ASSERT(this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
        this->slot = new ItemID[this->slot_max];
        for (int i=0; i<this->slot_max; this->slot[i++] = NULL_ITEM);
    }
};

class ItemContainer: public ItemContainerInterface
{
    public:

    #if DC_CLIENT
    void was_assigned()
    {
        if (this->type == name::toolbelt)
            Toolbelt::assign_toolbelt(this->id);
    }
    #endif

    ItemContainer(ItemContainerType type, ItemContainerID id) :
        ItemContainerInterface(type, id)
    {}
};

class ItemContainerHand: public ItemContainerInterface
{
    public:

    virtual ItemID get_item()
    {
        return ItemContainerInterface::get_item(0);
    }

    virtual void remove_item()
    {
        ItemContainerInterface::remove_item(0);
    }

    virtual int insert_item(ItemID item_id);

    // dont use this interface
    virtual void remove_item(int slot)
    {
        GS_ASSERT(false);
    }

    virtual ItemID get_item(int slot)
    {
        GS_ASSERT(false);
        return NULL_ITEM;
    }

    virtual int insert_item(int slot, ItemID item_id)
    {
        GS_ASSERT(false);
        return NULL_SLOT;
    }

    ItemContainerHand(ItemContainerType type, ItemContainerID id) :
        ItemContainerInterface(type, id)
    {}
};

class ItemContainerEnergyTanks: public ItemContainerInterface
{
    public:

        ItemType energy_tank_type;

    virtual int insert_item(int slot, ItemID item_id);

    virtual bool can_insert_item(int slot, ItemID item_id)
    {
        GS_ASSERT(this->energy_tank_type != NULL_ITEM_TYPE);
        if (Item::get_item_type(item_id) != this->energy_tank_type) return false;
        return ItemContainerInterface::can_insert_item(slot, item_id);
    }

    int consume_energy_tank()
    {   // returns number of energy tanks before consumption
        int n = this->slot_count;
        IF_ASSERT(n < 0) return n;
        if (n == 0) return n;

        for (int i=this->slot_max-1; i>=0; i--)
        {   // consume from the end of the slot array
            if (this->slot[i] == NULL_ITEM) continue;
            Item::destroy_item(this->slot[i]);
            break;
        }

        return n;
    }

    virtual void init(int xdim, int ydim)
    {
        this->energy_tank_type = Item::get_item_type("energy_tank");
        GS_ASSERT(this->energy_tank_type != NULL_ITEM_TYPE);
        ItemContainerInterface::init(xdim, ydim);
    }

    ItemContainerEnergyTanks(ItemContainerType type, ItemContainerID id) :
        ItemContainerInterface(type, id), energy_tank_type(NULL_ITEM_TYPE)
    {
        GS_ASSERT(type == name::energy_tanks);
    }
};

class ItemContainerCryofreezer: public ItemContainer
{
    public:

    int insert_item(int slot, ItemID item_id);

    ItemContainerCryofreezer(ItemContainerType type, ItemContainerID id) :
        ItemContainer(type, id)
    {}
};

class ItemContainerSynthesizer: public ItemContainerInterface
{
    public:

        static const int coins_slot = 0;
        ItemType coins_type;

    ItemID get_coins()
    {
        return this->get_item(this->coins_slot);
    }

    int insert_coins(ItemID item_id)
    {
        GS_ASSERT(Item::get_item_type(item_id) == this->coins_type);
        return this->insert_item(this->coins_slot, item_id);
    }

    virtual bool can_insert_item(int slot, ItemID item_id)
    {   // only allow coins
        if (Item::get_item_type(item_id) != this->coins_type) return false;
        return ItemContainerInterface::can_insert_item(slot, item_id);
    }

    virtual void init(int xdim, int ydim)
    {
        this->coins_type = Item::get_item_type("synthesizer_coin");
        GS_ASSERT(this->coins_type != NULL_ITEM_TYPE);
        ItemContainerInterface::init(xdim, ydim);
    }

    ItemContainerSynthesizer(ItemContainerType type, ItemContainerID id) :
        ItemContainerInterface(type, id), coins_type(NULL_ITEM_TYPE)
    {}
};

class ItemContainerCraftingBench: public ItemContainerInterface
{
    public:

    ItemContainerCraftingBench(ItemContainerType type, ItemContainerID id) :
        ItemContainerInterface(type, id)
    {}
};


class ItemContainerSmelter: public ItemContainerInterface
{
    public:

        static const int input_xdim = 1;
        static const int fuel_slot = 0;

        #if DC_SERVER
        bool on;
        #endif

        float fuel;       // 0.0f - 1.0f
        ItemType fuel_type;  // item type
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
        IF_ASSERT(this->slot_max <= 0) return NULL_ITEM;
        GS_ASSERT(this->is_valid_slot(this->fuel_slot));
        if (!this->is_valid_slot(this->fuel_slot)) return NULL_ITEM;
        return this->slot[this->fuel_slot];
    }

    #if DC_SERVER
    void burn_fuel();
    void reset_fuel();
    void fill_fuel(ItemType fuel_type);
    void begin_smelting(int recipe_id);
    void tick_smelting();
    void reset_smelting();
    #endif

    bool can_produce_output();
    bool can_produce_output(class Item::SmeltingRecipe** pRecipe, int* pRecipe_id);
    bool can_insert_outputs(ItemType* outputs, int* output_stacks, int n_outputs);

    unsigned int get_max_input_slots()
    {
        if (this->slot_max <= 0) return 0;
        int max  = this->slot_max - this->alt_xdim*this->alt_ydim - 1;
        if (max <= 0) return 0;
        return (unsigned int)max;
    }

    // fills *inputs with input items, sorted by type, up to max_inputs
    // return number of inputs filled
    int get_sorted_inputs(ItemID* inputs, unsigned int max_inputs)
    {
        // iterate input slots
        // inserting to *inputs, sorted
        IF_ASSERT(max_inputs <= 0) return 0;
        GS_ASSERT(max_inputs == this->get_max_input_slots());

        int n_inputs = 0;
        //int input_types[max_inputs];
        MALLOX(int, input_types, max_inputs); //type, name, size

        for (size_t i=0; i<max_inputs; i++)
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
        return 1;
    }

    int convert_product_slot(int product_slot)
    {   // translate product_slot to native slot
        return 2;
    }

    bool is_smelter_output(int slot)
    {
        return (slot == 2);
    }

    virtual bool can_insert_item(int slot, ItemID item_id)
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
        return ItemContainerInterface::can_insert_item(slot, item_id);
    }

    virtual int get_empty_slot()
    {
        for (int i=1; i<this->slot_max; i++)    // skip fuel slot
        {
            if (this->is_smelter_output(i)) continue;
            if (this->slot[i] == NULL_ITEM)
                return i;
        }
        return NULL_SLOT;
    }

    void remove_fuel()
    {
        this->remove_item(this->fuel_slot);
    }

    ItemContainerSmelter(ItemContainerType type, ItemContainerID id) :
        ItemContainerInterface(type, id),
        #if DC_SERVER
        on(false),
        #endif
        fuel(0.0f), fuel_type(NULL_ITEM_TYPE), burn_rate(1.0f/30.0f),
        recipe_id(NULL_SMELTING_RECIPE),
        progress(0.0f), progress_rate(1.0f/30.0f)
    {}

    protected:

        virtual void init_slot_max(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            GS_ASSERT(alt_xdim * alt_ydim > 0);
            GS_ASSERT(xdim * ydim > 0);
            this->slot_max = xdim*ydim + alt_xdim*alt_ydim + 1; // +1 for fuel
        }
};

class ItemContainerCrusher: public ItemContainerInterface
{
    public:

        static const int input_slot = 0;

    ItemID get_input_slot()
    {
        GS_ASSERT(this->is_valid_slot(this->input_slot));
        if (!this->is_valid_slot(this->input_slot)) return NULL_ITEM;
        return this->slot[this->input_slot];
    }

    virtual int get_empty_slot()
    {
        if (this->get_input_slot() == NULL_ITEM) return this->input_slot;
        return NULL_SLOT;
    }

    virtual bool can_insert_item(int slot, ItemID item_id)
    {
        if (slot != this->input_slot) return false;
        return ItemContainerInterface::can_insert_item(slot, item_id);
    }

    ItemContainerCrusher(ItemContainerType type, ItemContainerID id) :
        ItemContainerInterface(type, id)
    {}

    protected:

        virtual void set_slot_max(int xdim, int ydim)
        {
            this->xdim = xdim;
            this->ydim = ydim;
            this->slot_max = xdim*ydim + this->alt_xdim*this->alt_ydim;
        }
};

class ItemContainerEquipment: public ItemContainerInterface
{
    public:

        EquipmentType* slot_equipment_types;

    virtual int insert_item(int slot, ItemID item_id);
    virtual bool can_insert_item(int slot, ItemID item_id);
    virtual void remove_item(int slot);

    EquipmentType get_slot_equipment_type(int slot)
    {
        if (!this->is_valid_slot(slot)) return NULL_EQUIPMENT_TYPE;
        return this->slot_equipment_types[slot];
    }

    static void set_slot_equipment_types(EquipmentType* slot_equipment_types, int slot_max)
    {
        int i=0;
        // We define which equipment type goes in each slot.
        // If you ever need to add a new equipment type,
        // It must be APPENDED to this array
        // or the deserializer will break.
        // Also, NEVER CHANGE THIS ORDER
        slot_equipment_types[i++] = EQUIPMENT_TYPE_BODY;
        slot_equipment_types[i++] = EQUIPMENT_TYPE_HEAD;
        slot_equipment_types[i++] = EQUIPMENT_TYPE_FEET;
        slot_equipment_types[i++] = EQUIPMENT_TYPE_HANDS;
        slot_equipment_types[i++] = EQUIPMENT_TYPE_ACCESSORY;
        slot_equipment_types[i++] = EQUIPMENT_TYPE_ACCESSORY;
        slot_equipment_types[i++] = EQUIPMENT_TYPE_ACCESSORY;
        //slot_equipment_types[i++] = A_NEW_EQUIPMENT_TYPE;

        GS_ASSERT(i == slot_max);

        // check that the number of accesories assigned matches our const
        // we dont iterate N_ACCESSORIES to add our accessories, because
        // the deserializer does not support remapping equipment type positions
        int accessory_count = 0;
        for (int j=0; j<i; j++)
            if (slot_equipment_types[j] == EQUIPMENT_TYPE_ACCESSORY)
                accessory_count++;
        GS_ASSERT(accessory_count == N_ACCESSORIES);
    }

    virtual int get_empty_slot()
    {
        return NULL_SLOT;
    }

    virtual void init(int xdim, int ydim)
    {
        ItemContainerInterface::init(xdim, ydim);
        IF_ASSERT(this->slot_max <= 0) return;
        GS_ASSERT(this->slot_equipment_types == NULL);
        this->slot_equipment_types = (EquipmentType*)malloc(this->slot_max * sizeof(EquipmentType));
        this->set_slot_equipment_types(this->slot_equipment_types, this->slot_max);
    }

    virtual ~ItemContainerEquipment()
    {
        if (this->slot_equipment_types != NULL) free(this->slot_equipment_types);
    }

    ItemContainerEquipment(ItemContainerType type, ItemContainerID id) :
        ItemContainerInterface(type, id),
        slot_equipment_types(NULL)
    {
        GS_ASSERT(type == name::equipment);
    }
};

/* Ctor wrappers */

ItemContainerInterface* create_item_container_interface(int type, int id);

ItemContainerInterface* new_crusher(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainerCrusher(type, id);
}

ItemContainerInterface* new_smelter(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainerSmelter(type, id);
}

ItemContainerInterface* new_cryofreezer(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainerCryofreezer(type, id);
}

ItemContainerInterface* new_crafting_bench(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainerCraftingBench(type, id);
}

ItemContainerInterface* new_synthesizer(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainerSynthesizer(type, id);
}

ItemContainerInterface* new_energy_tanks(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainerEnergyTanks(type, id);
}

ItemContainerInterface* new_hand(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainerHand(type, id);
}

ItemContainerInterface* new_container(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainer(type, id);
}

ItemContainerInterface* new_equipment(ItemContainerType type, ItemContainerID id)
{
    return new ItemContainerEquipment(type, id);
}

/* ObjectList */

class ItemContainerList: public MultiObject_list<ItemContainerInterface>
{
    private:

    const char* name()
    {
        return "ItemContainer";
    }

    public:

    #if DC_CLIENT
    ItemContainerInterface* create(int type)
    {
        GS_ASSERT(false);
        printf("must create item container with id\n");
        return NULL;
    }

    ItemContainerInterface* create(int type, int id)
    {
        return MultiObject_list<ItemContainerInterface>::create(type, id);
    }
    #endif

    ItemContainerList(size_t capacity) :
        MultiObject_list<ItemContainerInterface>(capacity, create_item_container_interface)
    {}
};

}   // ItemContainer
