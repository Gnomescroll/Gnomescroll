#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/items/inventory/packets.hpp>
#include <c_lib/items/inventory/contents.hpp>

class BaseInventory
{
    public:
        int id;
        ObjectType type;
        int owner;

        InventorySlot* objects;
        int x,y;
        int max;
        int count;

    bool full()
    {
        if (this->count >= this->max) return true;
        return false;
    }

    int get_slot(int x, int y)
    {
        return this->x*y + x;
    }

    int get_empty_slot()
    {
        if (this->full()) return -1;
        for (int i=0; i<this->max; i++)
            if (this->objects[i].empty())
                return i;
        return -1;
    }

    bool is_valid_grid_position(int x, int y)
    {
        if (x < 0 || x >= this->x || y < 0 || y >= this->y) return false;
        return true;
    }

    bool is_valid_slot(int slot)
    {
        if (slot < 0 || slot >= this->max) return false;
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

    bool can_add(ObjectType type)
    {
        if (!this->type_allowed(type)) return false;
        if (this->full()) return false;
        return true;
    }
    
    bool can_add(ObjectType type, int slot)
    {
        if (!this->type_allowed(type)) return false;
        if (!this->is_valid_slot(slot)) return false;
        if (!this->objects[slot].empty()) return false;
        return true;
    }

    bool can_remove(int slot)
    {
        if (!this->is_valid_slot(slot)) return false;
        if (this->objects[slot].empty()) return false;
        return true;
    }

    bool can_swap(int slota, int slotb)
    {
        if (!this->is_valid_slot(slota) || !this->is_valid_slot(slotb)) return false;
        if (slota == slotb) return false;
        return true;
    }

    bool add(int id, ObjectType type, int stack_size, int slot)
    {
        if (!this->can_add(type, slot)) return false;
        if (this->objects[slot].empty() && id != EMPTY_SLOT) this->count++;
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
        if (!this->is_valid_slot(slot)) return false;
        if (!this->objects[slot].empty()) this->count--;
        this->objects[slot].load(EMPTY_SLOT, OBJECT_NONE, 1);
        return true;
    }

    bool swap(int slota, int slotb)
    {
        if (!this->can_swap(slota, slotb)) return false;
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
        if (!this->is_valid_grid_position(x,y)) return NULL;
        int slot = this->get_slot(x,y);
        return &this->objects[slot];
    }


        InventorySlot* get_slots_array()
        {
            return this->objects;
        }

        bool type_allowed(ObjectType type)
        {   // Restrict types here
            if (type == OBJECT_AGENT
              || type == OBJECT_MONSTER_BOMB
              || type == OBJECT_NONE)
                return false;
            return true;
        }

        bool dimension_mismatch(int x, int y)
        {
            //if (this->contents.x != x || this->contents.y != y) return true;
            if (this->x != x || this->y != y) return true;
            return false;
        }

        //bool can_add(ObjectType type)
        //{
            //if (!this->type_allowed(type)) return false;
            //return this->contents.can_add();
        //}

        //bool can_add(ObjectType type, int slot)
        //{
            //if (!this->type_allowed(type)) return false;
            //return this->contents.can_add(slot);
        //}

        //bool can_remove(int slot)
        //{
            //return this->contents.can_remove(slot);
        //}

        //bool can_swap(int slota, int slotb)
        //{
            //return this->contents.can_swap(slota, slotb);
        //}

        bool add(int id, ObjectType type, int stack_size)
        {
            //int slot = this->contents.get_empty_slot();
            int slot = this->get_empty_slot();
            if (slot < 0) return false;
            return this->add(id, type, stack_size, slot);
        }

        //bool add(int id, ObjectType type, int stack_size, int slot)
        //{
            //bool added = this->contents.add(id, type, stack_size, slot);
            //return added;
        //}

        //bool remove(int slot)
        //{
            //bool removed = this->contents.remove(slot);
            //return removed;
        //}

        //bool swap(int slota, int slotb)
        //{
            //bool swapped = this->contents.swap(slota, slotb);
            //return swapped;
        //}

    BaseInventory(int id)
    :
    id(id), type(OBJECT_NONE), owner(NO_AGENT),
    objects(NULL),
    x(0), y(0), max(0), count(0)
    {
        // remember to assign type after creation
        // also, owner if applicable
    }

    ~BaseInventory()
    {
        if (this->objects != NULL) delete[] this->objects;
    }

};
