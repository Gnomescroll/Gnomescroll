#pragma once

#if DC_CLIENT

namespace Item
{

static uint16_t container_event_id = 0;

static void send_container_alpha_action(int container_id, int slot)
{
    container_event_id += 1;
    container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.container_id = container_id;
    msg.slot = slot;
    msg.send();
}

static void send_container_beta_action(int container_id, int slot)
{
    container_event_id += 1;
    container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.container_id = container_id;
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
