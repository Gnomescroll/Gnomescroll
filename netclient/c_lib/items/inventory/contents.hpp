#pragma once

#include <c_lib/common/enum_types.hpp>

const int EMPTY_SLOT = 65535;
class BaseInventoryProperties
{
    public:
        int item_id;
        Object_types item_type;
        int item_subtype;
        int slot;

    bool empty()
    {
        if (this->item_id == EMPTY_SLOT)
            return true;
        return false;
    }

    BaseInventoryProperties()
    :
    item_id(EMPTY_SLOT), item_type(OBJ_TYPE_NONE), item_subtype(SUBTYPE_NONE),
    slot(-1)    // slot is set after allocation
    {
    }
};

template <class InventoryProperties>
class BaseInventoryContents // dont use behaviour list unless doing the registration model
{
    public:
        InventoryProperties* objects;
        int x,y;
        int max;
        int ct;

    bool full()
    {
        if (this->ct >= this->max)
            return true;
        return false;
    }

    int get_slot(int x, int y)
    {
        return this->x*y + x;
    }

    int get_empty_slot()
    {
        if (this->full())
            return -1;
        for (int i=0; i<this->max; i++)
            if (this->objects[i].empty())
                return i;
        return -1;
    }

    bool is_valid_grid_position(int x, int y)
    {
        if (x < 0 || x >= this->x || y < 0 || y >= this->y)
            return false;
        return true;
    }

    bool is_valid_slot(int slot)
    {
        if (slot < 0 || slot >= this->max)
            return false;
        return true;
    }

    void init(int x, int y)
    {
        if (objects != NULL)
        {
            printf("WARNING: Inventory::init() -- objects is not NULL\n");
            return;
        }
        this->x = x;
        this->y = y;
        this->max = x*y;
        if (this->max <= 0)
        {
            printf("ERROR: Inventory::init() -- dimension %d is <=0: x,y = %d,%d\n", this->max, x,y);
            return;
        }
        this->objects = new InventoryProperties[this->max];
        for (int i=0; i<this->max; i++)
            this->objects[i].slot = i;
    }

    bool can_add()
    {
        if (this->full())
            return false;
        return true;
    }
    
    bool can_add(int slot)
    {
        if (!this->is_valid_slot(slot))
            return false;
        if (!this->objects[slot].empty())
            return false;
        return true;
    }

    bool can_remove(int slot)
    {
        if (!this->is_valid_slot(slot))
            return false;
        if (this->objects[slot].empty())
            return false;
        return true;
    }

    bool can_swap(int slota, int slotb)
    {
        if (!this->is_valid_slot(slota) || !this->is_valid_slot(slotb))
            return false;
        if (slota == slotb)
            return false;
        return true;
    }

    bool add(int id, Object_types type, int subtype, int slot)
    {
        if (!this->can_add(slot))
            return false;
        if (this->objects[slot].empty() && id != EMPTY_SLOT)
            this->ct++;
        this->objects[slot].load(id, type, subtype);
        return true;
    }

    bool remove(int x, int y)
    {
        int slot = this->get_slot(x,y);
        return this->remove(slot);
    }
    
    bool remove(int slot)
    {
        if (!this->is_valid_slot(slot))
            return false;
        if (!this->objects[slot].empty())
            this->ct--;
        this->objects[slot].load(EMPTY_SLOT, OBJ_TYPE_NONE, SUBTYPE_NONE);
        return true;
    }

    bool swap(int slota, int slotb)
    {
        if (!this->can_swap(slota, slotb))
            return false;
        int item_id = this->objects[slota].item_id;
        Object_types item_type = this->objects[slota].item_type;
        int item_subtype = this->objects[slota].item_subtype;
        this->objects[slota].load(this->objects[slotb].item_id, this->objects[slotb].item_type, this->objects[slotb].item_subtype);
        this->objects[slotb].load(item_id, item_type, item_subtype);
        return true;
    }

    InventoryProperties* item_at_slot(int x, int y)
    {
        if (!this->is_valid_grid_position(x,y))
            return NULL;
        int slot = this->get_slot(x,y);
        return &this->objects[slot];
    }

    ~BaseInventoryContents<InventoryProperties>()
    {
        if (this->objects != NULL)
            delete[] this->objects;
    }

    BaseInventoryContents<InventoryProperties>()
    :
    objects(NULL),
    x(0), y(0), max(0), ct(0)
    {
    }
};
