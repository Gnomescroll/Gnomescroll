#pragma once

#if DC_CLIENT

namespace Item
{

// 3 packets, for actions
// container_action_alpha_CtoS
// container_action_beta_CtoS
// failure_StoC

// 2 packets, for state in shared containers
// container_remove_item_StoC
// container_add_item_StoC

static uint16_t container_event_id = 0;

void send_container_alpha_action(int inventory_id, int slot)
{
    container_event_id += 1;
    container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.inventory_id = inventory_id;
    msg.slot = slot;
    msg.send();
}

void send_container_beta_action(int inventory_id, int slot)
{
    container_event_id += 1;
    container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.inventory_id = inventory_id;
    msg.slot = slot;
    msg.send();
}

void mouse_left_click_handler(int id, int slot)
{
    bool something_happened = alpha_action_decision_tree(id, slot);
    if (something_happened) send_container_alpha_action(id, slot);
}

void mouse_right_click_handler(int id, int slot)
{
    bool something_happened = beta_action_decision_tree(id, slot);
    if (something_happened) send_container_beta_action(id, slot);
}

}

#endif
