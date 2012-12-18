#include "client.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

namespace ItemContainer
{

static uint16_t container_event_id = 0;
int* container_event = NULL;

int record_container_event(int container_id)
{
    GS_ASSERT(container_event != NULL);
    if (container_event == NULL) return -1;
    container_event_id += 1;
    container_event_id %= CONTAINER_EVENT_MAX;
    container_event[container_event_id] = container_id;
    return container_event_id;
}

void send_container_alpha_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;

    record_container_event(container_id);
    
    container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
}

void send_container_beta_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;

    record_container_event(container_id);

    container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;
    
    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;
        
        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

void send_synthesizer_alpha_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (action == CONTAINER_ACTION_NONE) return;

    record_container_event(container_id);
    
    synthesizer_container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD || action == PURCHASE_ITEM_FROM_SYNTHESIZER)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
}

void send_synthesizer_beta_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;

    record_container_event(container_id);

    synthesizer_container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;
    
    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

void send_craft_alpha_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (action == CONTAINER_ACTION_NONE) return;
    
    record_container_event(container_id);
    
    craft_container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
}

void send_purchase_item_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(action == PURCHASE_ITEM_FROM_SYNTHESIZER);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;

    record_container_event(container_id);
    
    purchase_item_from_synthesizer_action_CtoS msg;
    msg.event_id = container_event_id;
    
    msg.container_id = container_id;
    msg.slot = slot;

    msg.send();
}

void send_craft_beta_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;
    
    record_container_event(container_id);

    craft_container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;
    
    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

void send_craft_item_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(action == CRAFT_ITEM_FROM_BENCH);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;

    record_container_event(container_id);
    
    craft_item_from_bench_action_CtoS msg;
    msg.event_id = container_event_id;
    
    msg.container_id = container_id;
    msg.slot = slot;

    msg.send();
}

void send_no_container_alpha_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;

    record_container_event(NULL_CONTAINER);

    no_container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;
    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();
    msg.send();
}

void send_no_container_beta_action(ContainerActionType action, int container_id, int slot)
{
    record_container_event(NULL_CONTAINER);

    no_container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;
    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();
    msg.send();
}

void send_smelter_alpha_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;

    record_container_event(container_id);
    
    smelter_container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
}

void send_smelter_beta_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;

    record_container_event(container_id);

    smelter_container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;
    
    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;
        
        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

void send_crusher_alpha_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;

    record_container_event(container_id);
    
    crusher_container_action_alpha_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;

        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }

    msg.send();
}

void send_crusher_beta_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(player_hand_ui != NULL);
    if (player_hand_ui == NULL) return;
    GS_ASSERT(container_id != NULL_CONTAINER);
    GS_ASSERT(action != CONTAINER_ACTION_NONE);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;

    record_container_event(container_id);

    crusher_container_action_beta_CtoS msg;
    msg.event_id = container_event_id;
    msg.action = action;

    msg.container_id = container_id;
    msg.slot = slot;

    msg.hand_type = player_hand_ui->get_item_type();
    msg.hand_stack = player_hand_ui->get_item_stack();

    if (action == FULL_HAND_TO_WORLD)
    {
        msg.slot_type = NULL_ITEM_TYPE;
        msg.slot_stack = 1;
    }
    else
    {
        ItemContainerUIInterface* container = get_container_ui(container_id);
        GS_ASSERT(container != NULL);
        if (container == NULL) return;
        
        msg.slot_type = container->get_slot_type(slot);
        msg.slot_stack = container->get_slot_stack(slot);    
    }
    
    msg.send();
}

void send_crusher_crush_action(ContainerActionType action, int container_id, int slot)
{
    GS_ASSERT(action == CRUSHER_CRUSH_ITEM);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    if (action == CONTAINER_ACTION_NONE) return;
    
    record_container_event(container_id);
    
    crusher_crush_item_CtoS msg;
    msg.event_id = container_event_id;
    
    msg.container_id = container_id;
    msg.send();
}

// Handlers

void mouse_left_click_handler(int container_id, int slot, bool alt_action)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;
    if (you->status.dead) return;
    
    ItemContainerType container_type = get_container_type(container_id);
    class ContainerAttributes* attr = get_attr(container_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return;
    GS_ASSERT(attr->loaded);
    
    ContainerActionType action = CONTAINER_ACTION_NONE;
    if (alt_action)
    {
        GS_ASSERT(attr->alpha_action_alt != NULL);
        if (attr->alpha_action_alt == NULL) return;
        action = attr->alpha_action_alt(container_id, slot);        
    }
    else
    {
        GS_ASSERT(attr->alpha_action != NULL);
        if (attr->alpha_action == NULL) return;
        action = attr->alpha_action(container_id, slot);        
    }
        
    if (action == CONTAINER_ACTION_NONE) return;

    if (alt_action)
    {
        GS_ASSERT(attr->alpha_packet_alt != NULL);
        if (attr->alpha_packet_alt == NULL) return;
        attr->alpha_packet_alt(action, container_id, slot);        
    }
    else
    {
        GS_ASSERT(attr->alpha_packet != NULL);
        if (attr->alpha_packet == NULL) return;
        attr->alpha_packet(action, container_id, slot);
    }
}

void mouse_right_click_handler(int container_id, int slot, bool alt_action)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;
    if (you->status.dead) return;

    ItemContainerType container_type = get_container_type(container_id);
    class ContainerAttributes* attr = get_attr(container_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return;
    GS_ASSERT(attr->loaded);

    ContainerActionType action = CONTAINER_ACTION_NONE;
    if (alt_action)
    {
        GS_ASSERT(attr->beta_action_alt != NULL);
        if (attr->beta_action_alt == NULL) return;
        action = attr->beta_action_alt(container_id, slot);        
    }
    else
    {
        GS_ASSERT(attr->beta_action != NULL);
        if (attr->beta_action == NULL) return;
        action = attr->beta_action(container_id, slot);        
    }

    if (action == CONTAINER_ACTION_NONE) return;

    if (alt_action)
    {
        GS_ASSERT(attr->beta_packet_alt != NULL);
        if (attr->beta_packet_alt == NULL) return;
        attr->beta_packet_alt(action, container_id, slot);        
    }
    else
    {
        GS_ASSERT(attr->beta_packet != NULL);
        if (attr->beta_packet == NULL) return;
        attr->beta_packet(action, container_id, slot);        
    }
}

void send_container_open(int container_id, int event_id)
{
    GS_ASSERT(event_id >= 0);
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (event_id < 0 || container_id == NULL_CONTAINER) return;
    open_container_CtoS msg;
    msg.container_id = container_id;
    msg.event_id = event_id;
    msg.send();
}

void send_container_close(int container_id)
{
    GS_ASSERT(container_id != NULL_CONTAINER);
    if (container_id == NULL_CONTAINER) return;
    close_container_CtoS msg;
    msg.container_id = container_id;
    msg.send();    
}

}   // ItemContainer
