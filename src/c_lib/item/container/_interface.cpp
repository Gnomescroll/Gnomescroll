#include "_interface.hpp"

#include <common/crash_report/stack_trace.hpp>

#include <item/item.hpp>
#include <item/container/_state.hpp>
#include <item/particle/_interface.hpp>

#include <item/container/config/_interface.hpp>

#if DC_SERVER
#include <item/container/net/StoC.hpp>
#include <serializer/_interface.hpp>
#endif

#if DC_CLIENT
#include <item/container/client.hpp>
#endif

#include <item/container/net/StoC.hpp>
#include <item/container/net/CtoS.hpp>


namespace ItemContainer
{

void init()
{
    init_config();

    item_container_list = new ItemContainerList;

    #if DC_SERVER
    agent_inventory_list    = (int*) malloc(AGENT_MAX * sizeof(int));
    agent_toolbelt_list     = (int*) malloc(AGENT_MAX * sizeof(int));
    agent_synthesizer_list  = (int*) malloc(AGENT_MAX * sizeof(int));
    agent_energy_tanks_list = (int*) malloc(AGENT_MAX * sizeof(int));
    opened_containers       = (int*) malloc(AGENT_MAX * sizeof(int));
    agent_hand_list         = (int*) malloc(AGENT_MAX * sizeof(int));
    
    for (int i=0; i<AGENT_MAX; i++) agent_inventory_list   [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_toolbelt_list    [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_synthesizer_list [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_energy_tanks_list[i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) opened_containers      [i] = NULL_CONTAINER;
    for (int i=0; i<AGENT_MAX; i++) agent_hand_list        [i] = NULL_CONTAINER;
    #endif
    
    #if DC_CLIENT
    GS_ASSERT(container_event == NULL);
    container_event = (int*)malloc(CONTAINER_EVENT_MAX * sizeof(int));
    for (int i=0; i<CONTAINER_EVENT_MAX; container_event[i++] = NULL_CONTAINER);
    #endif
}

void teardown()
{
    if (item_container_list != NULL) delete item_container_list;

    #if DC_CLIENT
    if (player_container_ui    != NULL) delete player_container_ui;
    if (player_hand_ui         != NULL) delete player_hand_ui;
    if (player_toolbelt_ui     != NULL) delete player_toolbelt_ui;
    if (player_synthesizer_ui  != NULL) delete player_synthesizer_ui;
    if (player_energy_tanks_ui != NULL) delete player_energy_tanks_ui;
    if (player_craft_bench_ui  != NULL) delete player_craft_bench_ui;
    if (storage_block_ui       != NULL) delete storage_block_ui;
    
    if (container_event != NULL) free(container_event);
    #endif

    #if DC_SERVER
    if (agent_inventory_list    != NULL) free(agent_inventory_list);
    if (agent_toolbelt_list     != NULL) free(agent_toolbelt_list);
    if (agent_synthesizer_list  != NULL) free(agent_synthesizer_list);
    if (agent_energy_tanks_list != NULL) free(agent_energy_tanks_list);
    if (agent_hand_list         != NULL) free(agent_hand_list);
    if (opened_containers       != NULL) free(opened_containers);
    #endif
    
    teardown_config();
}

void init_packets()
{
    // container state
    create_item_container_StoC::register_client_packet();
    delete_item_container_StoC::register_client_packet();
    assign_item_container_StoC::register_client_packet();

    // container actions to server
    open_container_CtoS::register_server_packet();
    close_container_CtoS::register_server_packet();

    // container actions to client
    open_container_failed_StoC::register_client_packet();
    close_container_StoC::register_client_packet();
    open_container_StoC::register_client_packet();

    // container transactions to server
    container_action_alpha_CtoS::register_server_packet();
    container_action_beta_CtoS::register_server_packet();

    synthesizer_container_action_alpha_CtoS::register_server_packet();
    synthesizer_container_action_beta_CtoS::register_server_packet();
    purchase_item_from_synthesizer_action_CtoS::register_server_packet();
    
    craft_container_action_alpha_CtoS::register_server_packet();
    craft_container_action_beta_CtoS::register_server_packet();
    craft_item_from_bench_action_CtoS::register_server_packet();

    no_container_action_alpha_CtoS::register_server_packet();
    no_container_action_beta_CtoS::register_server_packet();

    smelter_container_action_alpha_CtoS::register_server_packet();
    smelter_container_action_beta_CtoS::register_server_packet();

    crusher_container_action_alpha_CtoS::register_server_packet();
    crusher_container_action_beta_CtoS::register_server_packet();
    crusher_crush_item_CtoS::register_server_packet();

    // container transactions to client
    container_action_failed_StoC::register_client_packet();
    insert_item_in_container_StoC::register_client_packet();
    remove_item_from_container_StoC::register_client_packet();
    insert_item_in_hand_StoC::register_client_packet();
    remove_item_from_hand_StoC::register_client_packet();

    // locks
    lock_container_StoC::register_client_packet();
    unlock_container_StoC::register_client_packet();

    // container blocks
    create_container_block_CtoS::register_server_packet();
    admin_create_container_block_CtoS::register_server_packet();

    // smelter
    smelter_fuel_StoC::register_client_packet();
    smelter_progress_StoC::register_client_packet();
}


ItemContainerInterface* get_container(int id)
{
    GS_ASSERT(item_container_list != NULL);
    return item_container_list->get(id);
}

void destroy_container(int id)
{
    GS_ASSERT(id != NULL_CONTAINER);
    if (id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(id);
    if (container == NULL) return;
    
    #if DC_CLIENT
    // close it, if we had it open
    close_container(container->id);

    // destroy contents
    for (int i=0; i<container->slot_max; i++)
        if (container->slot[i] != NULL_ITEM)
            Item::destroy_item(container->slot[i]);
    #endif

    #if DC_SERVER
    // close container if anyone is accessing it
    for (int i=0; i<AGENT_MAX; i++)
        if (opened_containers[i] == id)
            agent_close_container(i, opened_containers[i]);
    #endif

    item_container_list->destroy(id);
}

ItemContainerType get_container_type(int container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return CONTAINER_TYPE_NONE;
    return container->type;
}

int get_container_owner(int container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return NULL_AGENT;
    return container->owner;
}

void container_block_destroyed(int container_id, int x, int y, int z)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    #if DC_SERVER
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
        ItemID item_id = container->slot[i];
        container->remove_item(i);
        ItemParticle::dump_container_item(item_id, x,y,z);
        // no need to send container removal packet
    }
    #endif

    #if DC_CLIENT
    close_container(container_id);
    #endif

    // destroy container
    destroy_container(container_id);
}

bool container_block_in_range_of(Vec3 pos, int block[3])
{
    Vec3 container_position = vec3_init(block[0], block[1], block[2]);
    container_position = vec3_add(container_position, vec3_init(0.5f, 0.5f, 0.5f));
    container_position = translate_position(container_position);
    container_position = quadrant_translate_position(pos, container_position);
    if (vec3_distance_squared(pos, container_position)
        <= AGENT_CONTAINER_REACH*AGENT_CONTAINER_REACH) return true;
    return false;
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
    if (player_container_ui    != NULL) player_container_ui    ->load_data (player_container    ->slot);
    if (player_hand_ui         != NULL) player_hand_ui         ->load_data (player_hand         ->slot);
    if (player_toolbelt_ui     != NULL) player_toolbelt_ui     ->load_data (player_toolbelt     ->slot);
    if (player_synthesizer_ui  != NULL) player_synthesizer_ui  ->load_data (player_synthesizer  ->slot);
    if (player_energy_tanks_ui != NULL) player_energy_tanks_ui ->load_data (player_energy_tanks ->slot);
    if (player_craft_bench_ui  != NULL) player_craft_bench_ui  ->load_data (player_craft_bench  ->slot);
    if (storage_block_ui       != NULL) storage_block_ui       ->load_data (storage_block       ->slot);
    if (cryofreezer_ui         != NULL) cryofreezer_ui         ->load_data (cryofreezer         ->slot);
    if (crusher_ui             != NULL) crusher_ui             ->load_data (crusher             ->slot);
    if (smelter_ui             != NULL) smelter_ui             ->load_data (smelter             ->slot);
}

void update_smelter_ui()
{
    if (smelter_ui == NULL) return;
    smelter_ui->tick_fuel();
    smelter_ui->tick_progress();
}

void open_inventory()
{   // copy state to ui
    update_container_ui_from_state();
}

void close_inventory()
{   // attempt throw
    mouse_left_click_handler(NULL_CONTAINER, NULL_SLOT, false);
}

bool open_container(int container_id)
{
    GS_ASSERT(!input_state.agent_inventory);    // check that agent container is not opened here
    
    GS_ASSERT(container_id != NULL_CONTAINER);

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return false;
    GS_ASSERT(container->type != CONTAINER_TYPE_NONE);
    if (!container->can_be_opened_by(ClientState::playerAgent_state.agent_id)) return false;

    GS_ASSERT(opened_container == NULL_CONTAINER);
    
    // setup UI widget
    switch (container->type)
    {
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            GS_ASSERT(storage_block == NULL);
            GS_ASSERT(storage_block_ui == NULL);
            GS_ASSERT(cryofreezer == NULL);
            GS_ASSERT(cryofreezer_ui == NULL);
            GS_ASSERT(smelter == NULL);
            GS_ASSERT(smelter_ui == NULL);
            GS_ASSERT(crusher_ui == NULL);

            player_craft_bench = (ItemContainerCraftingBench*)container;
            if (player_craft_bench == NULL) return false;
            // setup ui
            if (player_craft_bench_ui != NULL) delete player_craft_bench_ui;
            player_craft_bench_ui = new ItemContainerUI(container_id);
            player_craft_bench_ui->set_alt_parameters(player_craft_bench->alt_xdim, player_craft_bench->alt_ydim);
            player_craft_bench_ui->init(player_craft_bench->type, player_craft_bench->xdim, player_craft_bench->ydim);
            player_craft_bench_ui->load_data(player_craft_bench->slot);
            HudContainer::set_container_id(player_craft_bench->type, player_craft_bench->id);
            did_open_container_block = true;
            break;

        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
            GS_ASSERT(player_craft_bench == NULL);
            GS_ASSERT(player_craft_bench_ui == NULL);
            GS_ASSERT(cryofreezer == NULL);
            GS_ASSERT(cryofreezer_ui == NULL);
            GS_ASSERT(smelter == NULL);
            GS_ASSERT(smelter_ui == NULL);
            GS_ASSERT(crusher_ui == NULL);

            storage_block = (ItemContainer*)container;
            if (storage_block == NULL) return false;
            // setup ui
            if (storage_block_ui == NULL) delete storage_block_ui;
            storage_block_ui = new ItemContainerUI(container_id);
            storage_block_ui->set_alt_parameters(storage_block->alt_xdim, storage_block->alt_ydim);
            storage_block_ui->init(storage_block->type, storage_block->xdim, storage_block->ydim);
            storage_block_ui->load_data(storage_block->slot);
            HudContainer::set_container_id(storage_block->type, storage_block->id);
            did_open_container_block = true;
            break;

        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
            GS_ASSERT(player_craft_bench == NULL);
            GS_ASSERT(player_craft_bench_ui == NULL);
            GS_ASSERT(storage_block == NULL);
            GS_ASSERT(storage_block_ui == NULL);
            GS_ASSERT(smelter == NULL);
            GS_ASSERT(smelter_ui == NULL);
            GS_ASSERT(crusher_ui == NULL);
            
            cryofreezer = (ItemContainerCryofreezer*)container;
            if (cryofreezer == NULL) return false;
            // setup ui
            if (cryofreezer_ui == NULL) delete cryofreezer_ui;
            cryofreezer_ui = new ItemContainerUI(container_id);
            cryofreezer_ui->set_alt_parameters(cryofreezer->alt_xdim, cryofreezer->alt_ydim);
            cryofreezer_ui->init(cryofreezer->type, cryofreezer->xdim, cryofreezer->ydim);
            cryofreezer_ui->load_data(cryofreezer->slot);
            HudContainer::set_container_id(cryofreezer->type, cryofreezer->id);
            did_open_container_block = true;
            break;
            
        case CONTAINER_TYPE_SMELTER_ONE:
            GS_ASSERT(player_craft_bench == NULL);
            GS_ASSERT(player_craft_bench_ui == NULL);
            GS_ASSERT(storage_block == NULL);
            GS_ASSERT(storage_block_ui == NULL);
            GS_ASSERT(cryofreezer == NULL);
            GS_ASSERT(cryofreezer_ui == NULL);
            GS_ASSERT(crusher_ui == NULL);
            
            smelter = (ItemContainerSmelter*)container;
            if (smelter == NULL) return false;
            // setup ui
            if (smelter_ui == NULL) delete smelter_ui;
            smelter_ui = new ItemContainerSmelterUI(container_id);
            smelter_ui->set_alt_parameters(smelter->alt_xdim, smelter->alt_ydim);
            smelter_ui->init(smelter->type, smelter->xdim, smelter->ydim);
            smelter_ui->load_data(smelter->slot);
            HudContainer::set_container_id(smelter->type, smelter->id);
            did_open_container_block = true;
            break;

        case CONTAINER_TYPE_CRUSHER:
            GS_ASSERT(player_craft_bench == NULL);
            GS_ASSERT(player_craft_bench_ui == NULL);
            GS_ASSERT(storage_block == NULL);
            GS_ASSERT(storage_block_ui == NULL);
            GS_ASSERT(cryofreezer == NULL);
            GS_ASSERT(cryofreezer_ui == NULL);
            GS_ASSERT(smelter_ui == NULL);

            crusher =(ItemContainerCrusher*)container;
            if (crusher == NULL) return false;
            if (crusher_ui != NULL) delete crusher_ui;
            crusher_ui = new ItemContainerCrusherUI(container_id);
            crusher_ui->set_alt_parameters(crusher->alt_xdim, crusher->alt_ydim);
            crusher_ui->init(crusher->type, crusher->xdim, crusher->ydim);
            crusher_ui->load_data(crusher->slot);
            HudContainer::set_container_id(crusher->type, crusher->id);
            did_open_container_block = true;
            break;


        default:
            GS_ASSERT(false);
            return false;
    }

    if (did_open_container_block)
        did_close_container_block = false;

    opened_container = container_id;

    // send open packet
    opened_container_event_id = record_container_event(container_id);
    if (opened_container_event_id >= 0)
        send_container_open(container_id, opened_container_event_id);
    return true;
}

bool close_container(int container_id)
{    
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;
    if (container_id != opened_container) return false;

    // attempt throw
    mouse_left_click_handler(NULL_CONTAINER, NULL_SLOT, false);

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    // clear the contents, for public containers
    if (container != NULL && !container->attached_to_agent)
        container->clear();

    // teardown UI widget
    // TODO -- handle multiple UI types
    player_craft_bench = NULL;
    if (player_craft_bench_ui != NULL) delete player_craft_bench_ui;
    player_craft_bench_ui = NULL;

    storage_block = NULL;
    if (storage_block_ui != NULL) delete storage_block_ui;
    storage_block_ui = NULL;

    cryofreezer = NULL;
    if (cryofreezer_ui != NULL) delete cryofreezer_ui;
    cryofreezer_ui = NULL;

    smelter = NULL;
    if (smelter_ui != NULL) delete smelter_ui;
    smelter_ui = NULL;

    crusher = NULL;
    if (crusher_ui != NULL) delete crusher_ui;
    crusher_ui = NULL;

    // unset hud container id
    HudContainer::close_container(container_id);

    //print_trace();
    opened_container = NULL_CONTAINER;
    did_open_container_block = false;
    did_close_container_block = true;

    return true;
}

bool container_block_was_opened(int* container_id)
{
    GS_ASSERT(container_id != NULL);
    if (container_id == NULL) return false;

    if (did_open_container_block)
    {
        GS_ASSERT(opened_container != NULL_CONTAINER);
        did_open_container_block = false;
        *container_id = opened_container;
        return true;
    }
    *container_id = NULL_CONTAINER;
    return false;
}

bool container_block_was_closed()
{
    if (did_close_container_block)
    {
        did_close_container_block = false;
        return true;
    }
    return false;
}

int get_event_container_id(int event_id)
{
    GS_ASSERT(event_id >= 0 && event_id < CONTAINER_EVENT_MAX);
    GS_ASSERT(container_event != NULL);
    if (container_event == NULL) return NULL_CONTAINER;
    return container_event[event_id];
}

ItemContainerUIInterface* get_container_ui(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (player_craft_bench_ui  != NULL && player_craft_bench_ui->id  == container_id) return player_craft_bench_ui;
    if (player_hand_ui         != NULL && player_hand_ui->id         == container_id) return player_hand_ui;
    if (player_container_ui    != NULL && player_container_ui->id    == container_id) return player_container_ui;
    if (player_energy_tanks_ui != NULL && player_energy_tanks_ui->id == container_id) return player_energy_tanks_ui;
    if (player_toolbelt_ui     != NULL && player_toolbelt_ui->id     == container_id) return player_toolbelt_ui;
    if (player_synthesizer_ui  != NULL && player_synthesizer_ui->id  == container_id) return player_synthesizer_ui;
    if (storage_block_ui       != NULL && storage_block_ui->id       == container_id) return storage_block_ui;
    if (cryofreezer_ui         != NULL && cryofreezer_ui->id         == container_id) return cryofreezer_ui;
    if (crusher_ui             != NULL && crusher_ui->id             == container_id) return crusher_ui;
    if (smelter_ui             != NULL && smelter_ui->id             == container_id) return smelter_ui;
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

int get_container_ui_slot_max(int container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return 0;
    return container->slot_max;
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

bool agent_can_access_container(int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return false;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    return container->can_be_opened_by(agent_id);
}

ItemID get_agent_toolbelt_item(int agent_id, int slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return NULL_ITEM;
    int toolbelt_id = get_agent_toolbelt(agent_id);
    if (toolbelt_id == NULL_CONTAINER) return NULL_ITEM;
    ItemContainer* toolbelt = (ItemContainer*)get_container(toolbelt_id);
    if (toolbelt == NULL) return NULL_ITEM;
    return toolbelt->get_item(slot);
}

int get_agent_hand(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return NULL_CONTAINER;
    GS_ASSERT(agent_hand_list != NULL);
    if (agent_hand_list == NULL) return NULL_CONTAINER;
    return agent_hand_list[agent_id];
}

ItemID get_agent_hand_item(int agent_id)
{
    int hand_id = get_agent_hand(agent_id);
    GS_ASSERT(hand_id != NULL_CONTAINER);
    if (hand_id == NULL_CONTAINER) return NULL_ITEM;
    ItemContainerHand* hand = (ItemContainerHand*)get_container(hand_id);
    GS_ASSERT(hand != NULL);
    if (hand == NULL) return NULL_ITEM;
    return hand->get_item();
}
    
int get_agent_inventory(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return NULL_CONTAINER;
    GS_ASSERT(agent_inventory_list != NULL);
    if (agent_inventory_list == NULL) return NULL_CONTAINER;
    return agent_inventory_list[agent_id];
}

int get_agent_toolbelt(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return NULL_CONTAINER;
    GS_ASSERT(agent_toolbelt_list != NULL);
    if (agent_toolbelt_list == NULL) return NULL_CONTAINER;
    return agent_toolbelt_list[agent_id];
}

int get_agent_synthesizer(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return NULL_CONTAINER;
    GS_ASSERT(agent_synthesizer_list != NULL);
    if (agent_synthesizer_list == NULL) return NULL_CONTAINER;
    return agent_synthesizer_list[agent_id];
}

int get_agent_energy_tanks(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return NULL_CONTAINER;
    GS_ASSERT(agent_energy_tanks_list != NULL);
    if (agent_energy_tanks_list == NULL) return NULL_CONTAINER;
    return agent_energy_tanks_list[agent_id];
}

int* get_player_containers(int agent_id, int* n_containers)
{
    static int containers[N_PLAYER_CONTAINERS] = {NULL_CONTAINER};
    int n = 0;
    containers[n] = get_agent_hand(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_inventory(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_toolbelt(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_synthesizer(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_energy_tanks(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;

    *n_containers = n;
    return containers;
}


ItemContainerInterface* create_container(ItemContainerType type)
{
    return item_container_list->create(type);
}

static void assign_container_to_agent(ItemContainerInterface* container, int* container_list, int agent_id, int client_id)
{
    GS_ASSERT(container != NULL);
    GS_ASSERT(container_list[agent_id] == NULL_ITEM);
    if (container == NULL) return;
    container_list[agent_id] = container->id;
    init_container(container);
    container->assign_owner(agent_id);
    send_container_create(client_id, container->id);
    send_container_assign(client_id, container->id);

    if (Options::serializer)
        serializer::load_player_container(client_id, container->id);
}

void assign_containers_to_agent(int agent_id, int client_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return;
    
    ItemContainer* agent_inventory = (ItemContainer*)item_container_list->create(AGENT_INVENTORY);
    GS_ASSERT(agent_inventory != NULL);
    assign_container_to_agent(agent_inventory, agent_inventory_list, agent_id, client_id);

    ItemContainerHand* agent_hand = (ItemContainerHand*)item_container_list->create(AGENT_HAND);
    GS_ASSERT(agent_hand != NULL);
    assign_container_to_agent(agent_hand, agent_hand_list, agent_id, client_id);

    ItemContainerEnergyTanks* agent_energy_tanks = (ItemContainerEnergyTanks*)item_container_list->create(AGENT_ENERGY_TANKS);
    GS_ASSERT(agent_energy_tanks != NULL);
    assign_container_to_agent(agent_energy_tanks, agent_energy_tanks_list, agent_id, client_id);
        
    ItemContainerSynthesizer* agent_synthesizer = (ItemContainerSynthesizer*)item_container_list->create(AGENT_SYNTHESIZER);
    GS_ASSERT(agent_synthesizer != NULL);
    assign_container_to_agent(agent_synthesizer, agent_synthesizer_list, agent_id, client_id);

    ItemContainer* agent_toolbelt = (ItemContainer*)item_container_list->create(AGENT_TOOLBELT);
    GS_ASSERT(agent_toolbelt != NULL);
    assign_container_to_agent(agent_toolbelt, agent_toolbelt_list, agent_id, client_id);
    // toolbelt contents are added by agent_born
}

static void throw_items_from_container(int client_id, int agent_id, int container_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    GS_ASSERT(container_id != NULL_CONTAINER);
    ItemContainer* container = (ItemContainer*)get_container(container_id);
    if (container == NULL) return;
    for (int i=0; i<container->slot_max; i++)
    {
        ItemID item_id = container->slot[i];
        if (item_id == NULL_ITEM) continue;
        container->remove_item(i);
        send_container_remove(client_id, container->id, i);
        ItemParticle::throw_agent_item(agent_id, item_id);
    }
}

void agent_born(int agent_id)
{
    // TODO -- restore agent_born
        // must wait for full deserialization, then we can proceed with this stuff
    
    return;
    
    // refill toolbelt if needed

    // toolbelt should have at least 1 maxed out laser rifle
    // and 1 maxed out mining_laser

    // if we have one or more of either not maxed out, fill up the most full one

    // if we have neither and there is only one slot, add the laser rifle (because fist can replace mining_laser)

    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;

    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    int client_id = a->client_id;
    
    GS_ASSERT(agent_toolbelt_list != NULL);
    if (agent_toolbelt_list == NULL) return;

    GS_ASSERT(agent_toolbelt_list[agent_id] != NULL_CONTAINER);
    if (agent_toolbelt_list[agent_id] == NULL_CONTAINER) return;
    ItemContainerInterface* toolbelt = get_container(agent_toolbelt_list[agent_id]);
    GS_ASSERT(toolbelt != NULL);
    if (toolbelt == NULL) return;

    // collect information on mining_laser / laser rifle contents

    int toolbelt_space = toolbelt->slot_max - toolbelt->slot_count;

    int laser_rifle_type = Item::get_item_type("laser_rifle");
    GS_ASSERT(laser_rifle_type != NULL_ITEM_TYPE);
    bool has_laser_rifle = false;
    Item::Item* most_durable_laser_rifle = NULL;
    int most_durable_laser_rifle_value = 0;

    int shovel_type = Item::get_item_type("copper_shovel");
    GS_ASSERT(shovel_type != NULL_ITEM_TYPE);
    bool has_shovel = false;
    Item::Item* most_durable_shovel = NULL;
    int most_durable_shovel_value = 0;

    GS_ASSERT(toolbelt->slot_max > 0);
    if (toolbelt->slot_max <= 0) return;
    for (int i=toolbelt->slot_max-1; i>=0; i--)
    {
        ItemID item_id = toolbelt->slot[i];
        if (item_id == NULL_ITEM) continue;
        Item::Item* item = Item::get_item(item_id);
        GS_ASSERT(item != NULL);
        if (item == NULL) continue;

        if (item->type == laser_rifle_type)
        {
            has_laser_rifle = true;
            GS_ASSERT(item->durability > 0 && item->durability != NULL_DURABILITY);
            if (item->durability >= most_durable_laser_rifle_value)
            {
                most_durable_laser_rifle = item;
                most_durable_laser_rifle_value = item->durability;
            }
        }

        if (item->type == shovel_type)
        {
            has_shovel = true;
            GS_ASSERT(item->durability > 0 && item->durability != NULL_DURABILITY);
            if (item->durability >= most_durable_shovel_value)
            {
                most_durable_shovel = item;
                most_durable_shovel_value = item->durability;
            }
        }
    }

    // decide what to add/ modify

    if (has_laser_rifle)
    {   // max out the durability
        GS_ASSERT(most_durable_laser_rifle != NULL);
        if (most_durable_laser_rifle != NULL)
        {
            most_durable_laser_rifle->durability = Item::get_max_durability(laser_rifle_type);
            Item::send_item_state(most_durable_laser_rifle->id);
        }
    }
    else
    {   // add a laser rifle
        if (toolbelt_space > 0)
        {
            Item::Item* laser_rifle = Item::create_item(laser_rifle_type);
            GS_ASSERT(laser_rifle != NULL);
            ContainerActionType event = CONTAINER_ACTION_NONE;
            if (laser_rifle != NULL)
                event = auto_add_free_item_to_container(client_id, toolbelt->id, laser_rifle->id);    // this will send the item create
            GS_ASSERT(event != CONTAINER_ACTION_NONE && event != PARTIAL_WORLD_TO_OCCUPIED_SLOT);
            toolbelt_space--;
        }
    }

    if (has_shovel)
    {   // max out the durability
        GS_ASSERT(most_durable_shovel != NULL);
        if (most_durable_shovel != NULL)
        {
            most_durable_shovel->durability = Item::get_max_durability(shovel_type);
            Item::send_item_state(most_durable_shovel->id);
        }
    }
    else
    {   // add a shovel
        if (toolbelt_space > 0)
        {
            Item::Item* shovel = Item::create_item(shovel_type);
            GS_ASSERT(shovel != NULL);
            ContainerActionType event = CONTAINER_ACTION_NONE;
            if (shovel != NULL)
                event = auto_add_free_item_to_container(client_id, toolbelt->id, shovel->id);    // this will send the item create
            GS_ASSERT(event != CONTAINER_ACTION_NONE && event != PARTIAL_WORLD_TO_OCCUPIED_SLOT);
            toolbelt_space--;
        }
    }

    // add energy tanks 
    int energy_tanks_id = get_agent_energy_tanks(agent_id);
    GS_ASSERT(energy_tanks_id != NULL_CONTAINER);
    ItemContainerEnergyTanks* energy_tanks = (ItemContainerEnergyTanks*)get_container(energy_tanks_id);
    GS_ASSERT(energy_tanks != NULL);
    if (energy_tanks != NULL)
    {
        #if PRODUCTION
        int n_energy_tanks = 1;
        if (energy_tanks->slot_count == energy_tanks->slot_max) n_energy_tanks = 0;
        #else
        int n_energy_tanks = energy_tanks->slot_max - energy_tanks->slot_count - 1;
        #endif
        for (int i=0; i<n_energy_tanks; i++)
        {
            int energy_tank_type = Item::get_item_type("energy_tank");
            GS_ASSERT(energy_tank_type != NULL_ITEM_TYPE);
            if (energy_tank_type == NULL_ITEM_TYPE) break;
            Item::Item* energy_tank = Item::create_item(energy_tank_type);
            GS_ASSERT(energy_tank != NULL);
            if (energy_tank == NULL) break;
            
            int slot = energy_tanks->get_empty_slot();
            if (slot == NULL_SLOT) break;

            bool added = transfer_free_item_to_container(energy_tank->id, energy_tanks->id, slot);
            if (!added) break;
        }
    }

    #if !PRODUCTION
    // fill coins to max
    int synth_id = get_agent_synthesizer(agent_id);
    GS_ASSERT(synth_id != NULL_CONTAINER);
    ItemContainerSynthesizer* synth = (ItemContainerSynthesizer*)get_container(synth_id);
    GS_ASSERT(synth != NULL);
    if (synth != NULL)
    {
        ItemID known_coins = synth->get_coins();
        if (known_coins == NULL_ITEM)
        {   // create new coins             
            Item::Item* coins = Item::create_item("synthesizer_coin");
            GS_ASSERT(coins != NULL);
            if (coins != NULL)
            {
                coins->stack_size = Item::get_max_stack_size(coins->type);
                transfer_free_item_to_container(coins->id, synth->id, synth->coins_slot);
            }       
        }
        else
        {   // update known coins
            Item::Item* coins = Item::get_item(known_coins);
            GS_ASSERT(coins != NULL);
            int stack_max = Item::get_max_stack_size(coins->type);
            GS_ASSERT(coins->stack_size <= stack_max);
            if (coins != NULL && coins->stack_size != stack_max)
            {
                coins->stack_size = stack_max;
                Item::send_item_state(coins->id);
            }
        }
    }

    ContainerActionType event = CONTAINER_ACTION_NONE;

    // put a grenade launcher in the toolbelt
    Item::Item* plasma_grenade = Item::create_item(Item::get_item_type("plasma_grenade"));
    GS_ASSERT(plasma_grenade != NULL);
    if (plasma_grenade != NULL)
    {
        plasma_grenade->stack_size = 100;
        event = auto_add_free_item_to_container(client_id, toolbelt->id, plasma_grenade->id);    // this will send the item create
        if (event == CONTAINER_ACTION_NONE || event == PARTIAL_WORLD_TO_OCCUPIED_SLOT) Item::destroy_item(plasma_grenade->id);
        else if (plasma_grenade->stack_size <= 0) Item::destroy_item(plasma_grenade->id);
    }

    int location_pointer_slot = toolbelt->slot_max-1;
    if (toolbelt->get_item(location_pointer_slot) == NULL_ITEM)
    {
        Item::Item* location_pointer = Item::create_item(Item::get_item_type((char*)"location_pointer"));
        GS_ASSERT(location_pointer != NULL);
        if (location_pointer != NULL)
        {
            bool added = transfer_free_item_to_container(location_pointer->id, toolbelt->id, location_pointer_slot);
            if (!added) Item::destroy_item(location_pointer->id);
        }
    }
    #endif

}

void agent_died(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    GS_ASSERT(a->status.dead);

    // throw agent inventory items
    GS_ASSERT(agent_inventory_list != NULL);
    if (agent_inventory_list != NULL && agent_inventory_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(a->client_id, a->id, agent_inventory_list[agent_id]);

    // close container
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers != NULL && opened_containers[agent_id] != NULL_CONTAINER)
        send_container_close(agent_id, opened_containers[agent_id]);

    // throw any items in hand. the agent_close_container will have thrown anything, if a free container was open
    // DO NOT throw_items_from_container(agent_hand), we need to use the transactional method for hand throwing
    ItemID hand_item = get_agent_hand_item(agent_id);
    if (hand_item != NULL_ITEM)
        transfer_hand_to_particle(agent_id);    // throw
    GS_ASSERT(get_agent_hand_item(agent_id) == NULL_ITEM);
}

static void save_agent_containers(int client_id, int agent_id, bool remove_items_after)
{
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    
    serializer::save_player_container(client_id, agent_inventory_list[agent_id], remove_items_after);
    serializer::save_player_container(client_id, agent_synthesizer_list[agent_id], remove_items_after);
    serializer::save_player_container(client_id, agent_toolbelt_list[agent_id], remove_items_after);
    serializer::save_player_container(client_id, agent_energy_tanks_list[agent_id], remove_items_after);
    serializer::save_player_container(client_id, agent_hand_list[agent_id], remove_items_after);
}

void dump_agent_containers(int client_id, int agent_id)
{
    ASSERT_VALID_CLIENT_ID(client_id);
    IF_INVALID_CLIENT_ID(client_id) return;
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    
    if (agent_inventory_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_inventory_list[agent_id]);

    if (agent_synthesizer_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_synthesizer_list[agent_id]);

    if (agent_toolbelt_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_toolbelt_list[agent_id]);

    if (agent_energy_tanks_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_energy_tanks_list[agent_id]);

    // DO NOT throw_items_from_container(agent_hand), we need to use the transactional method for hand throwing
    ItemID hand_item = get_agent_hand_item(agent_id);
    if (hand_item != NULL_ITEM)
        transfer_hand_to_particle(agent_id);    // throw
}

void agent_quit(int agent_id)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;

    // close opened free container (this will throw any items sitting in hand)
    GS_ASSERT(opened_containers != NULL);
    if (opened_containers != NULL && opened_containers[agent_id] != NULL_CONTAINER)
        agent_close_container(agent_id, opened_containers[agent_id]);

    GS_ASSERT(opened_containers != NULL && opened_containers[agent_id] == NULL_CONTAINER);
    if (opened_containers != NULL) opened_containers[agent_id] = NULL_CONTAINER;

    // still have to throw any items in hand, in case we have our private inventory opened
    ItemID hand_item = get_agent_hand_item(agent_id);
    if (hand_item != NULL_ITEM)
        transfer_hand_to_particle(agent_id);    // throw
    GS_ASSERT(get_agent_hand_item(agent_id) == NULL_ITEM);

    if (Options::serializer)
        save_agent_containers(a->client_id, a->id, true);    // remove items after
    else
        dump_agent_containers(a->client_id, a->id);

    // TODO -- dont destroy the container contents in this case


    // destroy containers
    if (agent_inventory_list[agent_id] != NULL_CONTAINER)
        destroy_container(agent_inventory_list[agent_id]);

    if (agent_toolbelt_list[agent_id] != NULL_CONTAINER)
        destroy_container(agent_toolbelt_list[agent_id]);
        
    if (agent_synthesizer_list[agent_id] != NULL_CONTAINER)
        destroy_container(agent_synthesizer_list[agent_id]);

    if (agent_energy_tanks_list[agent_id] != NULL_CONTAINER)
        destroy_container(agent_energy_tanks_list[agent_id]);

    if (agent_hand_list[agent_id] != NULL_CONTAINER)
        destroy_container(agent_hand_list[agent_id]);

    agent_inventory_list[agent_id] = NULL_CONTAINER;
    agent_toolbelt_list[agent_id] = NULL_CONTAINER;
    agent_synthesizer_list[agent_id] = NULL_CONTAINER;
    agent_energy_tanks_list[agent_id] = NULL_CONTAINER;
    agent_hand_list[agent_id] = NULL_CONTAINER;
}

void purchase_item_from_synthesizer(int agent_id, int shopping_slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return;
    
    // get container
    GS_ASSERT(agent_synthesizer_list != NULL);
    if (agent_synthesizer_list == NULL) return;
    if (agent_synthesizer_list[agent_id] == NULL_CONTAINER) return;
    ItemContainerSynthesizer* synthesizer = (ItemContainerSynthesizer*)get_container(agent_synthesizer_list[agent_id]);
    if (synthesizer == NULL) return;
    GS_ASSERT(agent_can_access_container(agent_id, synthesizer->id));
    if (!agent_can_access_container(agent_id, synthesizer->id)) return;

    // get the store item
    int xslot = shopping_slot % synthesizer->alt_xdim;
    int yslot = shopping_slot / synthesizer->alt_xdim;
    int cost;
    int item_type = Item::get_synthesizer_item(xslot, yslot, &cost);
    GS_ASSERT(cost >= 0);
    if (item_type == NULL_ITEM_TYPE) return;
    
    // compare to hand
    ItemID hand_item = get_agent_hand_item(agent_id);
    int hand_item_type = Item::get_item_type(hand_item);
    if (hand_item != NULL_ITEM)
    {   
        // can it stack
        if (hand_item_type != item_type) return;
        
        // will it fit
        int stack_space = Item::get_stack_space(hand_item);
        if (stack_space <= 0) return;
    }
    
    // get the coins
    ItemID coins = synthesizer->get_coins();
    int coin_stack = 0; // coin stack will return 1 for NULL_ITEM, but we want to treat that as 0
    if (coins != NULL_ITEM) coin_stack = Item::get_stack_size(coins);

    // can we afford it
    if (coin_stack < cost) return;
    
    Item::Item* coin_item = Item::get_item_object(coins);
    GS_ASSERT(coin_item != NULL);
    if (coin_item == NULL) return;

    if (hand_item == NULL_ITEM)
    {
        // create shopped item
        Item::Item* purchase = Item::create_item(item_type);
        GS_ASSERT(purchase != NULL);
        if (purchase == NULL) return;

        // add to hand
        transfer_free_item_to_hand(purchase->id, agent_id);
    }
    else
    {
        // get hand item pointer
        Item::Item* hand = Item::get_item(hand_item);
        GS_ASSERT(hand != NULL);
        if (hand == NULL) return;
        
        // increase count by 1
        hand->stack_size += 1;
        GS_ASSERT(hand->stack_size <= Item::get_max_stack_size(hand->type));
        Item::send_item_state(hand->id);
    }

    // update coins
    if (cost)
    {
        if (coin_stack == cost)
        {   // delete coins
            Item::destroy_item(coins);
        }
        else
        {   // decrement coin stack
            coin_item->stack_size -= cost;
            Item::send_item_state(coins);
        }
    }
}

void craft_item_from_bench(int agent_id, int container_id, int craft_slot)
{
    ASSERT_VALID_AGENT_ID(agent_id);

    Agent_state* agent = ServerState::agent_list->get(agent_id);
    if (agent == NULL) return;

    // agent does not own container, abort
    if (container_id != NULL_CONTAINER && !agent_can_access_container(agent->id, container_id)) return;

    Item::CraftingRecipe* recipe = Item::get_selected_craft_recipe(container_id, craft_slot);
    if (recipe == NULL) return;

    // hand is not empty and cannot stack the output
    ItemID hand_item = get_agent_hand_item(agent_id);
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
        item->stack_size = recipe->output_stack;
        transfer_free_item_to_hand(item->id, agent_id);
    }
    else if (hand_can_stack_recipe)
    {
        // update item stack
        Item::Item* item = Item::get_item(hand_item);
        GS_ASSERT(item != NULL);
        if (item == NULL) return;
        item->stack_size += recipe->output_stack;
        Item::send_item_state(item->id);
    }
}

bool consume_crafting_reagents(int agent_id, int container_id, int recipe_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(recipe_id != NULL_CRAFTING_RECIPE);

    ASSERT_VALID_AGENT_ID(agent_id);

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
    //ItemID inputs[bench->slot_max];
    //int input_types[bench->slot_max];
    MALLOX(ItemID, inputs, bench->slot_max); //type, name, size
    MALLOX(int, input_types, bench->slot_max); //type, name, size

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
            Item::send_item_state(item->id);
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
        Item::subscribe_agent_to_item(agent_id, container->slot[i]);
        send_container_insert(client_id, container->slot[i], container->id, i);
    }
}

bool agent_in_container_range(int agent_id, int container_id)
{
    // get agent position
    ASSERT_VALID_AGENT_ID(agent_id);
    IF_INVALID_AGENT_ID(agent_id) return false;
    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return false;

    Vec3 agent_position = a->get_camera_position();

    // get container position, if applicable
    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return false;
    GS_ASSERT(container_type_is_block(container->type));
    if (!container_type_is_block(container->type)) return false;

    int block[3];
    t_map::get_container_location(container->id, block);
    
    return container_block_in_range_of(agent_position, block);
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
    }
}

// for brand new items going to a container
ContainerActionType auto_add_free_item_to_container(int client_id, int container_id, ItemID item_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);

    Item::Item* item = Item::get_item(item_id);
    GS_ASSERT(item != NULL);
    if (item == NULL) return CONTAINER_ACTION_NONE;
    GS_ASSERT(item->stack_size > 0);
    GS_ASSERT(item->type != NULL_ITEM_TYPE);

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return CONTAINER_ACTION_NONE;

    int slot = container->get_stackable_slot(item->type, item->stack_size);
    if (slot == NULL_SLOT)
    {   // no easy stack slot found
        // try to put in empty slot
        slot = container->get_empty_slot();
        if (slot == NULL_SLOT)
        {   // no empty slot found, try to break this stack up and redistribute
            int starting_stack_size = item->stack_size;
            int stack_size = item->stack_size;
            int item_type = item->type;

            // distribute as much as we can into slots
            for (int i=0; i<container->slot_max; i++)
            {
                GS_ASSERT(stack_size > 0);
                if (container->slot[i] == NULL_ITEM) continue;
                Item::Item* slot_item = Item::get_item(container->slot[i]);
                GS_ASSERT(slot_item != NULL);
                if (slot_item == NULL) continue;
                if (slot_item->type != item_type) continue;
                int stack_space = Item::get_stack_space(slot_item->id);
                if (stack_space == 0) continue;

                if (stack_space >= stack_size)
                {   // full, final merge
                    Item::merge_item_stack(item->id, slot_item->id);
                    stack_size = 0;
                }
                else
                {
                    Item::merge_item_stack(item->id, slot_item->id, stack_space);
                    Item::send_item_state(slot_item->id);
                    stack_size -= stack_space;
                    GS_ASSERT(stack_size > 0);
                }
                if (stack_size <= 0) break;
            }
            
            if (stack_size <= 0)
            {
                Item::destroy_item(item_id);
                return FULL_WORLD_TO_OCCUPIED_SLOT;
            }
            else if (starting_stack_size != stack_size)
            {   // source item was only partially consumed
                // WARNING: Caller of this function must send state update!!
                return PARTIAL_WORLD_TO_OCCUPIED_SLOT; 
            }
        }
        else
        {   // empty slot found, put it there
            transfer_free_item_to_container(item_id, container_id, slot);
            return FULL_WORLD_TO_EMPTY_SLOT;
        }
    }
    else
    {   // stack
        ItemID slot_item_id = container->get_item(slot);
        GS_ASSERT(slot_item_id != NULL_ITEM);
        Item::merge_item_stack(item_id, slot_item_id);
        Item::destroy_item(item_id);
        Item::send_item_state(slot_item_id);
        return FULL_WORLD_TO_OCCUPIED_SLOT;
    }
    
    return CONTAINER_ACTION_NONE;
}

void update_smelters()
{
    for (int i=0; i<item_container_list->n_max; i++)
    {
        if (item_container_list->a[i] == NULL) continue;
        ItemContainerInterface* container = item_container_list->a[i];
        if (!Item::is_smelter(container->type)) continue;
        ItemContainerSmelter* smelter = (ItemContainerSmelter*)container;

        // if fuel is 0
            // if fuel slot is empty (or not fuel -- invalid state)
                // reset progress
                // send progress packet
            // else
                // assert fuel slot item is fuel
                // consume 1 fuel stack
                // set fuel_type to type,meter to 1.0f
                // send fuel packet to subscribers

        // else
            // look up recipe
            // if progress is 0
                // if recipe is non null
                    // set recipe type
                    // increment progress by amount prescribed by recipe
                    // send progress packet
            // else
                // if recipe matches
                    // increment progress by amount according to recipe
                // else
                    // reset progress
                    // send progress packet

        // if progress is 1.0f
            // consume inputs according to recipe
            // create output(s), put in output slots

        GS_ASSERT(smelter->fuel >= 0);
        GS_ASSERT(smelter->progress >= 0);
        GS_ASSERT(smelter->recipe_id != NULL_ITEM_TYPE || smelter->progress == 0)

        Item::SmeltingRecipe* recipe = NULL;
        int recipe_id = NULL_SMELTING_RECIPE;
        
        if (smelter->fuel <= 0)
        {
            smelter->fuel = 0;
            ItemID fuel_item = smelter->get_fuel();
            int fuel_item_type = NULL_ITEM_TYPE;
            bool is_fuel = false;
            if (fuel_item != NULL_ITEM)
            {
                fuel_item_type = Item::get_item_type(fuel_item);
                is_fuel = Item::is_fuel(fuel_item_type);
            }
            GS_ASSERT(fuel_item_type == NULL_ITEM_TYPE || is_fuel);

            // ERROR STATE -- throw item out of here
            if (fuel_item_type != NULL_ITEM_TYPE && !is_fuel)
            {
                Item::Item* fuel = Item::get_item(fuel_item);
                GS_ASSERT(fuel != NULL);
                if (fuel == NULL)
                {   // we have an erroneous value in here
                    // maybe a buffer overwrite
                    // just get rid of it
                    smelter->remove_fuel();
                }
                else
                {   // an existing item was placed in here somehow
                    // still could be buffer overwrite
                    // look where the item is located
                    // if its located here, throw() it
                    // else, just unset it
                    bool is_here =
                        (fuel->location == IL_CONTAINER
                      && fuel->location_id == smelter->id
                      && fuel->container_slot == smelter->fuel_slot);
                    GS_ASSERT(is_here);
                    if (is_here)
                    {   // throw it out
                        int p[3];
                        t_map::get_container_location(smelter->id, p);
                        Vec3 pos = vec3_init(p[0], p[1], p[2]);
                        pos = vec3_add(pos, vec3_init(0.5f, 0.5f, 1.05f));
                        ItemParticle::dump_container_item(fuel_item,
                            pos.x, pos.y, pos.z);
                    }
                    else
                    {   // erroneous state
                        //smelter->remove_fuel();   // Can't use this, because it modifies the item's state on the way out. the item probably isnt here
                        if (smelter->is_valid_slot(smelter->fuel_slot))
                        {
                            smelter->slot[smelter->fuel_slot] = NULL_ITEM;
                            smelter->slot_count--;
                        }
                    }
                }
                
                // clear fuel info
                fuel_item_type = NULL_ITEM_TYPE;
                fuel_item = NULL_ITEM;
            }

            if (fuel_item_type == NULL_ITEM_TYPE || !is_fuel)
            {   // reset progress
                smelter->reset_smelting();
            }
            else
            {   // consume fuel item stack
                if (smelter->can_produce_output())
                {
                    int remaining = Item::consume_stack_item(fuel_item);
                    if (remaining > 0) Item::send_item_state(fuel_item);
                    smelter->fill_fuel(fuel_item_type);
                }
            }
        }
        else
        {   // burn fuel, advance smelting
            smelter->burn_fuel();
            if (smelter->can_produce_output(&recipe, &recipe_id))
            {
                GS_ASSERT(recipe != NULL);
                GS_ASSERT(recipe_id != NULL_SMELTING_RECIPE);
                if (recipe_id != smelter->recipe_id)
                    smelter->reset_smelting();
                if (smelter->progress <= 0)
                    smelter->begin_smelting(recipe_id);
                else if (recipe_id == smelter->recipe_id)
                    smelter->tick_smelting();
            }
            else
                smelter->reset_smelting();
            GS_ASSERT(smelter->progress <= 0 || recipe != NULL);
        }

        if (smelter->progress >= 1.0f)
        {
            GS_ASSERT(recipe != NULL);
            if (recipe == NULL) continue;
            // consume recipe inputs
            unsigned int max_inputs = smelter->get_max_input_slots();
            MALLOX(ItemID, inputs, max_inputs); //type, name, size
            
            int n_inputs = smelter->get_sorted_inputs(inputs, max_inputs);    // sorted by type

            GS_ASSERT(recipe->reagent_num <= n_inputs);
            if (recipe->reagent_num > n_inputs) continue;
            for (int i=0; i<recipe->reagent_num; i++)
            {
                GS_ASSERT(recipe->reagent[i] != NULL_ITEM_TYPE);
                if (recipe->reagent[i] == NULL_ITEM_TYPE) continue;
                int reagent = recipe->reagent[i];
                int reagent_stack = recipe->reagent_count[i];

                ItemID input = inputs[i];
                GS_ASSERT(input != NULL_ITEM);
                if (input == NULL_ITEM) continue;
                int input_type = Item::get_item_type(input);
                GS_ASSERT(input_type != NULL_ITEM_TYPE);
                GS_ASSERT(input_type == reagent);
                int input_stack = Item::get_stack_size(input);
                GS_ASSERT(input_stack >= reagent_stack);
                if (input_stack < reagent_stack) reagent_stack = input_stack;   // cap it
                int remaining = Item::consume_stack_item(input, reagent_stack);
                if (remaining > 0) Item::send_item_state(input);
            }

            // create outputs
            for (int i=0; i<recipe->output_num; i++)
            {
                GS_ASSERT(recipe->output[i] != NULL_ITEM_TYPE);
                if (recipe->output[i] == NULL_ITEM_TYPE) continue;

                int slot = smelter->convert_product_slot(i);
                ItemID item_id = smelter->get_item(slot);
                if (item_id == NULL_ITEM)
                {   // item does not exist, create
                    Item::Item* product = Item::create_item(recipe->output[i]);
                    GS_ASSERT(product != NULL);
                    if (product == NULL) continue;
                    product->stack_size = recipe->output_stack[i];
                    GS_ASSERT(product->stack_size > 0);
                    transfer_free_item_to_container(product->id, smelter->id, slot);
                }
                else
                {   // item exists, increment
                    Item::Item* item = Item::get_item(item_id);
                    GS_ASSERT(item != NULL);
                    if (item == NULL) continue;
                    GS_ASSERT(item->type == recipe->output[i]);
                    if (item->type != recipe->output[i]) continue;
                    item->stack_size += recipe->output_stack[i];
                    GS_ASSERT(recipe->output_stack[i] >= 1);
                    int max_stack_size = Item::get_max_stack_size(item->type);
                    GS_ASSERT(item->stack_size <= max_stack_size);
                    if (item->stack_size > max_stack_size)
                        item->stack_size = max_stack_size;
                    Item::send_item_state(item->id);
                }
            }
            smelter->reset_smelting();
        }
    }
}

// used by serializer; places an item into a container based on the item's location information
// returns false on error
bool load_item_into_container(ItemID item_id, int container_id, int container_slot)
{
    return transfer_free_item_to_container(item_id, container_id, container_slot);
}

bool load_item_into_hand(ItemID item_id, int agent_id)
{
    return transfer_free_item_to_hand(item_id, agent_id);
}

//tests
void test_container_list_capacity()
{
    for (int i=0; i<ITEM_CONTAINER_MAX*2; i++)
        item_container_list->create(AGENT_INVENTORY);
}

}   // ItemContainer

#endif 
