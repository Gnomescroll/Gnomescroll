#include "client.hpp"

#if DC_CLIENT

namespace Item
{

static uint16_t container_event_id = 0;
int container_event[CONTAINER_EVENT_MAX];

static void send_container_alpha_action(ContainerActionType action, int container_id, int slot)
{
    assert(container_id != NULL_CONTAINER);
    assert(action != CONTAINER_ACTION_NONE);
    
    container_event_id += 1;
    container_event_id %= CONTAINER_EVENT_MAX;
    container_event[container_event_id] = container_id;
    
    container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    ItemContainerUI* container = get_container_ui(container_id);
    assert(container != NULL);

    msg.slot_type = container->get_slot_type(slot);
    msg.slot_stack = container->get_slot_stack(slot);
    
    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;
    
    msg.send();
}

static void send_container_beta_action(ContainerActionType action, int container_id, int slot)
{
    assert(container_id != NULL_CONTAINER);
    assert(action != CONTAINER_ACTION_NONE);
    
    container_event_id += 1;
    container_event_id %= CONTAINER_EVENT_MAX;
    container_event[container_event_id] = container_id;

    container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    ItemContainerUI* container = get_container_ui(container_id);
    assert(container != NULL);

    msg.slot_type = container->get_slot_type(slot);
    msg.slot_stack = container->get_slot_stack(slot);
    
    msg.hand_type = player_hand_type_ui;
    msg.hand_stack = player_hand_stack_ui;
    
    msg.send();
}

void mouse_left_click_handler(int id, int slot)
{
    ContainerActionType action = alpha_action_decision_tree(id, slot);
    if (action != CONTAINER_ACTION_NONE) send_container_alpha_action(action, id, slot);
}

void mouse_right_click_handler(int id, int slot)
{
    ContainerActionType action = beta_action_decision_tree(id, slot);
    if (action != CONTAINER_ACTION_NONE) send_container_beta_action(action, id, slot);
}

}

#endif
