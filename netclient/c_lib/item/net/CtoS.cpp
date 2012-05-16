#include "CtoS.hpp"

#if DC_CLIENT

namespace Item
{

// dummies
inline void container_action_alpha_CtoS::handle() {}
inline void container_action_beta_CtoS::handle() {}
inline void nanite_container_action_alpha_CtoS::handle() {}
inline void nanite_container_action_beta_CtoS::handle() {}
inline void craft_container_action_alpha_CtoS::handle() {}
inline void craft_container_action_beta_CtoS::handle() {}
inline void craft_item_from_bench_action_CtoS::handle() {}
inline void no_container_action_alpha_CtoS::handle(){}
inline void no_container_action_beta_CtoS::handle(){}

inline void open_container_CtoS::handle() {}
inline void close_container_CtoS::handle() {}

} // Item
#endif


#if DC_SERVER

#include <state/server_state.hpp>
#include <item/_interface.hpp>
#include <item/server.hpp>

namespace Item
{

inline void container_action_alpha_CtoS::handle()
{
    printf("container alpha action\n");
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = alpha_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = beta_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }
    
    ItemID hand_item = get_agent_hand(a->id);

    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void nanite_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = nanite_alpha_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    if (action == PURCHASE_ITEM_FROM_NANITE)
    {
        // do purchase action
        purchase_item_from_nanite(a->id, slot);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void nanite_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = nanite_beta_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }
    
    ItemID hand_item = get_agent_hand(a->id);

    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    if (action == PURCHASE_ITEM_FROM_NANITE)
    {
        // do purchase action
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void craft_container_action_alpha_CtoS::handle()
{
    printf("craft alpha action received\n");
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    printf("agent not null\n");
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;
    printf("container not null id // is owned\n");

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;
    printf("container not null\n");
    ContainerActionType action = craft_input_alpha_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        printf("actions dont match\n");
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        printf("hands dont match\n");
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void craft_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = craft_input_beta_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }
    
    ItemID hand_item = get_agent_hand(a->id);

    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void craft_item_from_bench_action_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    craft_item_from_bench(a->id, container_id, slot);
}

inline void no_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;

    ContainerActionType action = no_container_alpha_action_decision_tree(a->id, client_id);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }
}

inline void no_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;

    ContainerActionType action = no_container_beta_action_decision_tree(a->id, client_id);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }
}

inline void open_container_CtoS::handle()
{
    if (container_id == NULL_CONTAINER) return;
    
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;

    bool in_reach = agent_in_container_range(a->id, container_id);
    if (!in_reach)
    {
        send_open_container_failed(a->client_id, container_id, event_id);
        return;
    }

    bool opened = agent_open_container(a->id, container_id);
    if (!opened)
    {
        send_open_container_failed(a->client_id, container_id, event_id);
        return;
    }

    send_container_open(a->id, container_id);
}

inline void close_container_CtoS::handle()
{
    if (container_id == NULL_CONTAINER) return;

    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;

    agent_close_container(a->id, container_id);
}

} // Item
#endif
