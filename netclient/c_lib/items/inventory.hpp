#pragma once

#include <c_lib/entity/policy.hpp>
#include <c_lib/entity/layers.hpp>
#include <c_lib/lists/list.hpp>
#include <c_lib/components/component.hpp>

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

class InventoryProperties: public ComponentProperties
{
    public:
    InventoryProperties()
    {}
};

// TODO: deprecate
class InventoryComponent
{
    public:
        InventoryProperties inventory_properties;
    InventoryComponent() {}
};

const int EMPTY_SLOT = 65535; // uint16_t max

class InventoryContents: public BehaviourList
{
    private:
        const char* name() { return "InventoryContents"; }

    public:
        int x,y;

        int get_slot(int x, int y)
        {
            return this->x*y + x;
        }

        int get_empty_slot()
        {
            if (this->full())
                return -1;
            for (int i=0; i<this->max; i++)
                if (this->objects[i] == NULL)
                    return i;
            return -1;
        }

        bool is_valid_grid_position(int x, int y)
        {
            if (x < 0 || x >= this->x || y < 0 || y >= this->y)
                return false;
            return true;
        }

        void init(int x, int y)
        {
            this->set_dimensions(x,y);
        }

        void set_dimensions(int x, int y)
        {
            int new_max = x*y;
            if (new_max <= 0)
            {
                printf("ERROR: Inventory::init() -- dimension %d is <=0: x,y = %d,%d\n", new_max, x,y);
                return;
            }
            if (new_max != this->max)
            {   // allocate buffer
                if (this->objects == NULL)
                    this->objects = (ComponentProperties**)calloc(new_max, sizeof(ComponentProperties*));
                else
                {   // resizing
                    this->objects = (ComponentProperties**)realloc(this->objects, new_max * sizeof(ComponentProperties*));
                    if (new_max > this->max)    // null new memory
                        for (int i=this->max; i<new_max; i++)
                            this->objects[i] = NULL;
                }
            }
            this->x = x;
            this->y = y;
            this->max = new_max;
        }

        bool add(InventoryProperties* obj)
        {   // auto assign slot
            int slot = this->get_empty_slot();
            if (slot < 0)
                return false;
            this->objects[slot] = obj;
            this->ct++;
            return true;
        }

        bool add(InventoryProperties* obj, int x, int y)
        {
            if (this->full())
                return false;
            if (!this->is_valid_grid_position(x,y))
                return false;
            int slot = this->get_slot(x,y);
            if (this->objects[slot] != NULL)
                return false;

            this->objects[slot] = obj;
            this->ct++;
            return true;
        }

        bool remove(int x, int y)
        {
            int slot = this->get_slot(x,y);
            if (slot < 0 && slot >= this->max)
                return false;
            this->objects[slot] = NULL;
            return true;
        }

        void sendToClient(int client_id)
        {
            InventoryProperties* obj;
            for (int i=0; i<this->max; i++)
            {
                obj = (InventoryProperties*)this->objects[i];
                if (obj == NULL) continue;
                obj->obj->sendToClientCreate(client_id);
            }
        }

        InventoryProperties* item_at_slot(int x, int y)
        {
            if (!this->is_valid_grid_position(x,y))
                return NULL;
            int slot = this->get_slot(x,y);
            return (InventoryProperties*)this->objects[slot];
        }

    InventoryContents()
    : BehaviourList(),
    x(0), y(0)
    {
    }
};

class Inventory: public InventoryObjectInterface
{
    private:
    public:
        InventoryContents contents;
        
        void init(int x, int y)
        {
            this->contents.init(x,y);
        }

        void set_dimensions(int x, int y)
        {
            this->contents.set_dimensions(x,y);
        }

        //void add_contents(int* ids, Object_types* types, int n_contents);
        void add_contents_from_packet(uint16_t* ids, uint8_t* types, int n_contents);

        bool type_allowed(Object_types type)
        {   // Restrict types here
            return true;
        }

        bool add(InventoryProperties* obj, int x, int y)
        {
            if (!this->type_allowed(obj->obj->state()->type))
                return false;
            return this->contents.add(obj, x,y);
        }

        bool add(InventoryProperties* obj)
        {   // auto assign slot
            if (!this->type_allowed(obj->obj->state()->type))
                return false;
            return this->contents.add(obj);
        }

        bool remove(int x, int y)
        {
            return this->contents.remove(x,y);
        }

        ObjectPolicyInterface* item_at_slot(int x, int y)
        {
            InventoryProperties* obj = this->contents.item_at_slot(x,y);
            if (obj == NULL)
                return NULL;
            return obj->obj;
        }

        /* Network API */
        void sendToClientCreate(int client_id);
        void broadcastCreate();
        void sendToClientState(int client_id);
        void broadcastState();
        void broadcastDeath();

    explicit Inventory(int id)
    {
        this->_state.id = id;
        this->owned_properties.owner = NO_AGENT;
    }

    ~Inventory()
    {
    }

};


//class add_to_inventory_slot_CtoS: public FixedSizeReliableNetPacketToServer<add_to_inventory_slot_CtoS>
//{
    //public:
        //uint16_t id;
        //uint8_t type;
        //uint8_t x,y;

        //inline void packet(char* buff, int* buff_n, bool pack)
        //{
            //pack_u16(&id, buff, buff_n, pack);
            //pack_u8(&type, buff, buff_n, pack);
            //pack_u8(&x, buff, buff_n, pack);
            //pack_u8(&y, buff, buff_n, pack);
        //}
        //inline void handle();
//};

//class add_to_inventory_CtoS: public FixedSizeReliableNetPacketToServer<add_to_inventory_CtoS>
//{
    //public:
        //uint16_t id;
        //uint8_t type;

        //inline void packet(char* buff, int* buff_n, bool pack)
        //{
            //pack_u16(&id, buff, buff_n, pack);
            //pack_u8(&type, buff, buff_n, pack);
        //}
        //inline void handle();
//};

//class remove_from_inventory_CtoS: public FixedSizeReliableNetPacketToServer<remove_from_inventory_CtoS>
//{
    //public:
        //uint8_t x,y;

        //inline void packet(char* buff, int* buff_n, bool pack)
        //{
            //pack_u8(&x, buff, buff_n, pack);
            //pack_u8(&y, buff, buff_n, pack);
        //}
        //inline void handle();
//};

class inventory_create_StoC: public FixedSizeReliableNetPacketToClient<inventory_create_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t x,y;
        uint8_t owner;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&x, buff, buff_n, pack);
            pack_u8(&y, buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
        }
        inline void handle();
};

void inventory_create_message(inventory_create_StoC* msg, int id, Object_types type, int x, int y, int owner)
{
    msg->id = id;
    msg->type = type;
    msg->x = x;
    msg->y = y;
    msg->owner = owner;
}

class inventory_destroy_StoC: public FixedSizeReliableNetPacketToClient<inventory_create_StoC>
{
    public:
        uint16_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
        }
        inline void handle();
};

void inventory_destroy_message(inventory_destroy_StoC* msg, int id)
{
    msg->id = id;
}

