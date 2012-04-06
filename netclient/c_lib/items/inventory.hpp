#pragma once

#include <c_lib/entity/policy.hpp>
#include <c_lib/entity/layers.hpp>

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


const int EMPTY_SLOT = 65535; // uint16_t max

class Inventory: public InventoryObjectInterface
{
    private:
        int get_slot(int x, int y)
        {
            return this->x*y + x;
        }

        int get_empty_slot()
        {
            if (this->full())
                return -1;
            for (int i=0; i<this->max; i++)
                if (this->contents[i] == NULL)
                    return i;
            return -1;
        }

        bool is_valid_grid_position(int x, int y)
        {
            if (x < 0 || x >= this->x || y < 0 || y >= this->y)
                return false;
            return true;
        }

    public:
        int x,y;
        ObjectPolicyInterface** contents;

        int max;
        int ct;

        int owner;
        
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
        {   // resizing
            if (this->contents == NULL)
                this->contents = (ObjectPolicyInterface**)calloc(new_max, sizeof(ObjectPolicyInterface*));
            else
            {
                this->contents = (ObjectPolicyInterface**)realloc(this->contents, new_max * sizeof(ObjectPolicyInterface*));
                if (new_max > this->max)    // null new memory
                    for (int i=this->max; i<new_max; i++)
                        this->contents[i] = NULL;
            }
        }
        this->x = x;
        this->y = y;
        this->max = new_max;
    }

    void add_contents(int* ids, Object_types* types, int n_contents);
    void add_contents_from_packet(uint16_t* ids, uint8_t* types, int n_contents);

    bool type_allowed(Object_types type)
    {   // Restrict types here
        return true;
    }

    bool full()
    {
        if (this->ct >= this->max)
            return true;
        return false;
    }

    bool add(ObjectPolicyInterface* obj, int x, int y)
    {
        if (!this->type_allowed(obj->state()->type))
            return false;
        if (this->full())
            return false;

        if (!this->is_valid_grid_position(x,y))
            return NULL;
        int slot = this->get_slot(x,y);
        if (this->contents[slot] != NULL)
            return false;

        this->contents[slot] = obj;
        this->ct++;
        return true;
    }

    // auto assign slot
    // TODO: for stackable items, put on stack
    bool add(ObjectPolicyInterface* obj)
    {
        if (!this->type_allowed(obj->state()->type))
            return false;
        int slot = this->get_empty_slot();
        if (slot < 0)
            return false;
        this->contents[slot] = obj;
        this->ct++;
        return true;
    }

    ObjectPolicyInterface* remove(int x, int y)
    {
        ObjectPolicyInterface* obj = this->item_at_slot(x,y);
        int slot = this->get_slot(x,y);
        if (slot >= 0)
            this->contents[slot] = NULL;
        return obj;
    }

    ObjectPolicyInterface* item_at_slot(int x, int y)
    {
        if (!this->is_valid_grid_position(x,y))
            return NULL;
        int slot = this->get_slot(x,y);
        return this->contents[slot];
    }

    explicit Inventory(int id)
    :
    InventoryObjectInterface(this),
    x(0),y(0),
    contents(NULL),
    max(0), ct(0),
    owner(NO_AGENT)
    {
        this->_state.id = id;
    }

    ~Inventory()
    {
        if (this->contents != NULL)
            free(this->contents);
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
    ObjectPolicyInterface** contents
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
    
    ObjectPolicyInterface* elem;
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
            state = elem->state();
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

