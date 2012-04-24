#pragma once

#include <c_lib/entity/constants.hpp>

typedef struct Stack
{
    int count;
    int max;
} Stack;

const int EMPTY_SLOT = 65535;
class InventorySlot
{
    public:
        int item_id;
        ObjectType item_type;
        int slot;
        Stack stack;

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

    InventorySlot()
    :
    item_id(EMPTY_SLOT), item_type(OBJECT_NONE),
    slot(-1)    // slot is set after allocation
    #if DC_CLIENT
    , sprite_index(0)
    #endif
    {
        this->stack.max = 1;
        this->stack.count = 0;
    }
};


class InventoryContents // dont use behaviour list unless doing the registration model
{
    public:
        InventorySlot* objects;
        int x,y;
        int max;
        int count;

    bool full()
    {
        if (this->count >= this->max)
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
        printf("init contents %d,%d\n", x,y);
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
        this->objects = new InventorySlot[this->max];
        for (int i=0; i<this->max; i++)
            this->objects[i].slot = i;
    }

    InventorySlot* get(int slot)
    {
        if (!this->is_valid_slot(slot))
            return NULL;
        return &this->objects[slot];
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

    bool add(int id, ObjectType type, int stack_size, int slot)
    {
        if (!this->can_add(slot))
            return false;
        if (this->objects[slot].empty() && id != EMPTY_SLOT)
            this->count++;
        this->objects[slot].load(id, type, stack_size);
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
            this->count--;
        this->objects[slot].load(EMPTY_SLOT, OBJECT_NONE, 1);
        return true;
    }

    bool swap(int slota, int slotb)
    {
        if (!this->can_swap(slota, slotb))
            return false;
        InventorySlot itema = this->objects[slota];
        InventorySlot* itemb = &this->objects[slotb];
        //this->objects[slota].load(itemb->item_id, itemb->item_type, itemb->stack.properties.count);
        //this->objects[slotb].load(itema.item_id, itema.item_type, itema.stack.properties.count);
        this->objects[slota].load(itemb->item_id, itemb->item_type, 1);
        this->objects[slotb].load(itema.item_id, itema.item_type, 1);
        return true;
    }

    #if DC_SERVER
    void sendToClient(int inventory_id, int client_id);
    #endif

    InventorySlot* item_at_slot(int x, int y)
    {
        if (!this->is_valid_grid_position(x,y))
            return NULL;
        int slot = this->get_slot(x,y);
        return &this->objects[slot];
    }

    ~InventoryContents()
    {
        if (this->objects != NULL)
            delete[] this->objects;
    }

    InventoryContents()
    :
    objects(NULL),
    x(0), y(0), max(0), count(0)
    {
    }
};
