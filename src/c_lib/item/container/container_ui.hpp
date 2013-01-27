#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace ItemContainer
{

/*
 * Simulate an ItemContainer for rendering/predictive purpose
 */

struct SlotMetadata
{
    int type;
    int stack_size;
    int durability;
    int charges;
};

const struct SlotMetadata NULL_SLOT_METADATA = {
    NULL_ITEM_TYPE,
    1,
    NULL_DURABILITY,
    NULL_CHARGES
};

struct SlotMetadata copy_item_metadata(ItemID item_id);

class ItemContainerUIInterface
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

        SlotMetadata* slot_metadata;

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

    int get_slot_type(int slot)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return NULL_ITEM_TYPE;
        return this->slot_metadata[slot].type;
    }

    int get_slot_stack(int slot)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return 1;
        return this->slot_metadata[slot].stack_size;
    }

    int get_slot_durability(int slot)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return NULL_DURABILITY;
        return this->slot_metadata[slot].durability;
    }

    int get_slot_charges(int slot)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return 1;
        return this->slot_metadata[slot].charges;
    }

    struct SlotMetadata get_slot_metadata(int slot)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return NULL_SLOT_METADATA;
        return this->slot_metadata[slot];
    }

    void insert_item(int slot, struct SlotMetadata metadata)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return;
        this->slot_metadata[slot] = metadata;
        this->slot_count++;
    }

    void insert_item(int slot, ItemID item_id)
    {
        struct SlotMetadata metadata = copy_item_metadata(item_id);
        this->insert_item(slot, metadata);
    }

    void remove_item(int slot)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return;
        this->slot_metadata[slot] = NULL_SLOT_METADATA;
        this->slot_count--;
    }

    void load_data(ItemID* slots)
    {
        for (int i=0; i<this->slot_max; i++)
        {
            if (slots[i] == NULL_ITEM)
                this->slot_metadata[i] = NULL_SLOT_METADATA;
            else
            {
                Item::Item* item = Item::get_item(slots[i]);
                IF_ASSERT(item == NULL) continue;
                this->slot_metadata[i].type = item->type;
                this->slot_metadata[i].stack_size = item->stack_size;
                this->slot_metadata[i].durability = item->durability;
                this->slot_metadata[i].charges = item->charges;
            }
        }
    }

    int get_stackable_slot(int item_type, int stack_size)
    {
        for (int i=0; i<this->slot_max; i++)
        {
            if (this->slot_metadata[i].type == NULL_ITEM_TYPE) continue;
            if (this->slot_metadata[i].type != item_type) continue;
            int max_stack_size = Item::get_max_stack_size(this->slot_metadata[i].type);
            if (max_stack_size - this->slot_metadata[i].stack_size >= stack_size)
                return i;
        }
        return NULL_SLOT;
    }

    virtual bool can_insert_item(int slot, int item_type)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return false;
        return (item_type != NULL_ITEM_TYPE);
    }

    virtual int get_empty_slot()
    {
        for (int i=0; i<this->slot_max; i++)
            if (this->slot_metadata[i].type == NULL_ITEM_TYPE)
                return i;
        return NULL_SLOT;
    }

    virtual void init(ItemContainerType type, int xdim, int ydim)
    {
        this->set_dimensions(type, xdim, ydim);
        this->allocate_slots();
    }

    virtual void set_alt_parameters(int xdim, int ydim)
    {
        this->alt_xdim = xdim;
        this->alt_ydim = ydim;
    }

    virtual ~ItemContainerUIInterface()
    {
        if (this->slot_metadata != NULL) delete[] this->slot_metadata;
    }

    explicit ItemContainerUIInterface(ItemContainerID id) :
        id(id), type(NULL_CONTAINER_TYPE),
        xdim(0), ydim(0),
        alt_xdim(0), alt_ydim(0),
        slot_max(0), slot_count(0),
        slot_metadata(NULL)
    {}

    protected:

    virtual void set_dimensions(ItemContainerType type, int xdim, int ydim)
    {
        this->type = type;
        this->xdim = xdim;
        this->ydim = ydim;
        this->slot_max = this->xdim * this->ydim;
    }

    virtual void allocate_slots()
    {
        IF_ASSERT(this->slot_max <= 0 || this->slot_max >= NULL_SLOT) return;
        GS_ASSERT(this->slot_metadata == NULL);
        this->slot_metadata = new struct SlotMetadata[this->slot_max];
        for (int i=0; i<this->slot_max; this->slot_metadata[i++] = NULL_SLOT_METADATA);
    }
};

class ItemContainerUI: public ItemContainerUIInterface
{
    public:

    explicit ItemContainerUI(ItemContainerID id) :
        ItemContainerUIInterface(id)
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

    struct SlotMetadata get_item_metadata()
    {
        return ItemContainerUIInterface::get_slot_metadata(0);
    }

    virtual void insert_item(struct SlotMetadata metadata)
    {
        ItemContainerUIInterface::insert_item(0, metadata);
    }

    virtual void insert_item(ItemID item_id)
    {
        struct SlotMetadata metadata = copy_item_metadata(item_id);
        this->insert_item(metadata);
    }

    void remove_item()
    {
        ItemContainerUIInterface::remove_item(0);
    }

