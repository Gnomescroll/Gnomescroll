#include "_interface.hpp"

#include <common/crash_report/stack_trace.hpp>
#include <agent/_interface.hpp>
#include <item/item.hpp>
#include <item/container/_state.hpp>
#include <item/particle/_interface.hpp>
#include <item/container/config/_interface.hpp>
#include <item/container/net/StoC.hpp>
#include <item/container/net/CtoS.hpp>

#if DC_SERVER
# include <item/container/net/StoC.hpp>
# include <serializer/_interface.hpp>
#endif

#if DC_CLIENT
# include <item/container/client.hpp>
#endif

namespace ItemContainer
{

void init()
{
    GS_ASSERT(item_container_list == NULL);
    item_container_list = new ItemContainerList(MAX_CONTAINERS);

    size_t i = 0;
    player_container_types[i++] = name::hand;
    player_container_types[i++] = name::toolbelt;
    player_container_types[i++] = name::inventory;
    player_container_types[i++] = name::synthesizer;
    player_container_types[i++] = name::energy_tanks;
    player_container_types[i++] = name::equipment;
    #if !PRODUCTION
    // TODO -- enable premium_cache
    player_container_types[i++] = name::premium_cache;
    #endif
    GS_ASSERT(i == N_PLAYER_CONTAINERS);

    #if DC_SERVER
    agent_inventory_list    = (ItemContainerID*) malloc(MAX_AGENTS * sizeof(ItemContainerID));
    agent_toolbelt_list     = (ItemContainerID*) malloc(MAX_AGENTS * sizeof(ItemContainerID));
    agent_synthesizer_list  = (ItemContainerID*) malloc(MAX_AGENTS * sizeof(ItemContainerID));
    agent_energy_tanks_list = (ItemContainerID*) malloc(MAX_AGENTS * sizeof(ItemContainerID));
    agent_equipment_list    = (ItemContainerID*) malloc(MAX_AGENTS * sizeof(ItemContainerID));
    premium_cache_list      = (ItemContainerID*) malloc(MAX_AGENTS * sizeof(ItemContainerID));
    opened_containers       = (ItemContainerID*) malloc(MAX_AGENTS * sizeof(ItemContainerID));
    agent_hand_list         = (ItemContainerID*) malloc(MAX_AGENTS * sizeof(ItemContainerID));

    for (int i=0; i<MAX_AGENTS; i++)
    {
        agent_inventory_list   [i] = NULL_CONTAINER;
        agent_toolbelt_list    [i] = NULL_CONTAINER;
        agent_synthesizer_list [i] = NULL_CONTAINER;
        agent_energy_tanks_list[i] = NULL_CONTAINER;
        agent_equipment_list   [i] = NULL_CONTAINER;
        premium_cache_list     [i] = NULL_CONTAINER;
        opened_containers      [i] = NULL_CONTAINER;
        agent_hand_list        [i] = NULL_CONTAINER;
    }
    #endif

    #if DC_CLIENT
    GS_ASSERT(container_event == NULL);
    container_event = (ItemContainerID*)malloc(CONTAINER_EVENT_MAX * sizeof(ItemContainerID));
    for (int i=0; i<CONTAINER_EVENT_MAX; container_event[i++] = NULL_CONTAINER);

    GS_ASSERT(containers == NULL);
    GS_ASSERT(container_uis == NULL);
    containers = (ItemContainerInterface**)calloc(MAX_CONTAINER_TYPES, sizeof(ItemContainerInterface*));
    container_uis = (ItemContainerUIInterface**)calloc(MAX_CONTAINER_TYPES, sizeof(ItemContainerUIInterface*));
    #endif
}

void teardown()
{
    delete item_container_list;
    #if DC_CLIENT
    delete player_container_ui;
    delete player_hand_ui;
    delete player_toolbelt_ui;
    delete player_synthesizer_ui;
    delete player_energy_tanks_ui;
    delete player_equipment_ui;
    delete crafting_bench_ui;
    delete premium_cache_ui;
    delete storage_block_ui;
    delete cryofreezer_ui;
    delete smelter_ui;
    delete crusher_ui;
    free(container_event);
    free(container_uis);
    free(containers);
    #endif
    #if DC_SERVER
    free(agent_inventory_list);
    free(agent_toolbelt_list);
    free(agent_synthesizer_list);
    free(agent_energy_tanks_list);
    free(agent_equipment_list);
    free(premium_cache_list);
    free(agent_hand_list);
    free(opened_containers);
    #endif
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


ItemContainerInterface* get_container(ItemContainerID id)
{
    return item_container_list->get(id);
}

void destroy_container(ItemContainerID id)
{
    IF_ASSERT(id == NULL_CONTAINER) return;
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
    for (int i=0; i<MAX_AGENTS; i++)
        if (opened_containers[i] == id)
            agent_close_container((AgentID)i, opened_containers[i]);

    // destroy the container's contents
    // NOTE: if you want to do something with the contents, like throw them on the
    // ground, do it before this function
    for (int i=0; i<container->slot_max; i++)
        if (container->slot[i] != NULL_ITEM)
            Item::destroy_item(container->slot[i]);
    #endif

    item_container_list->destroy(id);
}

ItemContainerType get_container_type(ItemContainerID container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return NULL_CONTAINER_TYPE;
    return container->type;
}

AgentID get_container_owner(ItemContainerID container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return NULL_AGENT;
    return container->owner;
}

bool container_block_in_range_of(Vec3 pos, int block[3])
{
    Vec3 container_position = vec3_init(block[0], block[1], block[2]);
    container_position = vec3_add(container_position, vec3_init(0.5f, 0.5f, 0.5f));
    container_position = translate_position(container_position);
    container_position = quadrant_translate_position(pos, container_position);
    if (vec3_distance_squared(pos, container_position) <=
        AGENT_CONTAINER_REACH*AGENT_CONTAINER_REACH) return true;
    return false;
}

}   // ItemContainer

// Client
#if DC_CLIENT
namespace ItemContainer
{

ItemContainerInterface* create_container(ItemContainerType type, ItemContainerID id)
{
    return item_container_list->create(type, id);
}

void update_container_ui_from_state()
{
    for (size_t i=0; i<MAX_CONTAINER_TYPES; i++)
        if (container_uis[i] != NULL)
        {
            IF_ASSERT(containers[i] == NULL) continue;
            container_uis[i]->load_data(containers[i]->slot);
        }
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

bool open_container(ItemContainerID container_id)
{
    GS_ASSERT(!input_state.agent_inventory);    // check that agent container is not opened here
    GS_ASSERT(container_id != NULL_CONTAINER);

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return false;
    GS_ASSERT(container->type != NULL_CONTAINER_TYPE);
    IF_ASSERT(container_type_is_attached_to_agent(container->type)) return false;

    if (!container->can_be_opened_by(ClientState::player_agent.agent_id)) return false;
    GS_ASSERT(opened_container == NULL_CONTAINER);

    for (size_t i=0; i<MAX_CONTAINER_TYPES; i++)
        if (i == (size_t)container->type)
        {
            containers[i] = NULL;
            container_uis[i] = NULL;
        }
        else
        {
            ContainerAttribute* attr = get_attr((ItemContainerType)i);
            if (attr == NULL || attr->attached_to_agent) continue;
            GS_ASSERT(containers[i] == NULL);
            GS_ASSERT(container_uis[i] == NULL);
        }

    #define ASSIGN_CONTAINER(NAME, CLASS, UI_CLASS) { do { \
        NAME = (CLASS*)container; \
        if (NAME##_ui != NULL) delete NAME##_ui; \
        NAME##_ui = new UI_CLASS(NAME->id); \
        NAME##_ui->set_alt_parameters(NAME->alt_xdim, NAME->alt_ydim); \
        NAME##_ui->init(NAME->type, NAME->xdim, NAME->ydim); \
        NAME##_ui->load_data(NAME->slot); \
        containers[container->type] = NAME; \
        container_uis[container->type] = NAME##_ui; \
        } while (0); }

    // setup UI widget
    if (container->type == name::crafting_bench_basic)
        ASSIGN_CONTAINER(crafting_bench, ItemContainerCraftingBench, ItemContainerUI)
    else
    if (container->type == name::storage_block_small)
        ASSIGN_CONTAINER(storage_block, ItemContainer, ItemContainerUI)
    else
    if (container->type == name::cryofreezer_small)
        ASSIGN_CONTAINER(cryofreezer, ItemContainerCryofreezer, ItemContainerUI)
    else
    if (container->type == name::smelter_basic)
        ASSIGN_CONTAINER(smelter, ItemContainerSmelter, ItemContainerSmelterUI)
    else
    if (container->type == name::crusher)
        ASSIGN_CONTAINER(crusher, ItemContainerCrusher, ItemContainerCrusherUI)
    else
    {
        GS_ASSERT(false);
        return false;
    }

    #undef ASSIGN_CONTAINER

    HudContainer::set_container_id(container->type, container->id);

    did_open_container_block = true;
    did_close_container_block = false;

    opened_container = container_id;

    // send open packet
    opened_container_event_id = record_container_event(container_id);
    if (opened_container_event_id >= 0)
        send_container_open(container_id, opened_container_event_id);
    return true;
}

bool close_container(ItemContainerID container_id)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return false;
    if (container_id != opened_container) return false;

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return false;
    IF_ASSERT(!isValid(container->type)) return false;

