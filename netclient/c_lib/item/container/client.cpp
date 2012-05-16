#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace ItemContainer
{

static uint16_t container_event_id = 0;
int container_event[CONTAINER_EVENT_MAX];

int record_container_event(int container_id)
{
    container_event_id += 1;
    container_event_id %= CONTAINER_EVENT_MAX;
    container_event[container_event_id] = container_id;
    return container_event_id;
}

static void send_container_alpha_action(ContainerActionType action, int container_id, int slot)
{
    assert(action != CONTAINER_ACTION_NONE);
    
    record_container_event(container_id);
    
    container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        assert(container != NULL);

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
}

static void send_container_beta_action(ContainerActionType action, int container_id, int slot)
{
    assert(container_id != NULL_CONTAINER);
    assert(action != CONTAINER_ACTION_NONE);
    
    record_container_event(container_id);

    container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;
    
    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        assert(container != NULL);

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

static void send_nanite_alpha_action(ContainerActionType action, int container_id, int slot)
{
    assert(action != CONTAINER_ACTION_NONE);
    
    record_container_event(container_id);
    
    nanite_container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        assert(container != NULL);

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
}

static void send_nanite_beta_action(ContainerActionType action, int container_id, int slot)
{
    assert(container_id != NULL_CONTAINER);
    assert(action != CONTAINER_ACTION_NONE);
    
    record_container_event(container_id);

    nanite_container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;
    
    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        assert(container != NULL);

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

static void send_craft_alpha_action(ContainerActionType action, int container_id, int slot)
{
    assert(action != CONTAINER_ACTION_NONE);
    
    record_container_event(container_id);
    
    craft_container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        assert(container != NULL);

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
    printf("sent craft alpha action\n");
}

static void send_craft_beta_action(ContainerActionType action, int container_id, int slot)
{
    assert(container_id != NULL_CONTAINER);
    assert(action != CONTAINER_ACTION_NONE);
    
    record_container_event(container_id);

    craft_container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;
    
    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        assert(container != NULL);

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

static void send_craft_item_action(int container_id, int slot)
{
    assert(container_id != NULL_CONTAINER);

    record_container_event(container_id);
    
    craft_item_from_bench_action_CtoS msg;
    msg.event_id = container_event_id;
    
    msg.container_id = container_id;
    msg.slot = slot;

    msg.send();
}

static void send_no_container_alpha_action(ContainerActionType action)
{
    record_container_event(NULL_CONTAINER);

    no_container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;
    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;
    msg.send();
}

static void send_no_container_beta_action(ContainerActionType action)
{
    record_container_event(NULL_CONTAINER);

    no_container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;
    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;
    msg.send();
}

// Handlers

void mouse_left_click_handler(int container_id, int slot, bool nanite, bool craft_output)
{
    ContainerActionType action;

    int container_type = CONTAINER_TYPE_NONE;
    if (container_id != NULL_CONTAINER)
    {
        ItemContainerInterface* container = get_container(container_id);
        if (container == NULL) return;
        container_type = container->type;
    }

    switch (container_type)
    {
        case AGENT_CONTAINER:
        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
        case AGENT_TOOLBELT:
            action = alpha_action_decision_tree(container_id, slot);
            break;
        case AGENT_NANITE:
            action = nanite_alpha_action_decision_tree(container_id, slot);
            break;
        case CONTAINER_TYPE_CRAFTING_BENCH_REFINERY:
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            printf("crafting bench\n");
            if (craft_output) action = craft_output_alpha_action_decision_tree(container_id, slot);
            else action = craft_input_alpha_action_decision_tree(container_id, slot);
            break;
        case CONTAINER_TYPE_NONE:
            action = no_container_alpha_action_decision_tree();
            break;
        default:
            assert(false);
            return;
    }

    if (action == CONTAINER_ACTION_NONE) return;
    printf("container action is not none\n");
    switch (container_type)
    {
        case AGENT_CONTAINER:
        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
        case AGENT_TOOLBELT:
            send_container_alpha_action(action, container_id, slot);
            break;
        case AGENT_NANITE:
            send_nanite_alpha_action(action, container_id, slot);
            break;
        case CONTAINER_TYPE_CRAFTING_BENCH_REFINERY:
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            printf("send craft acton\n");
            if (action == CRAFT_ITEM_FROM_BENCH) send_craft_item_action(container_id, slot);
            else send_craft_alpha_action(action, container_id, slot);
            break;
        case CONTAINER_TYPE_NONE:
            // TODO -- use a specific method/packet for this
            send_no_container_alpha_action(action);
            break;
        default:
            assert(false);
            return;
    }
}

void mouse_right_click_handler(int container_id, int slot, bool nanite, bool craft_output)
{
    ContainerActionType action;

    int container_type = CONTAINER_TYPE_NONE;
    if (container_id != NULL_CONTAINER)
    {
        ItemContainerInterface* container = get_container(container_id);
        if (container == NULL) return;
        container_type = container->type;
    }

    switch (container_type)
    {
        case AGENT_CONTAINER:
        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
        case AGENT_TOOLBELT:
            action = beta_action_decision_tree(container_id, slot);
            break;
        case AGENT_NANITE:
            action = nanite_beta_action_decision_tree(container_id, slot);
            break;
        case CONTAINER_TYPE_CRAFTING_BENCH_REFINERY:
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            if (craft_output) action = craft_output_beta_action_decision_tree(container_id, slot);
            else action = craft_input_beta_action_decision_tree(container_id, slot);
            break;
        case CONTAINER_TYPE_NONE:
            action = no_container_beta_action_decision_tree();
            break;
        default:
            assert(false);
            return;
    }

    if (action == CONTAINER_ACTION_NONE) return;

    switch (container_type)
    {
        case AGENT_CONTAINER:
        case CONTAINER_TYPE_STORAGE_BLOCK_SMALL:
        case CONTAINER_TYPE_CRYOFREEZER_SMALL:
        case AGENT_TOOLBELT:
            send_container_beta_action(action, container_id, slot);
            break;
        case AGENT_NANITE:
            send_nanite_beta_action(action, container_id, slot);
            break;
        case CONTAINER_TYPE_CRAFTING_BENCH_REFINERY:
        case CONTAINER_TYPE_CRAFTING_BENCH_UTILITY:
            if (action == CRAFT_ITEM_FROM_BENCH) send_craft_item_action(container_id, slot);
            else send_craft_beta_action(action, container_id, slot);
            break;
        case CONTAINER_TYPE_NONE:
            // do nothing
            send_no_container_beta_action(action);
            break;
        default:
            assert(false);
            return;
    }
}

}   // ItemContainer
