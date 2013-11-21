/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

#include <common/crash_report/stack_trace.hpp>
#include <agent/_interface.hpp>
#include <item/item.hpp>
#include <item/_state.hpp>

#if DC_SERVER
# include <item/net/StoC.hpp>
# include <item/container/_state.hpp>
# include <item/container/_interface.hpp>
# include <item/toolbelt/_interface.hpp>
# include <item/toolbelt/_state.hpp>
#endif

namespace Item
{

void init()
{
    GS_ASSERT(item_list == NULL);
    item_list = new ItemList(MAX_ITEMS);
}

void teardown()
{
    delete item_list;
}

void init_packets()
{
    item_create_StoC::register_client_packet();
    item_state_StoC::register_client_packet();
    item_destroy_StoC::register_client_packet();
    item_charges_StoC::register_client_packet();
    request_item_create_CtoS::register_server_packet();
}

Item* get_item(ItemID id)
{
    if (id == NULL_ITEM) return NULL;
    return item_list->get(id);
}

ItemType get_item_type(ItemID id)
{
    if (id == NULL_ITEM) return NULL_ITEM_TYPE;
    Item* item = get_item(id);
    IF_ASSERT(item == NULL) return NULL_ITEM_TYPE;
    return item->type;
}

ItemGroup get_item_group(ItemID id)
{
    if (id == NULL_ITEM) return IG_ERROR;
    Item* item = get_item(id);
    IF_ASSERT(item == NULL) return IG_ERROR;
    return get_item_group_for_type(item->type);
}

int get_stack_size(ItemID id)
{   // space used in a stack
    if (id == NULL_ITEM) return 1;
    Item* item = get_item(id);
    IF_ASSERT(item == NULL) return 1;
    return item->stack_size;
}

int get_stack_space(ItemID id)
{   // space left in a stack
    if (id == NULL_ITEM) return 0;
    Item* item = get_item(id);
    IF_ASSERT(item == NULL) return 0;
    int stack_space = get_max_stack_size(item->type) - item->stack_size;
    IF_ASSERT(stack_space < 0) return 0;
    return stack_space;
}

int get_item_durability(ItemID id)
{
    Item* item = get_item(id);
    IF_ASSERT(item == NULL) return NULL_DURABILITY;
    return item->durability;
}

int get_item_charges(ItemID id)
{
    Item* item = get_item(id);
    IF_ASSERT(item == NULL) return NULL_CHARGES;
    return item->charges;
}

// Caller of this function is expected to destroy the src
void merge_item_stack(ItemID src, ItemID dest)
{
    Item* src_item = get_item(src);
    IF_ASSERT(src_item == NULL) return;
    Item* dest_item = get_item(dest);
    IF_ASSERT(dest_item == NULL) return;

    // add src's stack to dest
    dest_item->stack_size += src_item->stack_size;
    GS_ASSERT(dest_item->stack_size <= get_max_stack_size(dest_item->type));
}

void merge_item_stack(ItemID src, ItemID dest, int amount)
{
    IF_ASSERT(amount <= 0) return;

    Item* src_item = get_item(src);
    IF_ASSERT(src_item == NULL) return;
    Item* dest_item = get_item(dest);
    IF_ASSERT(dest_item == NULL) return;

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
    item_list->destroy(id);
}

Item* create_item(ItemType item_type, ItemID item_id)
{
    IF_ASSERT(item_type == NULL_ITEM_TYPE) return NULL;
    IF_ASSERT(!isValid(item_id)) return NULL;
    return item_list->create_type(item_type, item_id);
}

#if !PRODUCTION
bool request_item_create(const char* name, int stack_size)
{
    IF_ASSERT(stack_size <= 0 || stack_size > MAX_STACK_SIZE) return false;
    ItemType type = get_item_type(name);
    IF_ASSERT(type == NULL_ITEM_TYPE) return false;
    request_item_create_CtoS msg;
    msg.type = type;
    msg.stack_size = stack_size;
    msg.send();
    return true;
}
#endif


}   // Item
#endif

