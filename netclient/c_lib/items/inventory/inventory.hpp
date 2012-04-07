#pragma once

#include <c_lib/objects/common/interface/policy.hpp>
#include <c_lib/objects/common/interface/layers.hpp>
#include <c_lib/objects/common/list/list.hpp>
#include <c_lib/objects/components/sprite/sprite.hpp>

#include <c_lib/items/inventory/render.hpp>
#include <c_lib/items/inventory/packets.hpp>
#if DC_CLIENT
#include <c_lib/draw/transparent.hpp>
#endif

typedef OwnedState InventoryState;

class InventoryObjectInterface: public InventoryState
{
    public:
        void tick() {}
        void draw() {}
        void update() {}
        void born() {}
        void die() {}

    ~InventoryObjectInterface() {}
    InventoryObjectInterface() {}
};

/* Inventory Property, to use in an object */

const int EMPTY_SLOT = 65535;
class Inventory;

class InventoryProperties: public SpriteProperties
{
    public:
        int item_id; // id would be the id of the main object it is referring to. this lets us get cache info
                // otherwise inconsequential
        Object_types item_type;
        int slot;
        Inventory* inventory;

        void load(int id, Object_types type)
        {
            #if DC_CLIENT
            bool new_icon = (id != this->item_id || type != this->item_type);
            if (this->item_id != EMPTY_SLOT && new_icon)
                unregister_inventory_item_draw_list(this);
            if (type != this->item_type)
                this->sprite_index = get_icon_spritesheet_id(type);
            #endif
            this->item_id = id;
            this->item_type = type;
            #if DC_CLIENT
            if (id != EMPTY_SLOT && new_icon)   // set to new
                register_inventory_item_draw_list(this);
            #endif
            printf("Loaded inventory item %d,%d\n", id,type);

            // TODO -- lookup and set sprite properties
        }

    #if DC_CLIENT
    void get_sprite_data(struct Draw::SpriteData* data);
    #endif
        
    InventoryProperties()
    :
    item_id(EMPTY_SLOT), item_type(OBJ_TYPE_NONE),
    slot(-1),    // slot is set after allocation
    inventory(NULL)
    {
        this->scale = 4.0f;
    }
};

class InventoryContents // dont use behaviour list unless doing the registration model
{
    public:
        InventoryProperties* objects;
        int x,y;

        int max;
        int ct;

        Inventory* inventory;

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
                if (this->objects[i].item_id == EMPTY_SLOT)
                    return i;
            return -1;
        }

        bool is_valid_grid_position(int x, int y)
        {
            if (x < 0 || x >= this->x || y < 0 || y >= this->y)
                return false;
            return true;
        }

        void init(Inventory* inventory, int x, int y)
        {
            if (objects != NULL)
            {
                printf("WARNING: Inventory::init() -- objects is not NULL\n");
                return;
            }
            this->inventory = inventory;
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
            {
                this->objects[i].slot = i;
                this->objects[i].inventory = inventory;
            }
        }

        bool can_add()
        {
            if (this->full())
                return false;
            return true;
        }
        
        bool can_add(int slot)
        {
            if (slot < 0 || slot >= this->max)
                return false;
            if (this->objects[slot].item_id != EMPTY_SLOT)
                return false;
            return true;
        }

        bool add(int id, Object_types type, int slot)
        {
            if (!this->can_add(slot))
                return false;
            if (this->objects[slot].item_id == EMPTY_SLOT && id != EMPTY_SLOT)
                this->ct++;
            this->objects[slot].load(id, type);
            printf("added %d,%d to %d\n", id, type, slot);
            return true;
        }

        bool remove(int x, int y)
        {
            int slot = this->get_slot(x,y);
            return this->remove(slot);
        }
        
        bool remove(int slot)
        {
            if (slot < 0 || slot >= this->max)
                return false;
            if (this->objects[slot].item_id != EMPTY_SLOT)
                this->ct--;
            this->objects[slot].load(EMPTY_SLOT, OBJ_TYPE_NONE);
            return true;
        }

        bool can_remove(int slot)
        {
            if (slot < 0 || slot >= this->max)
                return false;
            if (this->objects[slot].item_id == EMPTY_SLOT)
                return false;
            return true;
        }

        void sendToClient(int inventory_id, int client_id);

        InventoryProperties* item_at_slot(int x, int y)
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
    x(0), y(0), max(0), ct(0)
    {
    }
};

