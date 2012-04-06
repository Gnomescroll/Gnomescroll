#pragma once

#include <c_lib/entity/policy.hpp>
#include <c_lib/entity/layers.hpp>
#include <c_lib/lists/list.hpp>
#include <c_lib/components/component.hpp>

class Inventory;
typedef DefaultState InventoryState;
class InventoryNetworkInterface: public InventoryState
{
    private:
        Inventory* object;
    public:
        void sendToClientCreate(int client_id);
        void broadcastCreate();
        void sendToClientState(int client_id);
        void broadcastState();
        void broadcastDeath();

    ~InventoryNetworkInterface() {}
    InventoryNetworkInterface(Inventory* object)
    : object(object) {}
};

class InventoryObjectInterface: public InventoryNetworkInterface
{
    public:
        void tick() {}
        void draw() {}
        void update() {}
        void born() {}
        void die() {}

    ~InventoryObjectInterface() {}
    InventoryObjectInterface(Inventory* object)
    : InventoryNetworkInterface(object) {}
};


/* Inventory Property, to use in an object */

class InventoryProperties: public ComponentProperties
{
    public:
    InventoryProperties()
    {}
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
        int owner;
        
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

        void remove(int x, int y)
        {
            int slot = this->contents.get_slot(x,y);
            if (slot >= 0 && slot < this->contents.max)
                this->contents.objects[slot] = NULL;
        }

        ObjectPolicyInterface* item_at_slot(int x, int y)
        {
            if (!this->contents.is_valid_grid_position(x,y))
                return NULL;
            int slot = this->contents.get_slot(x,y);
            if (this->contents.objects[slot] == NULL)
                return NULL;
            return this->contents.objects[slot]->obj;
        }

    explicit Inventory(int id)
    :
    InventoryObjectInterface(this),
    owner(NO_AGENT)
    {
        this->_state.id = id;
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

const int INVENTORY_PACKET_MAX_CONTENTS = 64;
class inventory_create_StoC: public FixedSizeReliableNetPacketToClient<inventory_create_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint8_t x,y;
        uint8_t owner;
        uint16_t content_ids[INVENTORY_PACKET_MAX_CONTENTS];
        uint8_t content_types[INVENTORY_PACKET_MAX_CONTENTS];

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u16(&id, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
            pack_u8(&x, buff, buff_n, pack);
            pack_u8(&y, buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
            for (int i=0; i<INVENTORY_PACKET_MAX_CONTENTS; i++)
            {
                pack_u16(&content_ids[i], buff, buff_n, pack);
                pack_u8(&content_types[i], buff, buff_n, pack);
            }
        }
        inline void handle();
};

bool inventory_create_message(
    inventory_create_StoC* msg,
    int id, Object_types type, int x, int y, int owner,
    InventoryProperties** contents
) {
    int max = x*y;
    if (max > INVENTORY_PACKET_MAX_CONTENTS)
    {
        printf("WARNING: inventory_create_message() -- Inventory max %d exceeds packet contents max %d\n", max, INVENTORY_PACKET_MAX_CONTENTS);
        return false;
    }
    msg->id = id;
    msg->type = type;
    msg->x = x;
    msg->y = y;
    msg->owner = owner;
    
    InventoryProperties* elem;
    ObjectState* state;
    int elem_id;
    Object_types elem_type;
    for (int i=0; i<max; i++)
    {
        elem_id = EMPTY_SLOT;
        elem_type = OBJ_TYPE_NONE;
        elem = contents[i];
        if (elem != NULL)
        {
            state = elem->obj->state();
            elem_id = state->id;
            elem_type = state->type;
        }
        msg->content_ids[i] = elem_id;
        msg->content_types[i] = elem_type;
    }
    return true;
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

