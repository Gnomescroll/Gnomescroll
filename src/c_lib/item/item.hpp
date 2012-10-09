#pragma once

#if DC_SERVER
#include <common/subscriber_list.hpp>
#endif

namespace Item
{

#if DC_SERVER
void verify_items();
bool is_valid_location_data(ItemLocationType location, int location_id, int container_slot, const int assert_limit);
#endif

const int ITEM_SUBSCRIBER_LIST_INITIAL_SIZE = 1;
const int ITEM_SUBSCRIBER_LIST_HARD_MAX = 1;

class Item
{
    public:

        ItemID id;
        int type;  // stone_block, dirt_block, mining_laser_beta,
        int64_t global_id;

        int durability;
        int stack_size;

        ItemLocationType location;
        int location_id;
        int container_slot;

        int gas_decay;

        #if DC_SERVER
        SubscriberList subscribers;
        bool valid;
        ItemSaveState save_state;
        #endif

    #if DC_SERVER
    bool valid_deserialization()
    {
        if (this->global_id == 0) return false;
        if (this->location == IL_NOWHERE) return false;
        if (this->location != IL_PARTICLE && this->container_slot == NULL_SLOT) return false;
        if (this->type == NULL_ITEM_TYPE) return false;
        if (this->stack_size <= 0 || this->stack_size > get_max_stack_size(this->type)) return false;
        if (this->durability <= 0 || this->durability > get_max_durability(this->type)) return false;
        return true;
    }

    void init_for_loading();   // only to be used by serializer
    #endif
    
    void init(int item_type);
    

    void print()
    {
        printf("Item:\n");
        printf("ID %d\n", id);
        printf("Global ID: %lld\n", this->global_id);
        printf("Group %d\n", get_item_group_for_type(this->type));
        printf("Type %d\n", type);
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
        global_id(0),
        durability(NULL_DURABILITY),
        stack_size(1),
        location(IL_NOWHERE),
        location_id(NULL_LOCATION),
        container_slot(NULL_SLOT),
        gas_decay(NULL_GAS_LIFETIME)
        #if DC_SERVER
        , subscribers(ITEM_SUBSCRIBER_LIST_INITIAL_SIZE, ITEM_SUBSCRIBER_LIST_HARD_MAX)
        , valid(true)
        , save_state(ISS_NONE)
        #endif
    {}

};

}

#include <common/template/dynamic_object_list.hpp>

namespace Item
{

class ItemList: public DynamicObjectList<Item, MAX_ITEMS, ITEM_LIST_HARD_MAX>
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
            Item* item = DynamicObjectList<Item, MAX_ITEMS, ITEM_LIST_HARD_MAX>::create(item_id);
            if (item == NULL) return NULL;
            item->init(item_type);
            return item;
        }
        #endif

        #if DC_SERVER
        Item* create_for_loading()
        {   // only used by serializer
            return this->create();
        }
        
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
        //static const int GAS_TICK_INTERVAL = 30;
        static const int GAS_TICK_INTERVAL = 10;
    public:
        #if DC_SERVER
        void decay_gas();
        void verify_items();
        #endif
};

}   // Item