    // attempt throw
    mouse_left_click_handler(NULL_CONTAINER, NULL_SLOT, false);

    // clear the contents, for public containers
    if (!container->attached_to_agent)
        container->clear();

    containers[container->type] = NULL;
    container_uis[container->type] = NULL;

    #define TEARDOWN_CONTAINER(NAME) { do { \
        NAME = NULL; \
        delete NAME##_ui; \
        NAME##_ui = NULL; \
    } while (0); }

    // public block types go here
    TEARDOWN_CONTAINER(crafting_bench);
    TEARDOWN_CONTAINER(storage_block);
    TEARDOWN_CONTAINER(cryofreezer);
    TEARDOWN_CONTAINER(smelter);
    TEARDOWN_CONTAINER(crusher);

    #undef TEARDOWN_CONTAINER

    // unset hud container id
    HudContainer::close_container(container_id);

    opened_container = NULL_CONTAINER;
    did_open_container_block = false;
    did_close_container_block = true;

    return true;
}

bool container_block_was_opened(ItemContainerID* container_id)
{
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
    if (!did_close_container_block) return false;
    did_close_container_block = false;
    return true;
}

ItemContainerID get_event_container_id(int event_id)
{
    GS_ASSERT(event_id >= 0 && event_id < CONTAINER_EVENT_MAX);
    IF_ASSERT(container_event == NULL) return NULL_CONTAINER;
    return container_event[event_id];
}