    virtual void init(ItemContainerType type, int xdim, int ydim)
    {
        GS_ASSERT(type == name::hand);
        ItemContainerUIInterface::init(type, xdim, ydim);
    }

    explicit ItemContainerHandUI(ItemContainerID id) :
        ItemContainerUIInterface(id)
    {}
};

class ItemContainerEnergyTanksUI: public ItemContainerUIInterface
{
    public:

        int energy_tank_type;

    virtual bool can_insert_item(int slot, int item_type)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return false;
        if (item_type == NULL_ITEM_TYPE) return false;

        return (item_type == this->energy_tank_type);
    }

    virtual void init(ItemContainerType type, int xdim, int ydim)
    {
        this->energy_tank_type = Item::get_item_type("energy_tank");
        GS_ASSERT(this->energy_tank_type != NULL_ITEM_TYPE);
        GS_ASSERT(type == name::energy_tanks);
        ItemContainerUIInterface::init(type, xdim, ydim);
    }

    explicit ItemContainerEnergyTanksUI(ItemContainerID id) :
        ItemContainerUIInterface(id),
        energy_tank_type(NULL_ITEM_TYPE)
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

    virtual bool can_insert_item(int slot, int item_type)
    {
        GS_ASSERT(this->is_valid_slot(slot));
        if (!this->is_valid_slot(slot)) return false;
        if (item_type == NULL_ITEM_TYPE) return false;

        // synthesizer coins only
        static int coin_type = Item::get_item_type("synthesizer_coin");
        GS_ASSERT(coin_type != NULL_ITEM_TYPE);
        return (item_type == coin_type);
    }

    explicit ItemContainerSynthesizerUI(ItemContainerID id) :
        ItemContainerUIInterface(id)
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
        if (slot >= this->slot_max - this->alt_xdim*this->alt_ydim) return true;
        return false;
    }

    int get_fuel()
    {
        IF_ASSERT(this->slot_max < 0) return NULL_ITEM_TYPE;
        if (this->slot_max == 0) return NULL_ITEM_TYPE;
        return this->slot_metadata[0].type;
    }

    virtual bool can_insert_item(int slot, int item_type)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return false;
        if (item_type == NULL_ITEM_TYPE) return false;

        if (slot == 0)
            return Item::is_fuel(item_type);
        else if (this->is_smelter_output(slot))
            return false;
        return true;
    }

    virtual int get_empty_slot()
    {
        for (int i=1; i<this->slot_max; i++)    // skip fuel slot
        {
            if (this->is_smelter_output(i)) continue;
            if (this->slot_metadata[i].type == NULL_ITEM_TYPE)
                return i;
        }
        return NULL_SLOT;
    }

    explicit ItemContainerSmelterUI(ItemContainerID id) :
        ItemContainerUIInterface(id),
        fuel(0.0f), fuel_type(NULL_ITEM_TYPE), progress(0.0f)
    {}

    protected:

    virtual void set_dimensions(ItemContainerType type, int xdim, int ydim)
    {
        this->type = type;
        this->xdim = xdim;
        this->ydim = ydim;
        this->slot_max = xdim*ydim + this->alt_xdim*this->alt_ydim + 1; // +1 for fuel
    }
};

class ItemContainerCrusherUI: public ItemContainerUIInterface
{
    public:

        static const int input_slot = 0;

    int get_input_slot_type()
    {
        IF_ASSERT(!this->is_valid_slot(this->input_slot)) return false;
        return this->slot_metadata[this->input_slot].type;
    }

    bool can_insert_item(int slot, int item_type)
    {
        IF_ASSERT(!this->is_valid_slot(slot)) return false;
        if (item_type == NULL_ITEM_TYPE) return false;

        if (slot == 0) return true;
        return false;
    }

    int get_empty_slot()
    {
        if (this->slot_max <= 0) return NULL_SLOT;
        if (this->slot_metadata[0].type == NULL_ITEM_TYPE) return 0;
        return NULL_SLOT;
    }

    explicit ItemContainerCrusherUI(ItemContainerID id) :
        ItemContainerUIInterface(id)
    {}

    protected:

    virtual void set_dimensions(ItemContainerType type, int xdim, int ydim)
    {
        this->type = type;
        this->xdim = xdim;
        this->ydim = ydim;
        this->slot_max = xdim*ydim + this->alt_xdim * this->alt_ydim;
    }
};

class ItemContainerEquipmentUI: public ItemContainerUIInterface
{
    public:

        EquipmentType* slot_equipment_types;

    virtual void init(ItemContainerType type, int xdim, int ydim)
    {
        GS_ASSERT(type == name::equipment);
        ItemContainerUIInterface::init(type, xdim, ydim);
        IF_ASSERT(this->slot_max <= 0) return;
        GS_ASSERT(this->slot_equipment_types == NULL);
        this->slot_equipment_types = (EquipmentType*)malloc(this->slot_max * sizeof(EquipmentType));
        ItemContainerEquipment::set_slot_equipment_types(this->slot_equipment_types, this->slot_max);
    }

    explicit ItemContainerEquipmentUI(ItemContainerID id) :
        ItemContainerUIInterface(id)
    {}
};

} // Item