// wrap InventoryContents into a game object
class Inventory: public InventoryObjectInterface
{
    private:
        InventoryContents contents;
    public:

        #if DC_CLIENT
        struct {
            float x,y,z;
        } screen;
        #endif

        // TODO -- move, this is owned specialization
        void attach_to_owner();
        
        void init(int x, int y)
        {
            this->contents.init(this, x,y);
        }

        bool type_allowed(Object_types type)
        {   // Restrict types here
            if (type == OBJ_TYPE_AGENT
              || type == OBJ_TYPE_SLIME
              || type == OBJ_TYPE_NONE)
                return false;
            return true;
        }

        bool dimension_mismatch(int x, int y)
        {
            if (this->contents.x != x || this->contents.y != y)
                return true;
            return false;
        }

        void sendContentsToClient(int client_id)
        {
            this->contents.sendToClient(this->_state.id, client_id);
        }

        bool can_add(Object_types type)
        {
            if (!this->type_allowed(type))
                return false;
            return this->contents.can_add();
        }

        bool can_add(Object_types type, int slot)
        {
            if (!this->type_allowed(type))
                return false;
            return this->contents.can_add(slot);
        }

        void sendToClientAdd(int id, Object_types type, int slot);
        void broadcastAdd(int id, Object_types type, int slot);
        void sendToClientRemove(int slot);
        void broadcastRemove(int slot);

        bool add(int id, Object_types type)
        {
            int slot = this->contents.get_empty_slot();
            if (slot < 0) return false;
            return this->add(id, type, slot);
        }

        bool add(int id, Object_types type, int slot)
        {
            bool added = this->contents.add(id,type,slot);
            return added;
        }

        bool remove(int slot)
        {
            bool removed = this->contents.remove(slot);
            return removed;
        }

        #if DC_SERVER
        void remove_all_action()
        {
            for (int i=0; i<this->contents.max; i++)
            {
                if (this->contents.objects[i].item_id == EMPTY_SLOT)
                    continue;
                this->remove_action(i);
            }
        }

        void remove_action(int slot)
        {
            bool removed = this->remove(slot);
            if (!removed) return;
            if (this->get_owner() != NO_AGENT)
                this->sendToClientRemove(slot);
            else
                this->broadcastRemove(slot);
        }

        bool add_action(int id, Object_types type, int slot)
        {
            bool added = this->add(id,type,slot);
            if (!added) return false;
            if (this->get_owner() != NO_AGENT)
                this->sendToClientAdd(id, type, slot);
            else
                this->broadcastAdd(id, type, slot);
            return added;
        }

        bool add_action(int id, Object_types type)
        {
            int slot = this->contents.get_empty_slot();
            bool added = this->add_action(id,type, slot);
            if (!added) return false;
            if (this->get_owner() != NO_AGENT)
                this->sendToClientAdd(id, type, slot);
            else
                this->broadcastAdd(id, type, slot);
            return added;
        }
        #endif

        #if DC_CLIENT
        void remove_any_action()
        {
            for (int i=0; i<this->contents.max; i++)
            {
                if (this->contents.objects[i].item_id == EMPTY_SLOT)
                    continue;
                this->remove_action(i);
                break;
            }
        }

        void remove_action(int slot)
        {
            bool can_remove = this->contents.can_remove(slot);
            if (!can_remove) return;
            remove_item_from_inventory_CtoS msg;
            msg.inventory_id = this->_state.id;
            msg.slot = slot;
            msg.send();
        }

        void add_action(int id, Object_types type)
        {
            int slot = this->contents.get_empty_slot();
            this->add_action(id, type, slot);
        }
        
        void add_action(int id, Object_types type, int slot)
        {
            bool can_add = this->can_add(type, slot);
            if (!can_add) return;
            add_item_to_inventory_CtoS msg;
            msg.inventory_id = this->_state.id;
            msg.id = id;
            msg.type = type;
            msg.slot = slot;
            msg.send();
        }
        #endif

        /* Network API */
        void sendToClientCreate(int client_id);
        void broadcastCreate();
        void sendToClientState(int client_id);
        void broadcastState();
        void broadcastDeath();

    explicit Inventory(int id);

    ~Inventory()
    {
    }

};
