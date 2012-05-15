#pragma once

namespace Item
{

class Item
{
    public:

        ItemID id;
        int group; //stack, laser, mining_laser
        int type;  // stone_block, dirt_block, mining_laser_beta,

        int energy;
        int durability;
        int stack_size;

        int container_id;
        int container_slot;

    void init(int item_type);

    void print()
    {
        printf("Item:\n");
        printf("ID %d\n", id);
        printf("Group %d\n", group);
        printf("Type %d\n", type);
        printf("Energy %d\n", energy);
        printf("Durability %d\n", durability);
        printf("Stack size %d\n", stack_size);
        printf("Container ID %d\n", container_id);
        printf("Container slot %d\n", container_slot);
    }

    #if DC_SERVER
    ~Item();
    #endif

    explicit Item(int id)
    :   id((ItemID)id),
        group(NULL_ITEM_GROUP),
        type(NULL_ITEM_TYPE),
        durability(NULL_DURABILITY),
        stack_size(1),
        container_id(NULL_CONTAINER),
        container_slot(NULL_SLOT)
    {}

};

}

#include <common/template/object_list.hpp>

namespace Item
{

const int ITEM_LIST_MAX = 1024;

class ItemList: public Object_list<Item, ITEM_LIST_MAX>
{
    private:
        const char* name() { return "Item"; }
    public:
        ItemList() { print_list((char*)this->name(), this); }

        #if DC_CLIENT && !PRODUCTION
        Item* create()
        {
            printf("must create item with id\n");
            assert(false);
            return NULL;
        }
        #endif

        #if DC_CLIENT
        Item* create_type(int item_type, ItemID item_id)
        {
            Item* item = Object_list<Item, ITEM_LIST_MAX>::create(item_id);
            if (item == NULL) return NULL;
            item->init(item_type);
            return item;
        }
        
        Item* get_or_create_type(int item_type, ItemID item_id)
        {
            Item* item = this->get(item_id);
            if (item != NULL)
            {
                if (item_type != item->type)
                    printf("WARNING: %s -- %s, item_type %d does not match item->type %d\n", this->name(), __FUNCTION__, item_type, item->type);
                return item;
            }
            
            item = Object_list<Item, ITEM_LIST_MAX>::create(item_id);
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

        void draw() {}
        void tick() {}
};

}
