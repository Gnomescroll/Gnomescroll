#pragma once

#if DC_SERVER
#include <common/subscriber_list.hpp>
#endif

namespace Item
{

const int ITEM_SUBSCRIBER_LIST_INITIAL_SIZE = 4;

class Item
{
    public:

        ItemID id;
        int type;  // stone_block, dirt_block, mining_laser_beta,

        int energy;
        int durability;
        int stack_size;

        ItemLocationType location;
        int location_id;
        int container_slot;

        int gas_decay;

        #if DC_SERVER
        SubscriberList subscribers;
        #endif

    void init(int item_type);

    void print()
    {
        printf("Item:\n");
        printf("ID %d\n", id);
        printf("Group %d\n", get_item_group_for_type(this->type));
        printf("Type %d\n", type);
        printf("Energy %d\n", energy);
        printf("Durability %d\n", durability);
        printf("Stack size %d\n", stack_size);
        printf("Location %d\n", location);
        printf("Location ID %d\n", location_id);
        printf("Container slot %d\n", container_slot);
        printf("Gas decay %d\n", gas_decay);
        #if DC_SERVER
        printf("Subscribers %d\n", subscribers.n);
        printf("\t");
        for (unsigned int i=0; i<subscribers.n; i++) printf("%d ", subscribers.subscribers[i]);
        printf("\n");
        #endif
    }

    explicit Item(int id)   // is not ItemID id because of the container list template
    :   id((ItemID)id),
        type(NULL_ITEM_TYPE),
        energy(NULL_ENERGY),
        durability(NULL_DURABILITY),
        stack_size(1),
        location(IL_NOWHERE),
        location_id(-1),
        container_slot(NULL_SLOT),
        gas_decay(ITEM_GAS_LIFETIME)
        #if DC_SERVER
        , subscribers(ITEM_SUBSCRIBER_LIST_INITIAL_SIZE)
        #endif
    {}

};

}

#include <common/template/dynamic_object_list.hpp>

namespace Item
{

const int ITEM_LIST_MAX = 1024;
const int ITEM_LIST_HARD_MAX = 0xffff;  // 65535 maximum (highest ID will be 65534)

class ItemList: public DynamicObjectList<Item, ITEM_LIST_MAX, ITEM_LIST_HARD_MAX>
{
    private:
        const char* name() { return "Item"; }
    public:
        ItemList() : gas_tick(0) { print_list((char*)this->name(), this); }

        #if DC_CLIENT && !PRODUCTION
        Item* create()
        {
            printf("must create item with id\n");
            GS_ASSERT(false);
            return NULL;
        }
        #endif

        #if DC_CLIENT
        Item* create_type(int item_type, ItemID item_id)
        {
            Item* item = DynamicObjectList<Item, ITEM_LIST_MAX, ITEM_LIST_HARD_MAX>::create(item_id);
            if (item == NULL) return NULL;
            item->init(item_type);
            return item;
        }
        
        Item* get_or_create_type(int item_type, ItemID item_id)
        {
            Item* item = this->get(item_id);
            if (item != NULL)
            {
                GS_ASSERT(item_type == item->type);
                return item;
            }
            
            item = DynamicObjectList<Item, ITEM_LIST_MAX, ITEM_LIST_HARD_MAX>::create(item_id);
            if (item == NULL) return NULL;
            item->init(item_type);
            return item;
        }
        #endif

        #if DC_SERVER
        Item* create_type(int item_type)
        {
            Item* item = this->create();
            if (item == NULL) return NULL;
            item->init(item_type);
            return item;
        }
        #endif

    private:
        unsigned int gas_tick;
        static const int GAS_TICK_INTERVAL = 30;
    public:
        void draw() {}
        void tick();
        void verify_items();
};

}   // Item
