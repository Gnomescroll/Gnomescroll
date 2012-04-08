#pragma once

// TODO:

/*  Keep adding various interfaces here (t_mech, t_map, agent etc)
 *  When time is right, create client/ folder and split interfaces
 *
 */

#if DC_CLIENT

#include <c_lib/items/inventory/base.hpp>
#include <c_lib/items/inventory/contents.hpp>
#include <c_lib/objects/common/interface/policy.hpp>


int get_icon_spritesheet_id(Object_types type)
{
    const int ERROR_SPRITE = 0;
    switch (type)
    {
        case OBJ_TYPE_DIRT:
            return 3;
        case OBJ_TYPE_STONE:
            return 4;

        case OBJ_TYPE_SPAWNER:
            return 6;
        case OBJ_TYPE_TURRET:
            return 7;
        
        default:
            return ERROR_SPRITE;
    }
}

class Inventory;
class InventoryProperties: public BaseInventoryProperties, public SpriteProperties
{
    public:
        Inventory* inventory;

    void load(int id, Object_types type);
    void get_sprite_data(struct Draw::SpriteData* data);
        
    InventoryProperties()
    :
    BaseInventoryProperties(), SpriteProperties(),
    inventory(NULL)
    {
        this->scale = 4.0f;
    }
};

class InventoryContents: public BaseInventoryContents<InventoryProperties>
{};

typedef BaseInventory<InventoryContents> BaseInventoryClient;
class Inventory: public BaseInventoryClient
{
    public:

    struct {
        float x,y,z;
    } screen;

    void attach_to_owner();

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

    /* Expose API here */
    bool dimension_mismatch(int x, int y)
    {
        return BaseInventoryClient::dimension_mismatch(x,y);
    }
    bool can_add(Object_types type)
    {
        return BaseInventoryClient::can_add(type);
    }
    bool can_add(Object_types type, int slot)
    {
        return BaseInventoryClient::can_add(type, slot);
    }
    bool add(int id, Object_types type, int slot)
    {
        return BaseInventoryClient::add(id, type, slot);
    }
    bool remove(int slot)
    {
        return BaseInventoryClient::remove(slot);
    }
    void init(int x, int y)
    {
        BaseInventoryClient::init(x,y);
        for (int i=0; i<this->contents.max; i++)
            this->contents.objects[i].inventory = this;
    }

    DUMMY_NETWORK_INTERFACE // to conform to object api

    explicit Inventory(int id);
};

#endif
