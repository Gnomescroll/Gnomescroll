#include "_interface.hpp"


#include <item/item_container.hpp>
#include <item/item.hpp>
#include <item/_state.hpp>
#include <item/particle/_interface.hpp>

#if DC_SERVER
#include <item/net/StoC.hpp>
#endif

#if DC_CLIENT
#include <item/client.hpp>
#endif

namespace Item
{

void init()
{
    item_container_list = new ItemContainerList;
    item_list           = new ItemList;

    //set_sprite_ids();

    #if DC_SERVER
    agent_container_list = (int*)malloc(AGENT_MAX * sizeof(int));
    agent_toolbelt_list  = (int*)malloc(AGENT_MAX * sizeof(int));
    agent_nanite_list    = (int*)malloc(AGENT_MAX * sizeof(int));
    agent_hand_list      = (ItemID*)malloc(AGENT_MAX * sizeof(ItemID));
    for (int i=0; i<AGENT_MAX; i++) agent_container_list[i] = NULL_ITEM;
    for (int i=0; i<AGENT_MAX; i++) agent_toolbelt_list [i] = NULL_ITEM;
    for (int i=0; i<AGENT_MAX; i++) agent_nanite_list   [i] = NULL_ITEM;
    for (int i=0; i<AGENT_MAX; i++) agent_hand_list     [i] = NULL_ITEM;
    #endif

    init_properties();
}

void teardown()
{
    if (item_container_list != NULL) delete item_container_list;
    if (item_list           != NULL) delete item_list;

    #if DC_CLIENT
    if (player_container_ui != NULL) delete player_container_ui;
    if (player_toolbelt_ui  != NULL) delete player_toolbelt_ui;
    if (player_nanite_ui    != NULL) delete player_nanite_ui;
    #endif
    
    #if DC_SERVER
    if (agent_container_list != NULL) free(agent_container_list);
    if (agent_toolbelt_list  != NULL) free(agent_toolbelt_list);
    if (agent_nanite_list    != NULL) free(agent_nanite_list);
    if (agent_hand_list      != NULL) free(agent_hand_list);
    #endif

    tear_down_properties();
}


ItemContainer* get_container(int id)
{
    assert(item_container_list != NULL);
    return item_container_list->get(id);
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

int get_item_group(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return IG_ERROR;
    return item->group;
}

int get_stack_size(ItemID id)
{   // space used in a stack
    if (id == NULL_ITEM) return 1;
    Item* item = get_item(id);
    assert(item != NULL);
    return item->stack_size;
}

int get_stack_space(ItemID id)
{   // space left in a stack
    if (id == NULL_ITEM) return 0;
    Item* item = get_item(id);
    assert(item != NULL);
    int stack_space = get_max_stack_size(item->type) - item->stack_size;
    assert(stack_space >= 0);
    return stack_space;
}

int get_item_durability(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return NULL_DURABILITY;
    return item->durability;
}

void destroy_item(ItemID id)
{
    Item* item = get_item(id);
    if (item == NULL) return;
    int container_id = item->container_id;
    int slot = item->container_slot;
    ItemContainer* container = get_container(container_id);
    if (container != NULL && slot != NULL_SLOT)
    {
        container->remove_item(slot);
        #if DC_SERVER
        Agent_state* a = STATE::agent_list->get(container->owner);
        if (a != NULL) send_container_remove(container->owner, container_id, slot);
        #endif
    }
    item_list->destroy(id);
}

void merge_item_stack(ItemID src, ItemID dest)
{
    assert(src != NULL_ITEM);
    assert(dest != NULL_ITEM);

    Item* src_item = get_item(src);
    assert(src_item != NULL);
    Item* dest_item = get_item(dest);
    assert(dest_item != NULL);

    // add src's stack to dest
    dest_item->stack_size += src_item->stack_size;
    assert(dest_item->stack_size <= get_max_stack_size(dest_item->type));
}

void merge_item_stack(ItemID src, ItemID dest, int amount)
{
    assert(src != NULL_ITEM);
    assert(dest != NULL_ITEM);
    assert(amount > 0);
    
    Item* src_item = get_item(src);
    assert(src_item != NULL);
    Item* dest_item = get_item(dest);
    assert(dest_item != NULL);

    // add src's stack to dest
    dest_item->stack_size += amount;
    assert(dest_item->stack_size <= get_max_stack_size(dest_item->type));
    
    // remove from src
    src_item->stack_size -= amount;
    assert(src_item->stack_size >= 1);
}

}
 
// Client
#if DC_CLIENT
namespace Item
{

void update_container_ui_from_state()
{
    if (player_container_ui != NULL) player_container_ui->load_data(player_container->slot);
    if (player_toolbelt_ui  != NULL) player_toolbelt_ui->load_data(player_toolbelt->slot);
    if (player_nanite_ui    != NULL) player_nanite_ui->load_data(player_nanite->slot);
}

void open_container()
{
    // copy state to ui
    if (player_container == NULL) return;
    if (player_container_ui == NULL) return;
    player_container_ui->load_data(player_container->slot);
}

void close_container()
{
    // attempt throw
    mouse_left_click_handler(NULL_CONTAINER, NULL_SLOT);
}


int get_event_container_id(int event_id)
{
    assert(event_id >= 0 && event_id < CONTAINER_EVENT_MAX);
    return container_event[event_id];
}

ItemContainerUI* get_container_ui(int container_id)
{
    assert(container_id != NULL_CONTAINER);
    if (player_container_ui != NULL && player_container_ui->id == container_id) return player_container_ui;
    if (player_toolbelt_ui  != NULL && player_toolbelt_ui->id  == container_id) return player_toolbelt_ui;
    if (player_nanite_ui    != NULL && player_nanite_ui->id    == container_id) return player_nanite_ui;
    return NULL;
}

ItemID get_toolbelt_item(int slot)
{
    assert(player_toolbelt != NULL);
    assert(slot >= 0 && slot < player_toolbelt->xdim);
    return player_toolbelt->get_item(slot);
}

Item* create_item(int item_type, ItemID item_id)
{
    return item_list->create_type(item_type, item_id);
}

ItemID* get_container_contents(int container_id)
{
    ItemContainer* container = item_container_list->get(container_id);
    if (container == NULL) return NULL;
    return container->slot;
}

int* get_container_ui_types(int container_id)
{
    ItemContainerUI* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_type;
}

int* get_container_ui_stacks(int container_id)
{
    ItemContainerUI* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_stack;
}

int* get_container_ui_durabilities(int container_id)
{
    ItemContainerUI* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_durability;
}

void set_ui_slot_durability(int container_id, int slot, int durability)
{
    if (slot == NULL_SLOT) return;
    ItemContainerUI* container = get_container_ui(container_id);
    if (container == NULL) return;
    int item_type = container->get_slot_type(slot);
    int item_stack = container->get_slot_stack(slot);
    container->insert_item(slot, item_type, item_stack, durability);
}

}
#endif 

// Server
#if DC_SERVER

namespace Item
{

ItemID split_item_stack(ItemID src, int amount)
{
    assert(src != NULL_ITEM);
    assert(amount >= 1);

    Item* src_item = get_item(src);
    assert(src_item != NULL);
    src_item->stack_size -= amount;
    assert(src_item->stack_size >= 1);

    Item* new_item = create_item(src_item->type);
    new_item->stack_size = amount;
    return new_item->id;
}

ItemID split_item_stack_in_half(ItemID src)
{
    assert(src != NULL_ITEM);

    Item* src_item = get_item(src);
    assert(src_item != NULL);
    int split_amount = src_item->stack_size / 2;
    assert(split_amount >= 1);  // Do not call this function for a stack with only 1 (cannot split)
    src_item->stack_size -= split_amount;

    Item* new_item = create_item(src_item->type);
    new_item->stack_size = split_amount;
    return new_item->id;
}
bool agent_owns_container(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    if (container_id == NULL_CONTAINER) return false;
    if (agent_container_list[agent_id] == container_id) return true;
    if (agent_toolbelt_list[agent_id] == container_id) return true;
    if (agent_nanite_list[agent_id] == container_id) return true;
    return false;
}

ItemID get_agent_toolbelt_item(int agent_id, int slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    int toolbelt_id = get_agent_toolbelt(agent_id);
    if (toolbelt_id == NULL_CONTAINER) return NULL_ITEM;
    ItemContainer* toolbelt = item_container_list->get(toolbelt_id);
    if (toolbelt == NULL) return NULL_ITEM;
    return toolbelt->get_item(slot);
}

ItemID get_agent_hand(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(agent_hand_list != NULL);
    return agent_hand_list[agent_id];
}
    
int get_agent_container(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(agent_container_list != NULL);
    return agent_container_list[agent_id];
}

int get_agent_toolbelt(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    assert(agent_toolbelt_list != NULL);
    return agent_toolbelt_list[agent_id];
}

static void assign_container_to_agent(ItemContainer* container, ItemContainerType type, int* container_list, int agent_id, int client_id)
{
    assert(container != NULL);
    assert(container_list[agent_id] == NULL_ITEM);
    container_list[agent_id] = container->id;
    container->assign_owner(agent_id);
    init_container(container, type);
    send_container_create(container, client_id);
    send_container_assign(container, client_id);
}

void assign_containers_to_agent(int agent_id, int client_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    
    ItemContainer* agent_container = item_container_list->create();
    assign_container_to_agent(agent_container, AGENT_CONTAINER, agent_container_list, agent_id, client_id);
    
    ItemContainer* agent_toolbelt = item_container_list->create();
    assign_container_to_agent(agent_toolbelt, AGENT_TOOLBELT, agent_toolbelt_list, agent_id, client_id);

    // put a grenade launcher in the toolbelt to selt
    Item* grenade_launcher = create_item(8);
    grenade_launcher->energy = get_max_energy(grenade_launcher->type);
    auto_add_item_to_container(client_id, agent_toolbelt->id, grenade_launcher->id);    // this will send the item create

    Item* laser_rifle = create_item(5);
    laser_rifle->energy = get_max_energy(laser_rifle->type);
    auto_add_item_to_container(client_id, agent_toolbelt->id, laser_rifle->id);    // this will send the item create

    Item* mining_laser = create_item(7);
    auto_add_item_to_container(client_id, agent_toolbelt->id, mining_laser->id);    // this will send the item create
    
    ItemContainer* agent_nanite = item_container_list->create();
    assign_container_to_agent(agent_nanite, AGENT_NANITE, agent_nanite_list, agent_id, client_id);
}

Item* create_item(int item_type)
{
    return item_list->create_type(item_type);
}

void agent_died(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    assert(a->status.dead);

    // remove items from agent inventory
    assert(agent_container_list != NULL);
    ItemContainer* container = get_container(agent_container_list[agent_id]);
    if (container == NULL) return;
    for (int i=0; i<container->slot_max; i++)
    {
        ItemID item_id = container->slot[i];
        if (item_id == NULL_ITEM) continue;
        container->remove_item(i);
        ItemParticle::throw_item(agent_id, item_id);
    }
}


}

#endif 
