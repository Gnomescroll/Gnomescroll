#include "_interface.hpp"

#include <common/crash_report/stack_trace.hpp>

#include <item/item.hpp>
#include <item/container/_state.hpp>
#include <item/particle/_interface.hpp>

#if DC_SERVER
#include <item/container/net/StoC.hpp>
#endif

#if DC_CLIENT
#include <item/container/client.hpp>
#endif

namespace ItemContainer
{

void init()
{
    item_container_list = new ItemContainerList;

    #if DC_SERVER
    agent_container_list   = (int*)   malloc(AGENT_MAX * sizeof(int));
    agent_toolbelt_list    = (int*)   malloc(AGENT_MAX * sizeof(int));
    agent_nanite_list      = (int*)   malloc(AGENT_MAX * sizeof(int));
    agent_hand_list        = (ItemID*)malloc(AGENT_MAX * sizeof(ItemID));
    opened_containers      = (int*)   malloc(AGENT_MAX * sizeof(int));
    
    for (int i=0; i<AGENT_MAX; i++) agent_container_list  [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_toolbelt_list   [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_nanite_list     [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_hand_list       [i] = NULL_ITEM;
    for (int i=0; i<AGENT_MAX; i++) opened_containers     [i] = NULL_CONTAINER;
    #endif
}

void teardown()
{
    if (item_container_list != NULL) delete item_container_list;

    #if DC_CLIENT
    if (player_container_ui   != NULL) delete player_container_ui;
    if (player_toolbelt_ui    != NULL) delete player_toolbelt_ui;
    if (player_nanite_ui      != NULL) delete player_nanite_ui;
    if (player_craft_bench_ui != NULL) delete player_craft_bench_ui;
    #endif

    #if DC_SERVER
    if (agent_container_list   != NULL) free(agent_container_list);
    if (agent_toolbelt_list    != NULL) free(agent_toolbelt_list);
    if (agent_nanite_list      != NULL) free(agent_nanite_list);
    if (agent_hand_list        != NULL) free(agent_hand_list);
    if (opened_containers      != NULL) free(opened_containers);
    #endif
}


ItemContainerInterface* get_container(int id)
{
    GS_ASSERT(item_container_list != NULL);
    return item_container_list->get(id);
}

void destroy_container(int id)
{
    ItemContainerInterface* container = get_container(id);
    if (container == NULL) return;
    
    #if DC_CLIENT
    // destroy contents
    for (int i=0; i<container->slot_max; i++)
    {
        if (container->slot[i] == NULL_ITEM) continue;
        Item::destroy_item(container->slot[i]);
    }
    #endif

    item_container_list->destroy(id);
}

ItemContainerType get_container_type(int container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return CONTAINER_TYPE_NONE;
    return container->type;
}

}   // ItemContainer
 
// Client
#if DC_CLIENT
namespace ItemContainer
{

ItemContainerInterface* create_container(ItemContainerType type, int id)
{
    return item_container_list->create(type, id);
}

void update_container_ui_from_state()
{
    if (player_container_ui != NULL) player_container_ui->load_data(player_container->slot);
    if (player_toolbelt_ui  != NULL) player_toolbelt_ui->load_data(player_toolbelt->slot);
    if (player_nanite_ui    != NULL) player_nanite_ui->load_data(player_nanite->slot);
    if (player_craft_bench_ui != NULL) player_craft_bench_ui->load_data(player_craft_bench->slot);
}

void open_inventory()
{
    // copy state to ui
    update_container_ui_from_state();
}

void close_inventory()
{
    // attempt throw
    mouse_left_click_handler(NULL_CONTAINER, NULL_SLOT, false, false);
}

void open_container(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);

    // setup UI widget
    // TODO -- handle multiple UI types 
    player_craft_bench = (ItemContainerCraftingBench*)get_container(container_id);
    if (player_craft_bench == NULL) return;
    if (player_craft_bench_ui != NULL) delete player_craft_bench_ui;
    player_craft_bench_ui = new ItemContainerUI(container_id);
    player_craft_bench_ui->init(player_craft_bench->type, player_craft_bench->xdim, player_craft_bench->ydim);
    player_craft_bench_ui->load_data(player_craft_bench->slot);
    t_hud::set_container_id(player_craft_bench->type, player_craft_bench->id);
    
    // assigned "opened_container" id
    if (opened_container == NULL_CONTAINER) was_opened = true;
    opened_container = container_id;

    // send open packet
    opened_container_event_id = record_container_event(container_id);
    open_container_CtoS msg;
    msg.container_id = container_id;
    msg.event_id = opened_container_event_id;
    msg.send();
}

void close_container()
{
    // attempt throw
    mouse_left_click_handler(NULL_CONTAINER, NULL_SLOT, false, false);

    if (opened_container == NULL_CONTAINER) return;

    // teardown UI widget
    // TODO -- handle multiple UI types
    player_craft_bench = NULL;
    if (player_craft_bench_ui != NULL) delete player_craft_bench_ui;
    player_craft_bench_ui = NULL;

    // unset hud container id
    t_hud::close_container(opened_container);

    if (opened_container != NULL_CONTAINER) was_closed = true;
    opened_container = NULL_CONTAINER;
    
    // send packet
    close_container_CtoS msg;
    msg.container_id = opened_container;
    msg.send();
}

bool container_was_opened()
{
    if (was_opened)
    {
        was_opened = false;
        return true;
    }
    return false;
}

bool container_was_closed()
{
    if (was_closed)
    {
        was_closed = false;
        return true;
    }
    return false;
}

int get_event_container_id(int event_id)
{
    GS_ASSERT(event_id >= 0 && event_id < CONTAINER_EVENT_MAX);
    return container_event[event_id];
}

ItemContainerUIInterface* get_container_ui(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (player_craft_bench_ui != NULL && player_craft_bench_ui->id == container_id) return player_craft_bench_ui;
    if (player_container_ui   != NULL && player_container_ui->id   == container_id) return player_container_ui;
    if (player_toolbelt_ui    != NULL && player_toolbelt_ui->id    == container_id) return player_toolbelt_ui;
    if (player_nanite_ui      != NULL && player_nanite_ui->id      == container_id) return player_nanite_ui;
    return NULL;
}

ItemID get_toolbelt_item(int slot)
{
    if (player_toolbelt == NULL) return NULL_ITEM;
    GS_ASSERT(player_toolbelt != NULL);
    GS_ASSERT(slot >= 0 && slot < player_toolbelt->xdim);
    return player_toolbelt->get_item(slot);
}

ItemID* get_container_contents(int container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return NULL;
    return container->slot;
}

int* get_container_ui_types(int container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_type;
}

int* get_container_ui_stacks(int container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_stack;
}

int* get_container_ui_durabilities(int container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_durability;
}

void set_ui_slot_durability(int container_id, int slot, int durability)
{
    if (slot == NULL_SLOT) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;
    int item_type = container->get_slot_type(slot);
    int item_stack = container->get_slot_stack(slot);
    container->insert_item(slot, item_type, item_stack, durability);
}

void set_ui_slot_stack_size(int container_id, int slot, int stack_size)
{
    if (slot == NULL_SLOT) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;
    int item_type = container->get_slot_type(slot);
    int item_durability = container->get_slot_durability(slot);
    container->insert_item(slot, item_type, stack_size, item_durability);
}

}   // ItemContainer
#endif 

// Server
#if DC_SERVER

namespace ItemContainer
{

bool agent_owns_container(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    if (container_id == NULL_CONTAINER) return false;
    if (agent_container_list[agent_id] == container_id) return true;
    if (agent_toolbelt_list[agent_id] == container_id) return true;
    if (agent_nanite_list[agent_id] == container_id) return true;
    return false;
}

bool agent_can_access_container(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    // owned by other player
    if (container->owner != NO_AGENT && container->owner != agent_id) return false;
    return true;
}

ItemID get_agent_toolbelt_item(int agent_id, int slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    int toolbelt_id = get_agent_toolbelt(agent_id);
    if (toolbelt_id == NULL_CONTAINER) return NULL_ITEM;
    ItemContainer* toolbelt = (ItemContainer*)get_container(toolbelt_id);
    if (toolbelt == NULL) return NULL_ITEM;
    return toolbelt->get_item(slot);
}

ItemID get_agent_hand(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(agent_hand_list != NULL);
    return agent_hand_list[agent_id];
}
    
int get_agent_container(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(agent_container_list != NULL);
    return agent_container_list[agent_id];
}

int get_agent_toolbelt(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(agent_toolbelt_list != NULL);
    return agent_toolbelt_list[agent_id];
}

ItemContainerInterface* create_container(ItemContainerType type)
{
    return item_container_list->create(type);
}

void assign_container_to_agent(ItemContainerInterface* container, int* container_list, int agent_id, int client_id)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(container_list[agent_id] == NULL_ITEM);
    container_list[agent_id] = container->id;
    container->assign_owner(agent_id);
    init_container(container);
    send_container_create(client_id, container->id);
    send_container_assign(client_id, container->id);
}

void assign_containers_to_agent(int agent_id, int client_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    
    ItemContainer* agent_container = (ItemContainer*)item_container_list->create(AGENT_CONTAINER);
    GS_ASSERT(agent_container != NULL);
    assign_container_to_agent(agent_container, agent_container_list, agent_id, client_id);
    
    ItemContainer* agent_toolbelt = (ItemContainer*)item_container_list->create(AGENT_TOOLBELT);
    GS_ASSERT(agent_toolbelt != NULL);
    assign_container_to_agent(agent_toolbelt, agent_toolbelt_list, agent_id, client_id);

    Item::Item* laser_rifle = Item::create_item(Item::get_item_type((char*)"laser_rifle"));
    GS_ASSERT(laser_rifle != NULL);
    auto_add_item_to_container(client_id, agent_toolbelt->id, laser_rifle->id);    // this will send the item create

    Item::Item* mining_laser = Item::create_item(Item::get_item_type((char*)"mining_laser"));
    GS_ASSERT(mining_laser != NULL);
    auto_add_item_to_container(client_id, agent_toolbelt->id, mining_laser->id);    // this will send the item create

    #if PRODUCTION
    //Item::Item* crate = Item::create_item(Item::get_item_type((char*)"crate_3"));
    //GS_ASSERT(crate != NULL);
    //auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    #else
    // put a grenade launcher in the toolbelt to selt
    Item::Item* grenade_launcher = Item::create_item(Item::get_item_type((char*)"grenade_launcher"));
    GS_ASSERT(grenade_launcher != NULL);
    auto_add_item_to_container(client_id, agent_toolbelt->id, grenade_launcher->id);    // this will send the item create

    // add a few container blocks
    Item::Item* crate;
    //crate = Item::create_item(Item::get_item_type((char*)"crate_1"));
    //auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    //crate = Item::create_item(Item::get_item_type((char*)"crate_2"));
    //auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    crate = Item::create_item(Item::get_item_type((char*)"crate_3"));
    GS_ASSERT(crate != NULL);
    auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    crate = Item::create_item(Item::get_item_type((char*)"crate_3"));
    GS_ASSERT(crate != NULL);
    auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    crate = Item::create_item(Item::get_item_type((char*)"crate_3"));
    GS_ASSERT(crate != NULL);
    auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    crate = Item::create_item(Item::get_item_type((char*)"crate_3"));
    GS_ASSERT(crate != NULL);
    auto_add_item_to_container(client_id, agent_toolbelt->id, crate->id);
    #endif

    #if !PRODUCTION
    // debug items
    Item::Item* block_placer = Item::create_item(Item::get_item_type((char*)"block_placer"));
    GS_ASSERT(block_placer != NULL);
    agent_toolbelt->insert_item(agent_toolbelt->slot_max-1, block_placer->id);
    send_container_item_create(client_id, block_placer->id, agent_toolbelt->id, agent_toolbelt->slot_max-1);

    Item::Item* location_pointer = Item::create_item(Item::get_item_type((char*)"location_pointer"));
    GS_ASSERT(location_pointer != NULL);
    agent_toolbelt->insert_item(agent_toolbelt->slot_max-2, location_pointer->id);
    send_container_item_create(client_id, location_pointer->id, agent_toolbelt->id, agent_toolbelt->slot_max-2);
    #endif
    
    ItemContainerNanite* agent_nanite = (ItemContainerNanite*)item_container_list->create(AGENT_NANITE);
    GS_ASSERT(agent_nanite != NULL);
    assign_container_to_agent(agent_nanite, agent_nanite_list, agent_id, client_id);
}

void agent_died(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    GS_ASSERT(a->status.dead);

    // remove items from agent inventory
    GS_ASSERT(agent_container_list != NULL);
    ItemContainer* container = (ItemContainer*)get_container(agent_container_list[agent_id]);
    if (container == NULL) return;
    for (int i=0; i<container->slot_max; i++)
    {
        ItemID item_id = container->slot[i];
        if (item_id == NULL_ITEM) continue;
        container->remove_item(i);
        send_container_remove(a->client_id, container->id, i);
        ItemParticle::throw_agent_item(agent_id, item_id);
    }

    // close container
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers[agent_id] != NULL_CONTAINER)
    {
        send_container_close(agent_id, opened_containers[agent_id]);
        agent_close_container(agent_id, opened_containers[agent_id]);
    }
}

void agent_quit(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    // destroy containers
    destroy_container(agent_container_list[agent_id]);
    destroy_container(agent_toolbelt_list[agent_id]);
    destroy_container(agent_nanite_list[agent_id]);
}

void digest_nanite_food()
{
    for (int i=0; i<AGENT_MAX; i++)
    {
        if (agent_nanite_list[i] == NULL_CONTAINER) continue;
        ItemContainerNanite* nanite = (ItemContainerNanite*)get_container(agent_nanite_list[i]);
        if (nanite == NULL) continue;
        nanite->digest();
    }
}

void purchase_item_from_nanite(int agent_id, int slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    GS_ASSERT(agent_nanite_list != NULL);
    GS_ASSERT(agent_hand_list != NULL);

    // if hand is not empty there will be item leaks
    if (agent_hand_list[agent_id] != NULL_ITEM) return;

    // get container
    if (agent_nanite_list[agent_id] == NULL_CONTAINER) return;
    ItemContainerNanite* nanite = (ItemContainerNanite*)get_container(agent_nanite_list[agent_id]);
    if (nanite == NULL) return;

    // transform plain slot to a shopping slot (as used by dat)
    slot = nanite->get_shopping_slot(slot);
    if (slot == NULL_SLOT) return;
    
    // get the store item
    int xslot = slot % nanite->xdim;
    int yslot = slot / nanite->xdim;
    int item_type, cost;
    Item::get_nanite_store_item(nanite->level, xslot, yslot, &item_type, &cost);
    GS_ASSERT(cost >= 0);
    if (item_type == NULL_ITEM_TYPE) return;
    
    // get the coins
    ItemID coins = nanite->get_coins();
    int coin_stack = 0; // coin stack will return 1 for NULL_ITEM, but we want to treat that as 0
    if (coins != NULL_ITEM) coin_stack = Item::get_stack_size(coins);
    if (coin_stack < cost) return;

    // get agent, for sending state to
    Agent_state* a = ServerState::agent_list->get(nanite->owner);

    // create shopped item
    Item::Item* purchase = Item::create_item(item_type);
    if (purchase == NULL) return;

    if (a != NULL) Item::send_item_create(a->client_id, purchase->id);
    // add to hand
    agent_hand_list[agent_id] = purchase->id;
    if (a != NULL) send_hand_insert(a->client_id, purchase->id);

    // update coins
    if (cost)
    {
        if (coin_stack == cost)
        {   // delete coins
            nanite->remove_item(nanite->slot_max-1);
            if (a != NULL) send_container_remove(a->client_id, nanite->id, nanite->slot_max-1);
            Item::destroy_item(coins);
        }
        else
        {   // decrement coin stack
            Item::Item* coin_item = Item::get_item_object(coins);
            GS_ASSERT(coin_item != NULL);
            coin_item->stack_size -= cost;
            if (a != NULL) Item::send_item_state(a->client_id, coins);
        }
    }
}

void craft_item_from_bench(int agent_id, int container_id, int craft_slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(agent_hand_list != NULL);

    Agent_state* agent = ServerState::agent_list->get(agent_id);
    if (agent == NULL) return;

    // agent does not own container, abort
    if (container_id != NULL_CONTAINER && !agent_can_access_container(agent->id, container_id)) return;

    Item::CraftingRecipe* recipe = Item::get_selected_craft_recipe(container_id, craft_slot);
    if (recipe == NULL) return;

    // hand is not empty and cannot stack the output
    ItemID hand_item = agent_hand_list[agent_id];
    bool hand_empty = (hand_item == NULL_ITEM);
    bool hand_can_stack_recipe = (Item::get_item_type(hand_item) == recipe->output && Item::get_stack_space(hand_item) >= 1);
    if (!hand_empty && !hand_can_stack_recipe) return;
        
    // remove reagents from container
    // deleting items as needed, modifying others
    bool consumed = consume_crafting_reagents(agent_id, container_id, recipe->id);
    if (!consumed) return;

    // place in hand
    if (hand_empty)
    {
        // create new item of type
        Item::Item* item = Item::create_item(recipe->output);
        if (item == NULL) return;
        Item::send_item_create(agent->client_id, item->id);
        agent_hand_list[agent_id] = item->id;
        send_hand_insert(agent->client_id, item->id);
    }
    else
    {
        // update item stack
        Item::Item* item = Item::get_item(hand_item);
        GS_ASSERT(item != NULL);
        item->stack_size += 1;
        Item::send_item_state(agent->client_id, item->id);
        send_hand_insert(agent->client_id, item->id);   // force client to update new hand state
    }
}

bool consume_crafting_reagents(int agent_id, int container_id, int recipe_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(recipe_id != NULL_CRAFTING_RECIPE);

    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(agent_hand_list != NULL);

    Agent_state* agent = ServerState::agent_list->get(agent_id);
    if (agent == NULL) return false;

    // agent does not own container, abort
    if (container_id != NULL_CONTAINER && !agent_can_access_container(agent->id, container_id)) return false;

    ItemContainerCraftingBench* bench = (ItemContainerCraftingBench*)get_container(container_id);
    GS_ASSERT(bench != NULL);

    Item::CraftingRecipe* recipe = Item::get_craft_recipe(recipe_id);
    GS_ASSERT(recipe != NULL);

    // assemble sorted bench inputs
    // we need them sorted so we can correctly decrement from each item
    int input_count = 0;
    ItemID inputs[bench->slot_max];
    int input_types[bench->slot_max];

    // clear initial state
    for (int i=0; i<bench->slot_max; inputs[i++] = NULL_ITEM);
    for (int i=0; i<bench->slot_max; input_types[i++] = NULL_ITEM_TYPE);

    // sort
    for (int i=0; i<bench->slot_max; i++)
    {
        ItemID item_id = bench->get_item(i);
        if (item_id == NULL_ITEM) continue;
        int item_type = Item::get_item_type(item_id);
        GS_ASSERT(item_type != NULL_ITEM_TYPE);

        // insert sorted
        if (input_count == 0)
        {   // degenerate case
            inputs[input_count] = item_id;
            input_types[input_count] = item_type;
        }
        else
        {   // find insertion point
            int j=0;
            for (; j<input_count; j++)
            {
                // comparison is on types
                if (input_types[j] <= item_type) continue;
                // shift forward
                for (int k=input_count; k>j; k--) inputs[k] = inputs[k-1];
                for (int k=input_count; k>j; k--) input_types[k] = input_types[k-1];
                // insert
                inputs[j] = item_id;
                input_types[j] = item_type;
                break;
            }
            
            // insert failed, append to end
            if (j == input_count)
            {
                inputs[j] = item_id;
                input_types[j] = item_type;
            }
        }
        input_count++;
    }

    // first verify we can craft with the reagents
    for (int i=0; i<recipe->reagent_num; i++)
    {   // remove reagents from inputs
        GS_ASSERT(recipe->reagent[i] != NULL_ITEM_TYPE);

        // gather recipe data
        int count = recipe->reagent_count[i];
        GS_ASSERT(count > 0);

        ItemID item_id = inputs[i];
        GS_ASSERT(item_id != NULL_ITEM);
        Item::Item* item = Item::get_item(item_id);
        GS_ASSERT(item != NULL);
        GS_ASSERT(recipe->reagent[i] == item->type);   // verifies sorting
        if (item->stack_size < count) return false; // cant craft
    }

    // now actually craft
    for (int i=0; i<recipe->reagent_num; i++)
    {   // remove reagents from inputs
        GS_ASSERT(recipe->reagent[i] != NULL_ITEM_TYPE);

        // gather recipe data
        int count = recipe->reagent_count[i];
        GS_ASSERT(count > 0);

        ItemID item_id = inputs[i];
        GS_ASSERT(item_id != NULL_ITEM);
        Item::Item* item = Item::get_item(item_id);
        GS_ASSERT(item != NULL);
        GS_ASSERT(recipe->reagent[i] == item->type);   // verifies sorting
        GS_ASSERT(item->stack_size >= count);

        // determine whether to decrement or fully remove item
        if (item->stack_size <= count)
        {   // remove this item
            Item::destroy_item(item->id);
        }
        else
        {   // decrement this item
            item->stack_size -= count;
            Item::send_item_state(agent->client_id, item->id);
        }
    }
    
    return true;
}

void send_container_contents(int agent_id, int client_id, int container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    if (!agent_can_access_container(agent_id, container_id)) return;
    
    for (int i=0; i<container->slot_max; i++)
    {
        if (container->slot[i] == NULL_ITEM) continue;
        Item::send_item_create(client_id, container->slot[i]);
        send_container_insert(client_id, container->slot[i], container->id, i);
    }
}

void container_block_destroyed(int container_id, int x, int y, int z)
{
    GS_ASSERT(container_id != NULL_CONTAINER);

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    // close all opened containers
    GS_ASSERT(opened_containers != NULL);
    for (int i=0; i<AGENT_MAX; i++)
        if (opened_containers[i] == container_id)
            opened_containers[i] = NULL_CONTAINER;

    // queue the container delete packet first
    // the handler will destroy the contents -- then the item_particle create will recreate
    broadcast_container_delete(container_id);

    // dump contents
    for (int i=0; i<container->slot_max; i++)
    {
        if (container->slot[i] == NULL_ITEM) continue;
        ItemParticle::dump_container_item(container->slot[i], x,y,z);
        container->remove_item(i);
        // no need to send container removal packet
    }

    // destroy container
    destroy_container(container_id);
}

bool agent_in_container_range(int agent_id, int container_id)
{
    // get agent position
    ASSERT_VALID_AGENT_ID(agent_id);
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return false;

    Vec3 agent_position = a->get_center();

    // get container position, if applicable
    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (!Item::container_type_is_block(container->type)) return false;

    int position[3];
    t_map::get_container_location(container->id, position);
    Vec3 container_position = vec3_init(position[0], position[1], position[2]);
    container_position = vec3_add(container_position, vec3_init(0.5f, 0.5f, 0.5f));
    
    // do radius check
    if (vec3_distance_squared(agent_position, container_position)
        <= AGENT_CONTAINER_REACH*AGENT_CONTAINER_REACH) return true;
    return false;
}

// check that agents are still in range of containers they are accessing
void check_agents_in_container_range()
{
    using ServerState::agent_list;
    for (int i=0; i<agent_list->n_max; i++)
    {
        if (agent_list->a[i] == NULL) continue;
        Agent_state* a = agent_list->a[i];
        if (opened_containers[a->id] == NULL_CONTAINER) continue;
        if (agent_in_container_range(a->id, opened_containers[a->id])) continue;
        agent_close_container(a->id, opened_containers[a->id]);
        send_container_close(a->id, opened_containers[a->id]);
    }
}

int auto_add_item_to_container(int client_id, int container_id, ItemID item_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);

    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);

    int slot = container->get_stackable_slot(item->type, item->stack_size);
    if (slot == NULL_SLOT)
    {   // insert normal
        slot = container->get_empty_slot();
        if (slot == NULL_SLOT) return NULL_SLOT;
        container->insert_item(slot, item_id);
        send_container_item_create(client_id, item_id, container_id, slot);
    }
    else
    {   // stack
        ItemID slot_item_id = container->get_item(slot);
        GS_ASSERT(slot_item_id != NULL_ITEM);
        Item::merge_item_stack(item_id, slot_item_id);
        Item::send_item_state(client_id, slot_item_id);
        send_container_insert(client_id, slot_item_id, container_id, slot);
    }
    return slot;
}

//tests
void test_container_list_capacity()
{
    for (int i=0; i<ITEM_CONTAINER_MAX*2; i++)
        item_container_list->create(AGENT_CONTAINER);
}

}   // ItemContainer

#endif 