ItemContainerUIInterface* get_container_ui(ItemContainerID container_id)
{
    IF_ASSERT(!isValid(container_id)) return NULL;
    for (size_t i=0; i<MAX_CONTAINER_TYPES; i++)
        if (container_uis[i] != NULL && container_uis[i]->id == container_id)
            return container_uis[i];
    return NULL;
}

ItemID get_toolbelt_item(int slot)
{
    if (player_toolbelt == NULL) return NULL_ITEM;  // dont assert, its normal if server is not connected
    GS_ASSERT(slot >= 0 && slot < player_toolbelt->xdim);
    return player_toolbelt->get_item(slot);
}

ItemID* get_container_contents(ItemContainerID container_id)
{
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return NULL;
    return container->slot;
}

int get_container_ui_slot_max(ItemContainerID container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return 0;
    return container->slot_max;
}

struct SlotMetadata* get_container_ui_slot_metadata(ItemContainerID container_id)
{
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return NULL;
    return container->slot_metadata;
}

void set_ui_slot_durability(ItemContainerID container_id, int slot, int durability)
{
    if (slot == NULL_SLOT) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;

    struct SlotMetadata metadata = container->get_slot_metadata(slot);
    metadata.durability = durability;
    container->insert_item(slot, metadata);
}

void set_ui_slot_stack_size(ItemContainerID container_id, int slot, int stack_size)
{
    if (slot == NULL_SLOT) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;

    struct SlotMetadata metadata = container->get_slot_metadata(slot);
    metadata.stack_size = stack_size;
    container->insert_item(slot, metadata);
}

void set_ui_slot_charges(ItemContainerID container_id, int slot, int charges)
{
    if (slot == NULL_SLOT) return;
    ItemContainerUIInterface* container = get_container_ui(container_id);
    if (container == NULL) return;

    struct SlotMetadata metadata = container->get_slot_metadata(slot);
    metadata.charges = charges;
    container->insert_item(slot, metadata);
}

}   // ItemContainer
#endif

// Server
#if DC_SERVER

namespace ItemContainer
{

bool agent_can_access_container(AgentID agent_id, ItemContainerID container_id)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(container_id == NULL_CONTAINER) return false;
    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return false;
    return container->can_be_opened_by(agent_id);
}

ItemID get_agent_toolbelt_item(AgentID agent_id, int slot)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_ITEM;
    ItemContainerID toolbelt_id = get_agent_toolbelt(agent_id);
    if (toolbelt_id == NULL_CONTAINER) return NULL_ITEM;
    ItemContainer* toolbelt = (ItemContainer*)get_container(toolbelt_id);
    if (toolbelt == NULL) return NULL_ITEM;
    return toolbelt->get_item(slot);
}

ItemContainerID get_agent_hand(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_CONTAINER;
    return agent_hand_list[agent_id];
}

