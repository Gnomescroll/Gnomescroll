#pragma once

#include <entity/constants.hpp>
#include <inventory/packets.hpp>
#include <inventory/slot.hpp>

class BaseInventory;
class Agent_state;
namespace Items
{
void init_inventory_dimensions(BaseInventory* inventory);
void move_inventory_item_to_world(Agent_state* agent, InventorySlot* item);
} // Items

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

        /* Getters */

        int get_slot(int x, int y)
        {
            return this->x*y + x;
        }

        InventorySlot* get_item(int x, int y)
        {
            if (!this->is_valid_grid_position(x,y)) return NULL;
            int slot = this->get_slot(x,y);
            return &this->objects[slot];
        }

        InventorySlot* get_item(int slot)
        {
            if (!this->is_valid_slot(slot)) return NULL;
            return &this->objects[slot];
        }

        int get_empty_slot()
        {
            if (this->full()) return -1;
            for (int i=0; i<this->max; i++)
                if (this->objects[i].empty())
                    return i;
            return -1;
        }

        // returns slot to stack on if exists
        int get_slot_for_stack(ObjectType type, int stack_size)
        {
            // look for matching
            for (int i=0; i<this->max; i++)
            {
                if (this->objects[i].empty()) continue;
                if (this->objects[i].item_type != type) continue;
                int avail = STACK_SIZE_MAX - this->objects[i].count;
                if (stack_size <= avail) return i;
            }
            return -1;
        }

        InventorySlot* get_slots_array()
        {
            return this->objects;
        }

        /* Filter checks */

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
            if (this->x != x || this->y != y) return true;
            return false;
        }

        bool full()
        {
            if (this->count >= this->max) return true;
            return false;
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
        
        bool can_add(ObjectType type, int slot, int stack_size)
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

        bool can_merge_stack(int slota, int slotb, int count)
        {
            InventorySlot* item = this->get_item(slota);
            if (item == NULL) return false;
            bool can_add = this->can_add_stack(item->item_type, slotb, count);
            if (!can_add) return false;
            bool can_remove = this->can_remove_stack(slota, count);
            return can_remove;
        }

        bool can_add_stack(ObjectType type, int slot, int count)
        {
            if (!this->type_allowed(type)) return false;
            if (!this->is_valid_slot(slot)) return false;
            InventorySlot* item = &this->objects[slot];
            if (item->item_type != type) return false; // only merge same type items
            // TODO -- check stack maximum allowed
            // assume 64
            if (item->count + count > STACK_SIZE_MAX) return false;
            return true;
        }

        bool can_remove_stack(int slot, int count)
        {
            if (!this->is_valid_slot(slot)) return false;
            if (this->objects[slot].count < count) return false;
            return true;
        }

        /* State transactions */

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
            if (this->objects[slot].empty()) return false;
            this->count--;
            this->objects[slot].load(EMPTY_SLOT, OBJECT_NONE, 1);
            return true;
        }

        bool swap(int slota, int slotb)
        {
            if (!this->can_swap(slota, slotb)) return false;
            InventorySlot itema = this->objects[slota];
            InventorySlot* itemb = &this->objects[slotb];
            this->objects[slota].load(itemb->item_id, itemb->item_type, itemb->count);
            this->objects[slotb].load(itema.item_id, itema.item_type, itema.count);
            return true;
        }

        // move count from slota to slotb
        bool merge_stack(int slota, int slotb, int stack_size)
        {
            if (!this->is_valid_slot(slota)) return false;
            if (!this->is_valid_slot(slotb)) return false;
            this->add_stack(slotb, stack_size);
            this->remove_stack(slota, stack_size);
            return true;
        }

        bool set_stack(int slot, int stack_size)
        {
            if (!this->is_valid_slot(slot)) return false;
            if (stack_size <= 0) return false;
            this->objects[slot].count = stack_size;
            return true;
        }

        bool add_stack(int slot, int stack_size)
        {
            if (!this->is_valid_slot(slot)) return false;
            this->objects[slot].count += stack_size;
            return true;
        }

        bool remove_stack(int slot, int stack_size)
        {
            if (!this->is_valid_slot(slot)) return false;
            this->objects[slot].count -= stack_size;
            if (this->objects[slot].count <= 0) this->remove(slot);
            return true;
        }

        /* initializers */

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

    explicit BaseInventory(int id)
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