// Server
#if DC_SERVER
namespace Item
{

static void _item_destruction_event(Item* item)
{
    IF_ASSERT(item == NULL) return;
    static const ItemType scoped_laser_rifle = get_item_type("scoped_laser_rifle");
    static const ItemType glass_scope = get_item_type("glass_scope");
    IF_ASSERT(!isValid(scoped_laser_rifle) || !isValid(glass_scope)) return;
    if (item->type != scoped_laser_rifle) return;   // must be scoped_laser_rifle
    if (item->durability > 0) return;               // must be destroyed via lack of durability
    if (item->location != IL_CONTAINER) return;     // must be in container (toolbelt)
    AgentID owner = ItemContainer::get_container_owner((ItemContainerID)item->location_id);
    Agents::Agent* agent = Agents::get_agent(owner);
    IF_ASSERT(agent == NULL) return;
    struct Vec3 pos = agent->get_center();
    ItemParticle::create_item_particle(glass_scope, pos, vec3_init(0));
}

void destroy_item(ItemID id)
{
    Item* item = get_item(id);
    IF_ASSERT(item == NULL) return;

    _item_destruction_event(item);

    if (item->location == IL_CONTAINER)
    {
        ItemContainerID container_id = (ItemContainerID)item->location_id;
        int slot = item->container_slot;
        ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
        if (container != NULL && slot != NULL_SLOT)
        {
            container->remove_item(slot);
            if (container->owner != NULL_AGENT)
            {
                if (container->id == ItemContainer::get_agent_toolbelt(container->owner)
                 && item->id == Toolbelt::get_agent_selected_item(container->owner))
                {
                    GS_ASSERT(slot == Toolbelt::get_agent_selected_slot(container->owner));
                    Toolbelt::force_remove_selected_item(container->owner);
                }
                Agents::Agent* a = Agents::get_agent(container->owner);
                if (a != NULL) ItemContainer::send_container_remove(a->client_id, container_id, slot);
            }
        }
    }
    else if (item->location == IL_HAND)
    {
        ItemContainer::remove_item_from_hand((AgentID)item->location_id);    // we're destroying the item, so subscriptions dont matter
        Agents::Agent* a = Agents::get_agent((AgentID)item->location_id);
        if (a != NULL) ItemContainer::send_hand_remove(a->client_id);
    }
    else if (item->location == IL_PARTICLE)
        ItemParticle::destroy((ItemParticleID)item->location_id);

    send_item_destroy(id);
    item_list->destroy(id);
}

void destroy_item_for_loading(ItemID id)
{   // only used by serializer; skips all the maintenance stuff
    // the item never reached the system, it was deemed invalid
    item_list->destroy(id);
}

ItemID split_item_stack(ItemID src, int amount)
{
    IF_ASSERT(src == NULL_ITEM) return NULL_ITEM;
    IF_ASSERT(amount < 1) return NULL_ITEM;

    Item* src_item = get_item(src);
    IF_ASSERT(src_item == NULL) return NULL_ITEM;
    src_item->stack_size -= amount;
    IF_ASSERT(src_item->stack_size < 1) return NULL_ITEM;

    Item* new_item = create_item(src_item->type);
    IF_ASSERT(new_item == NULL) return NULL_ITEM;
    new_item->stack_size = amount;
    return new_item->id;
}

ItemID split_item_stack_in_half(ItemID src)
{   // Do not call this function for a stack with only 1 (cannot split)
    IF_ASSERT(src == NULL_ITEM) return NULL_ITEM;

    Item* src_item = get_item(src);
    IF_ASSERT(src_item == NULL) return NULL_ITEM;
    int split_amount = src_item->stack_size / 2;
    IF_ASSERT(split_amount < 1) return NULL_ITEM;
    src_item->stack_size -= split_amount;

    Item* new_item = create_item(src_item->type);
    IF_ASSERT(new_item == NULL) return NULL_ITEM;
    new_item->stack_size = split_amount;
    return new_item->id;
}

unsigned int item_space()
{
    return item_list->space();
}

class Item* create_item(ItemType item_type)
{
    IF_ASSERT(item_type == NULL_ITEM_TYPE) return NULL;
    return item_list->create_type(item_type);
}

class Item* create_item(const char* item_name)
{
    ItemType item_type = get_item_type(item_name);
    IF_ASSERT(item_type == NULL_ITEM_TYPE) return NULL;
    return create_item(item_type);
}

int consume_stack_item(ItemID item_id, int amount, bool auto_destroy)
{
    IF_ASSERT(item_id == NULL_ITEM) return 0;

    int stack_size = get_stack_size(item_id);
    GS_ASSERT(stack_size >= amount);
    if (auto_destroy && stack_size <= amount)
    {
        destroy_item(item_id);
        return 0;
    }

    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return 0;
    item->stack_size -= amount;
    IF_ASSERT(item->stack_size < 0) item->stack_size = 0;
    return item->stack_size;
}

// will destroy item if fully consumed
// returns remaining stack size
int consume_stack_item(ItemID item_id)
{
    return consume_stack_item(item_id, 1, true);
}

// will destroy item if fully consumed
// returns remaining stack size
int consume_stack_item(ItemID item_id, int amount)
{
    return consume_stack_item(item_id, amount, true);
}

// will destroy item if fully consumed
// return remaining durability
int consume_durability(ItemID item_id, int amount, bool auto_destroy)
{
    IF_ASSERT(item_id == NULL_ITEM) return 0;

    Item* item = get_item(item_id);
    IF_ASSERT(item == NULL) return 0;
    item->durability -= amount;
    item->durability = GS_MAX(0, item->durability);
    if (auto_destroy && item->durability <= 0)
    {
        destroy_item(item_id);
        return 0;
    }
    return item->durability;
}

// will destroy item if fully consumed
// returns remaining durability
int consume_durability(ItemID item_id)
{
    return consume_durability(item_id, 1, true);
}

// will destroy item if fully consumed
// returns remaining durability
int consume_durability(ItemID item_id, int amount)
{
    return consume_durability(item_id, amount, true);
}

void agent_quit(AgentID agent_id)
{
    Agents::Agent* agent = Agents::get_agent(agent_id);
    IF_ASSERT(agent == NULL) return;

    // remove client from item subscription lists
    // TODO -- reverse lookup from agent

    ClientID client_id = agent->client_id;
    for (size_t i=0; i<item_list->max; i++)
        if (item_list->objects[i].id != item_list->null_id)
            item_list->objects[i].subscribers.remove(client_id);
}

// tests
void test_item_list_capacity()
{
    printf("Testing item list capacity\n");
    for (int i=0; i<MAX_ITEMS+1024; i++)
        item_list->create_type((ItemType)0);
}

void tick()
{
    item_list->decay_gas();
    item_list->recharge_items();
    item_list->verify_items();
}

}   // Item

#endif