ItemID get_agent_hand_item(AgentID agent_id)
{
    ItemContainerID hand_id = get_agent_hand(agent_id);
    IF_ASSERT(hand_id == NULL_CONTAINER) return NULL_ITEM;
    ItemContainerHand* hand = (ItemContainerHand*)get_container(hand_id);
    IF_ASSERT(hand == NULL) return NULL_ITEM;
    return hand->get_item();
}

ItemContainerID get_agent_inventory(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_CONTAINER;
    return agent_inventory_list[agent_id];
}

ItemContainerID get_agent_toolbelt(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_CONTAINER;
    return agent_toolbelt_list[agent_id];
}

ItemContainerID get_agent_synthesizer(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_CONTAINER;
    return agent_synthesizer_list[agent_id];
}

ItemContainerID get_agent_energy_tanks(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_CONTAINER;
    return agent_energy_tanks_list[agent_id];
}

ItemContainerID get_agent_premium_cache(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_CONTAINER;
    return premium_cache_list[agent_id];
}

ItemContainerID get_agent_equipment(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return NULL_CONTAINER;
    return agent_equipment_list[agent_id];
}

ItemContainerID* get_player_containers(AgentID agent_id, size_t* n_containers)
{
    static ItemContainerID containers[N_PLAYER_CONTAINERS] = {NULL_CONTAINER};
    size_t n = 0;
    containers[n] = get_agent_hand(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_toolbelt(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_inventory(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_synthesizer(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_energy_tanks(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_premium_cache(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;
    containers[n] = get_agent_equipment(agent_id);
    if (containers[n] != NULL_CONTAINER) n++;

    GS_ASSERT(n == N_PLAYER_CONTAINERS);
    *n_containers = n;
    return containers;
}


ItemContainerInterface* create_container(ItemContainerType type)
{
    return item_container_list->create(type);
}

static void assign_container_to_agent(ItemContainerInterface* container, ItemContainerID* container_list, AgentID agent_id, ClientID client_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    GS_ASSERT(container_list[agent_id] == NULL_CONTAINER);

    container_list[agent_id] = container->id;
    container->assign_owner(agent_id);
    send_container_create(client_id, container->id);
}

bool assign_containers_to_agent(AgentID agent_id, ClientID client_id)
{
    IF_ASSERT(!isValid(agent_id)) return false;
    IF_ASSERT(!isValid(client_id)) return false;

    ItemContainerInterface* agent_toolbelt     = create_container(name::toolbelt);
    ItemContainerInterface* agent_inventory    = create_container(name::inventory);
    ItemContainerInterface* agent_hand         = create_container(name::hand);
    ItemContainerInterface* agent_equipment    = create_container(name::equipment);
    ItemContainerInterface* agent_synthesizer  = create_container(name::synthesizer);
    ItemContainerInterface* agent_energy_tanks = create_container(name::energy_tanks);
    #if !PRODUCTION
    // TODO -- enable this for certain premium users
    ItemContainerInterface* premium_cache      = create_container(name::premium_cache);
    #endif

    IF_ASSERT(agent_hand         == NULL) goto error;
    IF_ASSERT(agent_toolbelt     == NULL) goto error;
    IF_ASSERT(agent_inventory    == NULL) goto error;
    IF_ASSERT(agent_synthesizer  == NULL) goto error;
    IF_ASSERT(agent_energy_tanks == NULL) goto error;
    IF_ASSERT(agent_equipment    == NULL) goto error;
    #if !PRODUCTION
    // TODO -- enable this for certain premium users
    IF_ASSERT(premium_cache      == NULL) goto error;
    #endif

    assign_container_to_agent(agent_hand,         agent_hand_list,         agent_id, client_id);
    assign_container_to_agent(agent_toolbelt,     agent_toolbelt_list,     agent_id, client_id);
    assign_container_to_agent(agent_inventory,    agent_inventory_list,    agent_id, client_id);
    assign_container_to_agent(agent_synthesizer,  agent_synthesizer_list,  agent_id, client_id);
    assign_container_to_agent(agent_energy_tanks, agent_energy_tanks_list, agent_id, client_id);
    assign_container_to_agent(agent_equipment,    agent_equipment_list,    agent_id, client_id);
    #if !PRODUCTION
    assign_container_to_agent(premium_cache,      premium_cache_list,      agent_id, client_id);
    #endif

    return true;

    error:
        if (agent_hand         != NULL) destroy_container(agent_hand->id);
        if (agent_toolbelt     != NULL) destroy_container(agent_toolbelt->id);
        if (agent_inventory    != NULL) destroy_container(agent_inventory->id);
        if (agent_synthesizer  != NULL) destroy_container(agent_synthesizer->id);
        if (agent_energy_tanks != NULL) destroy_container(agent_energy_tanks->id);
        if (agent_equipment    != NULL) destroy_container(agent_equipment->id);
        #if !PRODUCTION
        if (premium_cache      != NULL) destroy_container(premium_cache->id);
        #endif
        return false;
}

void send_container_assignments_to_agent(AgentID agent_id, ClientID client_id)
{
    size_t n_containers = 0;
    ItemContainerID* containers = get_player_containers(agent_id, &n_containers);
    for (size_t i=0; i<n_containers; i++)
        send_container_assign(client_id, containers[i]);
}

static void throw_items_from_container(ClientID client_id, AgentID agent_id, ItemContainerID container_id)
{
    GS_ASSERT(isValid(agent_id));
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

void agent_born(AgentID agent_id)
{
    // refill toolbelt if needed

    // toolbelt should have at least 1 maxed out laser rifle
    // and 1 maxed out mining_laser

    // if we have one or more of either not maxed out, fill up the most full one

    // if we have neither and there is only one slot, add the laser rifle (because fist can replace mining_laser)

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;
    ClientID client_id = a->client_id;

    IF_ASSERT(agent_toolbelt_list[agent_id] == NULL_CONTAINER) return;
    ItemContainerInterface* toolbelt = get_container(agent_toolbelt_list[agent_id]);
    IF_ASSERT(toolbelt == NULL) return;

    // collect information on mining_laser / laser rifle contents

    int toolbelt_space = toolbelt->slot_max - toolbelt->slot_count;

    ItemType laser_rifle_type = Item::get_item_type("laser_rifle");
    GS_ASSERT(laser_rifle_type != NULL_ITEM_TYPE);
    bool has_laser_rifle = false;
    Item::Item* most_durable_laser_rifle = NULL;
    int most_durable_laser_rifle_value = 0;

    ItemType shovel_type = Item::get_item_type("copper_shovel");
    GS_ASSERT(shovel_type != NULL_ITEM_TYPE);
    bool has_shovel = false;
    Item::Item* most_durable_shovel = NULL;
    int most_durable_shovel_value = 0;

    IF_ASSERT(toolbelt->slot_max <= 0) return;
    for (int i=toolbelt->slot_max-1; i>=0; i--)
    {
        ItemID item_id = toolbelt->slot[i];
        if (item_id == NULL_ITEM) continue;
        Item::Item* item = Item::get_item(item_id);
        IF_ASSERT(item == NULL) continue;

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
    ItemContainerID energy_tanks_id = get_agent_energy_tanks(agent_id);
    GS_ASSERT(energy_tanks_id != NULL_CONTAINER);
    ItemContainerEnergyTanks* energy_tanks = (ItemContainerEnergyTanks*)get_container(energy_tanks_id);
    GS_ASSERT(energy_tanks != NULL);
    if (energy_tanks != NULL)
    {
        #if PRODUCTION
        int max_energy_tanks = 0;
        #else
        int max_energy_tanks = 1;
        #endif
        int n_energy_tanks = energy_tanks->slot_count;
        for (int i=0; i<max_energy_tanks-n_energy_tanks; i++)
        {
            ItemType energy_tank_type = Item::get_item_type("energy_tank");
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
    ItemContainerID synth_id = get_agent_synthesizer(agent_id);
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

    //Item::Item* rock_landmine = Item::create_item(Item::get_item_type("rock_landmine"));
    //GS_ASSERT(rock_landmine != NULL);
    //if (rock_landmine != NULL)
    //{
        //event = auto_add_free_item_to_container(client_id, toolbelt->id, rock_landmine->id);
        //if (event == CONTAINER_ACTION_NONE || event == PARTIAL_WORLD_TO_OCCUPIED_SLOT) Item::destroy_item(rock_landmine->id);
        //else if (rock_landmine->stack_size <= 0) Item::destroy_item(rock_landmine->id);
    //}

    Item::Item* storage_block_small = Item::create_item(Item::get_item_type("storage_block_small"));
    GS_ASSERT(storage_block_small != NULL);
    if (storage_block_small != NULL)
    {
        event = auto_add_free_item_to_container(client_id, toolbelt->id, storage_block_small->id);
        if (event == CONTAINER_ACTION_NONE || event == PARTIAL_WORLD_TO_OCCUPIED_SLOT) Item::destroy_item(storage_block_small->id);
        else if (storage_block_small->stack_size <= 0) Item::destroy_item(storage_block_small->id);
    }
    #endif

}

void agent_died(AgentID agent_id)
{
    IF_ASSERT(!isValid(agent_id)) return;
    Agents::Agent* a = Agents::get_agent(agent_id);
    if (a == NULL) return;
    GS_ASSERT(a->status.dead);

    // throw agent inventory items
    if (agent_inventory_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(a->client_id, a->id, agent_inventory_list[agent_id]);

    // close container
    if (opened_containers[agent_id] != NULL_CONTAINER)
        send_container_close(agent_id, opened_containers[agent_id]);

    // throw any items in hand. the agent_close_container will have thrown anything, if a free container was open
    // DO NOT throw_items_from_container(agent_hand), we need to use the transactional method for hand throwing
    ItemID hand_item = get_agent_hand_item(agent_id);
    if (hand_item != NULL_ITEM)
        transfer_hand_to_particle(agent_id);    // throw
    GS_ASSERT(get_agent_hand_item(agent_id) == NULL_ITEM);
}

static void save_agent_containers(ClientID client_id, AgentID agent_id)
{
    IF_ASSERT(!isValid(client_id)) return;
    IF_ASSERT(!isValid(agent_id)) return;

    // TODO -- re-enable hand saving after we have a magic container for lost/purchased items
    //serializer::save_player_container(client_id, agent_hand_list[agent_id]);
    serializer::save_player_container(client_id, agent_toolbelt_list[agent_id]);
    serializer::save_player_container(client_id, agent_inventory_list[agent_id]);
    serializer::save_player_container(client_id, agent_synthesizer_list[agent_id]);
    serializer::save_player_container(client_id, agent_energy_tanks_list[agent_id]);
    serializer::save_player_container(client_id, agent_equipment_list[agent_id]);
    if (premium_cache_list[agent_id] != NULL_CONTAINER)
        serializer::save_player_container(client_id, premium_cache_list[agent_id]);
}

void dump_agent_containers(ClientID client_id, AgentID agent_id)
{
    IF_ASSERT(!isValid(client_id)) return;
    IF_ASSERT(!isValid(agent_id)) return;

    if (agent_inventory_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_inventory_list[agent_id]);

    if (agent_synthesizer_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_synthesizer_list[agent_id]);

    if (agent_toolbelt_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_toolbelt_list[agent_id]);

    if (agent_energy_tanks_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_energy_tanks_list[agent_id]);

    if (premium_cache_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, premium_cache_list[agent_id]);

    if (agent_equipment_list[agent_id] != NULL_CONTAINER)
        throw_items_from_container(client_id, agent_id, agent_equipment_list[agent_id]);

    // DO NOT throw_items_from_container(agent_hand), we need to use the transactional method for hand throwing
    ItemID hand_item = get_agent_hand_item(agent_id);
    if (hand_item != NULL_ITEM)
        transfer_hand_to_particle(agent_id);    // throw
}

void agent_quit(AgentID agent_id)
{
    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    // close opened free container (this will throw any items sitting in hand)
    if (opened_containers[agent_id] != NULL_CONTAINER)
        agent_close_container(agent_id, opened_containers[agent_id]);

    IF_ASSERT(opened_containers[agent_id] != NULL_CONTAINER)
        opened_containers[agent_id] = NULL_CONTAINER;

    // still have to throw any items in hand, in case we have our private inventory opened
    ItemID hand_item = get_agent_hand_item(agent_id);
    if (hand_item != NULL_ITEM)
        transfer_hand_to_particle(agent_id);    // throw
    GS_ASSERT(get_agent_hand_item(agent_id) == NULL_ITEM);

    if (Options::serializer)
        save_agent_containers(a->client_id, a->id);
    else
        dump_agent_containers(a->client_id, a->id);

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

    if (agent_equipment_list[agent_id] != NULL_CONTAINER)
        destroy_container(agent_equipment_list[agent_id]);

    if (premium_cache_list[agent_id] != NULL_CONTAINER)
        destroy_container(premium_cache_list[agent_id]);

    agent_inventory_list[agent_id] = NULL_CONTAINER;
    agent_toolbelt_list[agent_id] = NULL_CONTAINER;
    agent_synthesizer_list[agent_id] = NULL_CONTAINER;
    agent_energy_tanks_list[agent_id] = NULL_CONTAINER;
    agent_hand_list[agent_id] = NULL_CONTAINER;
    agent_equipment_list[agent_id] = NULL_CONTAINER;
    premium_cache_list[agent_id] = NULL_CONTAINER;
}

void purchase_item_from_synthesizer(AgentID agent_id, int shopping_slot)
{
    IF_ASSERT(!isValid(agent_id)) return;

    // get container
    if (agent_synthesizer_list[agent_id] == NULL_CONTAINER) return;
    ItemContainerSynthesizer* synthesizer = (ItemContainerSynthesizer*)get_container(agent_synthesizer_list[agent_id]);
    if (synthesizer == NULL) return;
    IF_ASSERT(!agent_can_access_container(agent_id, synthesizer->id)) return;

    // get the store item
    int xslot = shopping_slot % synthesizer->alt_xdim;
    int yslot = shopping_slot / synthesizer->alt_xdim;
    int cost;
    ItemType item_type = Item::get_synthesizer_item(xslot, yslot, &cost);
    IF_ASSERT(cost < 0) return;
    if (item_type == NULL_ITEM_TYPE) return;

    // compare to hand
    ItemID hand_item = get_agent_hand_item(agent_id);
    int hand_item_type = Item::get_item_type(hand_item);
    if (hand_item != NULL_ITEM)
    {   // can it stack
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

    Item::Item* coin_item = Item::get_item(coins);
    IF_ASSERT(coin_item == NULL) return;

    if (hand_item == NULL_ITEM)
    {   // create shopped item
        Item::Item* purchase = Item::create_item(item_type);
        IF_ASSERT(purchase == NULL) return;

        // add to hand
        transfer_free_item_to_hand(purchase->id, agent_id);
    }
    else
    {   // get hand item pointer
        Item::Item* hand = Item::get_item(hand_item);
        IF_ASSERT(hand == NULL) return;

        // increase count by 1
        hand->stack_size += 1;
        GS_ASSERT(hand->stack_size <= Item::get_max_stack_size(hand->type));
        Item::send_item_state(hand->id);
    }

    if (cost && coin_item != NULL)
    {   // update coins
        if (coin_stack == cost)
        {   // delete coins
            Item::destroy_item(coins);
        }
        else
        {   // decrement coin stack
            coin_item->stack_size -= cost;
            GS_ASSERT(coin_item->stack_size > 0);
            Item::send_item_state(coins);
        }
    }
}

void craft_item_from_bench(AgentID agent_id, ItemContainerID container_id, int craft_slot)
{
    GS_ASSERT(isValid(agent_id));
    Agents::Agent* agent = Agents::get_agent(agent_id);
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

bool consume_crafting_reagents(AgentID agent_id, ItemContainerID container_id, int recipe_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(recipe_id != NULL_CRAFTING_RECIPE);
    GS_ASSERT(isValid(agent_id));

    Agents::Agent* agent = Agents::get_agent(agent_id);
    if (agent == NULL) return false;

    // agent does not own container, abort
    if (container_id != NULL_CONTAINER && !agent_can_access_container(agent->id, container_id)) return false;

    ItemContainerCraftingBench* bench = (ItemContainerCraftingBench*)get_container(container_id);
    IF_ASSERT(bench == NULL) return false;

    Item::CraftingRecipe* recipe = Item::get_craft_recipe(recipe_id);
    IF_ASSERT(recipe == NULL) return false;;

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
        ItemType item_type = Item::get_item_type(item_id);
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
        IF_ASSERT(item == NULL) return false;
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
        IF_ASSERT(item == NULL) return false;
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
            GS_ASSERT(item->stack_size > 0);
            Item::send_item_state(item->id);
        }
    }

    return true;
}

void send_container_contents(AgentID agent_id, ClientID client_id, ItemContainerID container_id)
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

bool agent_in_container_range(AgentID agent_id, ItemContainerID container_id)
{
    // get agent position
    IF_ASSERT(!isValid(agent_id)) return false;
    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return false;

    Vec3 agent_position = a->get_camera_position();

    // get container position, if applicable
    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return false;
    IF_ASSERT(!container_type_is_block(container->type)) return false;

    int block[3];
    if (!t_map::get_container_location(container->id, block))
    {
        IF_ASSERT(true) return false;
    }
    return container_block_in_range_of(agent_position, block);
}

// check that agents are still in range of containers they are accessing
void check_agents_in_container_range()
{
    using Agents::agent_list;
    for (size_t i=0; i<agent_list->max; i++)
    {
        if (agent_list->objects[i].id == agent_list->null_id) continue;
        Agents::Agent* a = &agent_list->objects[i];
        if (opened_containers[a->id] == NULL_CONTAINER) continue;
        if (agent_in_container_range(a->id, opened_containers[a->id])) continue;
        agent_close_container(a->id, opened_containers[a->id]);
    }
}

// for brand new Item*s going to a container
ContainerActionType auto_add_free_item_to_container(ClientID client_id, ItemContainerID container_id, ItemID item_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);

    Item::Item* item = Item::get_item(item_id);
    IF_ASSERT(item == NULL) return CONTAINER_ACTION_NONE;
    GS_ASSERT(item->stack_size > 0);
    GS_ASSERT(item->type != NULL_ITEM_TYPE);

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return CONTAINER_ACTION_NONE;

    int slot = container->get_stackable_slot(item->type, item->stack_size);
    if (slot == NULL_SLOT)
    {   // no easy stack slot found
        // try to put in empty slot
        slot = container->get_empty_slot();
        if (slot == NULL_SLOT)
        {   // no empty slot found, try to break this stack up and redistribute
            int starting_stack_size = item->stack_size;
            int stack_size = item->stack_size;
            ItemType item_type = item->type;

            // distribute as much as we can into slots
            for (int i=0; i<container->slot_max; i++)
            {
                GS_ASSERT(stack_size > 0);
                if (container->slot[i] == NULL_ITEM) continue;
                Item::Item* slot_item = Item::get_item(container->slot[i]);
                IF_ASSERT(slot_item == NULL) continue;
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

ItemID auto_add_item_to_container(const char* item_name, ItemContainerID container_id)
{   // creates and adds item to the container. returns id of the created item
    IF_ASSERT(!isValid(container_id)) return NULL_ITEM;

    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return NULL_ITEM;

    int slot = container->get_empty_slot();
    if (slot == NULL_SLOT) return NULL_ITEM;    // can't fit

    class Item::Item* item = Item::create_item(item_name);
    IF_ASSERT(item == NULL) return NULL_ITEM;

    bool can_insert = container->can_insert_item(slot, item->id);
    IF_ASSERT(!can_insert)
    {
        Item::destroy_item(item->id);
        return NULL_ITEM;
    }

    transfer_free_item_to_container(item->id, container->id, slot);

    return item->id;
}

ItemContainerID create_container_block(ItemContainerType container_type, int x, int y, int z)
{
    IF_ASSERT(!isValid(container_type)) return NULL_CONTAINER;

    CubeType cube_type = t_map::get_cube_type_for_container(container_type);
    IF_ASSERT(cube_type == NULL_CUBE) return NULL_CONTAINER;

    ItemContainerInterface* container = create_container(container_type);
    IF_ASSERT(container == NULL) return NULL_CONTAINER;

    t_map::broadcast_set_block(x,y,z, cube_type);

    bool added = t_map::create_item_container_block(x,y,z, container->type, container->id);
    IF_ASSERT(!added)
    {
        destroy_container(container->id);
        return NULL_CONTAINER;
    }
    return container->id;
}

void update_smelters()
{
    for (size_t i=0; i<item_container_list->n_max; i++)
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
            ItemType fuel_item_type = NULL_ITEM_TYPE;
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
                    bool is_here = (fuel->location == IL_CONTAINER &&
                                    fuel->location_id == smelter->id &&
                                    fuel->container_slot == smelter->fuel_slot);
                    GS_ASSERT(is_here);
                    if (is_here)
                    {   // throw it out
                        int p[3];
                        if (t_map::get_container_location(smelter->id, p))
                        {
                            Vec3 pos = vec3_init(p[0], p[1], p[2]);
                            pos = vec3_add(pos, vec3_init(0.5f, 0.5f, 1.05f));
                            ItemParticle::dump_container_item(
                                fuel_item, pos.x, pos.y, pos.z);
                        }
                        else
                        {
                            GS_ASSERT(false);
                        }
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
bool load_item_into_container(ItemID item_id, ItemContainerID container_id, int container_slot)
{
    return transfer_free_item_to_container(item_id, container_id, container_slot);
}

bool load_item_into_hand(ItemID item_id, AgentID agent_id)
{
    return transfer_free_item_to_hand(item_id, agent_id);
}

//tests
void test_container_list_capacity()
{
    for (int i=0; i<MAX_CONTAINERS*2; i++)
        item_container_list->create(name::inventory);
}

void container_block_destroyed(ItemContainerID container_id, int x, int y, int z)
{
    IF_ASSERT(container_id == NULL_CONTAINER) return;
    ItemContainerInterface* container = get_container(container_id);
    IF_ASSERT(container == NULL) return;

    // close all opened containers
    for (int i=0; i<MAX_AGENTS; i++)
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

    // destroy container
    destroy_container(container_id);
}


}   // ItemContainer

#endif
