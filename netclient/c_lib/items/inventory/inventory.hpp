#pragma once

#include <c_lib/objects/common/interface/policy.hpp>
#include <c_lib/objects/common/interface/layers.hpp>
#include <c_lib/objects/common/list/list.hpp>

#include <c_lib/items/inventory/render.hpp>

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

class InventoryProperties
{
    public:
        int id; // id would be the id of the main object it is referring to. this lets us get cache info
                // otherwise inconsequential
        Object_types type;
        int slot;
        //ObjectPolicyInterface* obj;

        void load(int id, Object_types type)
        {
            #if DC_CLIENT
            if (id != EMPTY_SLOT && (id != this->id || type != this->type))
                unregister_inventory_item_draw_list(this);
            #endif
            this->id = id;
            this->type = type;
            #if DC_CLIENT
            if (id != EMPTY_SLOT)
                register_inventory_item_draw_list(this);
            #endif
        }
        
    InventoryProperties()
    :
    id(EMPTY_SLOT), type(OBJ_TYPE_NONE),
    slot(-1)    // slot is set after allocation
    {}
};

class InventoryContents // dont use behaviour list unless doing the registration model
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
                if (this->objects[i].id == EMPTY_SLOT)
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

        bool add(int id, Object_types type, int slot)
        {
            if (slot < 0 || slot >= this->max)
                return false;
            if (this->objects[slot].id == EMPTY_SLOT && id != EMPTY_SLOT)
                this->ct++;
            this->objects[slot].load(id, type);
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
            if (this->objects[slot].id != EMPTY_SLOT)
                this->ct--;
            this->objects[slot].load(EMPTY_SLOT, OBJ_TYPE_NONE);
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
        
        void init(int x, int y)
        {
            this->contents.init(x,y);
        }

        bool type_allowed(Object_types type)
        {   // Restrict types here
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

        void add(int id, Object_types type, int slot)
        {
            this->contents.add(id, type, slot);
        }

        void remove(int slot)
        {
            this->contents.remove(slot);
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


//struct Icon
//{
    //char index; // texture index in sheet
    //char sheet; // texture sheet
//};

//struct Icon get_icon_data(Object_types type)
//{
    //struct Icon icon = { 0,0 }; // load "missing" icon here
    //switch (type)
    //{
        //// e.g. :
        ////case OBJ_TYPE_GRENADE_REFILL:
            ////icon.index = 1;
            ////icon.sheet = 0;
        //default: break;
    //}
    //return icon;
//}

//void render_inventory_item(InventoryProperties data)
//{
    //// draw

    //// Available data:
    ////data.id;
    ////data.type;
    ////data.slot;
//}

//void draw_inventory_hud_panel(Object_types inventory_type)
//{
    ////int tex = 0;
    //switch (inventory_type)
    //{   // TODO: replace Object_types with Inventory_type
        //case OBJ_TYPE_INVENTORY:
            ////tex = inventory_panel_texture;
            //break;
        //default: return;
    //}
//}

//// class data members
////class InventoryProperties
////{
    ////public:
        ////int id;
        ////Object_types type;
        ////int slot;
////}

//// draw wrapper will pass inventory data to this function
//void render_inventory(Object_types inventory_type, InventoryProperties* contents, int size)
//{
    //draw_inventory_hud_panel(inventory_type);
    //for (int i=0; i<size; i++)
    //{
        //if (contents[i].id == EMPTY_SLOT)
            //continue;
        //render_inventory_item(contents[i]);
    //}
//}

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

class add_item_to_inventory_StoC: public FixedSizeNetPacketToClient<add_item_to_inventory_StoC>
{
    public:
        uint16_t inventory_id;
        uint16_t id;
        uint8_t type;
        uint8_t slot;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&inventory_id, buff, buff_n, pack);
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u8(&slot, buff, buff_n, pack);
    }
    inline void handle();
};

class remove_item_from_inventory_StoC: public FixedSizeNetPacketToClient<remove_item_from_inventory_StoC>
{
    public:
        uint16_t inventory_id;
        uint8_t slot;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&inventory_id, buff, buff_n, pack);
        pack_u8(&slot, buff, buff_n, pack);
    }
    inline void handle();
};
