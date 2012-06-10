#include "_interface.hpp"

#include <common/crash_report/stack_trace.hpp>

#include <item/item.hpp>
#include <item/_state.hpp>

#if DC_SERVER
#include <item/net/StoC.hpp>
#include <item/container/_state.hpp>
#endif

#if DC_CLIENT
#include <item/client.hpp>
#endif

namespace Item
{

void init()
{
    item_list = new ItemList;
    init_properties();
}

void teardown()
{
    if (item_list != NULL) delete item_list;
    tear_down_properties();
}

Item* get_item(ItemID id)
{
    return item_list->get(id);
}

// alias for get_item
class Item* get_item_object(ItemID id)
{
    return get_item(id);
}

int get_item_type(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return NULL_ITEM_TYPE;
    return item->type;
}

ItemGroup get_item_group(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return IG_ERROR;
    return item->group;
}

int get_stack_size(ItemID id)
{   // space used in a stack
    if (id == NULL_ITEM) return 1;
    Item* item = get_item(id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return 1;
    return item->stack_size;
}

int get_stack_space(ItemID id)
{   // space left in a stack
    if (id == NULL_ITEM) return 0;
    Item* item = get_item(id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return 0;
    int stack_space = get_max_stack_size(item->type) - item->stack_size;
    GS_ASSERT(stack_space >= 0);
    if (stack_space < 0) return 0;
    return stack_space;
}

int get_item_durability(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return NULL_DURABILITY;
    return item->durability;
}

// Caller of this function is expected to destroy the src
void merge_item_stack(ItemID src, ItemID dest)
{
    GS_ASSERT(src != NULL_ITEM);
    GS_ASSERT(dest != NULL_ITEM);
    if (src == NULL_ITEM) return;
    if (dest == NULL_ITEM) return;

    Item* src_item = get_item(src);
    GS_ASSERT(src_item != NULL);
    if (src_item == NULL) return;
    Item* dest_item = get_item(dest);
    GS_ASSERT(dest_item != NULL);
    if (dest_item == NULL) return;

    // add src's stack to dest
    dest_item->stack_size += src_item->stack_size;
    GS_ASSERT(dest_item->stack_size <= get_max_stack_size(dest_item->type));
}

void merge_item_stack(ItemID src, ItemID dest, int amount)
{
    GS_ASSERT(src != NULL_ITEM);
    GS_ASSERT(dest != NULL_ITEM);
    GS_ASSERT(amount > 0);
    if (src == NULL_ITEM) return;
    if (dest == NULL_ITEM) return;
    if (amount <= 0) return;
    
    Item* src_item = get_item(src);
    GS_ASSERT(src_item != NULL);
    if (src_item == NULL) return;
    Item* dest_item = get_item(dest);
    GS_ASSERT(dest_item != NULL);
    if (dest_item == NULL) return;

    // add src's stack to dest
    dest_item->stack_size += amount;
    GS_ASSERT(dest_item->stack_size <= get_max_stack_size(dest_item->type));
    
    // remove from src
    src_item->stack_size -= amount;
    GS_ASSERT(src_item->stack_size >= 1);
}

}   // Item
 
// Client
#if DC_CLIENT
namespace Item
{

void destroy_item(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return;

    //int container_id = item->container_id;
    //int slot = item->container_slot;
    //ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
    //if (container != NULL && slot != NULL_SLOT) container->remove_item(slot);
    
    item_list->destroy(id);
}
    
Item* create_item(int item_type, ItemID item_id)
{
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    GS_ASSERT(item_list != NULL);
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_type == NULL_ITEM_TYPE) return NULL;
    if (item_list == NULL) return NULL;
    if (item_id == NULL_ITEM) return NULL;
    return item_list->create_type(item_type, item_id);
}

}   // Item
#endif 

// Server
#if DC_SERVER
namespace Item
{

void destroy_item(ItemID id)
{
    GS_ASSERT(ItemContainer::agent_hand_list != NULL);
    if (ItemContainer::agent_hand_list == NULL) return;
    
    Item* item = get_item(id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return;

    if (item->location == IL_CONTAINER)
    {
        int container_id = item->location_id;
        int slot = item->container_slot;
        ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
        if (container != NULL && slot != NULL_SLOT)
        {
            container->remove_item(slot);
            Agent_state* a = ServerState::agent_list->get(container->owner);
            if (a != NULL) ItemContainer::send_container_remove(a->client_id, container_id, slot);
        }
    }
    else if (item->location == IL_HAND)
    {
        ItemContainer::remove_item_from_hand(item->location_id);
        Agent_state* a = ServerState::agent_list->get(item->location_id);
        if (a != NULL) ItemContainer::send_hand_remove(a->client_id);
    }
    else if (item->location == IL_PARTICLE)
        ItemParticle::destroy(item->location_id);

    // destroy source particle
    if (item->location == IL_PARTICLE) ItemParticle::destroy(item->location_id);
    
    broadcast_item_destroy(id);
    item_list->destroy(id);
}

// broadcasts nothing
void destroy_item_silently(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return;
    if (item->location == IL_CONTAINER)
    {
        int container_id = item->location_id;
        int slot = item->container_slot;
        ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
        if (container != NULL && slot != NULL_SLOT) container->remove_item(slot);
    }
    else if (item->location == IL_PARTICLE)
        ItemParticle::destroy_silently(item->location_id);
    else if (item->location == IL_HAND)
        ItemContainer::remove_item_from_hand(item->location_id);
    
    item_list->destroy(id);
}

ItemID split_item_stack(ItemID src, int amount)
{
    GS_ASSERT(src != NULL_ITEM);
    if (src == NULL_ITEM) return NULL_ITEM;
    GS_ASSERT(amount >= 1);
    if (amount < 1) return NULL_ITEM;

    Item* src_item = get_item(src);
    GS_ASSERT(src_item != NULL);
    if (src_item == NULL) return NULL_ITEM;
    src_item->stack_size -= amount;
    GS_ASSERT(src_item->stack_size >= 1);
    if (src_item->stack_size < 1) return NULL_ITEM;

    Item* new_item = create_item(src_item->type);
    GS_ASSERT(new_item != NULL);
    if (new_item == NULL) return NULL_ITEM;
    new_item->stack_size = amount;
    return new_item->id;
}

ItemID split_item_stack_in_half(ItemID src)
{
    GS_ASSERT(src != NULL_ITEM);
    if (src == NULL_ITEM) return NULL_ITEM;

    Item* src_item = get_item(src);
    GS_ASSERT(src_item != NULL);
    if (src_item == NULL) return NULL_ITEM;
    int split_amount = src_item->stack_size / 2;
    GS_ASSERT(split_amount >= 1);  // Do not call this function for a stack with only 1 (cannot split)
    if (split_amount < 1) return NULL_ITEM;
    src_item->stack_size -= split_amount;

    Item* new_item = create_item(src_item->type);
    if (new_item == NULL) return NULL_ITEM;
    new_item->stack_size = split_amount;
    return new_item->id;
}

Item* create_item(int item_type)
{
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return NULL;
    return item_list->create_type(item_type);
}

Item* create_item(char* item_name)
{
    int item_type = get_item_type(item_name);
    GS_ASSERT(item_type != NULL_ITEM_TYPE);
    if (item_type == NULL_ITEM_TYPE) return NULL;
    return create_item(item_type);
}

// will destroy item if fully consumed
// returns stack size
int consume_stack_item(ItemID item_id)
{
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return 0;
    int stack_size = get_stack_size(item_id);
    GS_ASSERT(stack_size > 0);
    if (stack_size <= 1)
    {
        destroy_item(item_id);
        return 0;
    }
    Item* item = get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return 0;
    item->stack_size -= 1;
    GS_ASSERT(item->stack_size > 0);
    return item->stack_size;
}

// tests
void test_item_list_capacity()
{
    printf("Testing item list capacity\n");
    for (int i=0; i<ITEM_LIST_MAX*2; i++)
        item_list->create_type(0);
}

}   // Item

#endif 
