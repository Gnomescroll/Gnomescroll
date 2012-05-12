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
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

    ContainerActionType action = alpha_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        printf("actions do not match\n");
        printf("this->action = %d\n", this->action);
        printf("action = %d\n", action);
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != get_item_type(hand_item) || hand_stack != get_stack_size(hand_item))
    {
        printf("hands do not match\n");
        send_container_failed_action(client_id, event_id);
        return;
    }

    if (action != FULL_HAND_TO_WORLD)
    {
        ItemContainerInterface* container = get_container(container_id);
        if (container == NULL) return;
        ItemID slot_item = container->get_item(slot);
        if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
        {
            printf("slots do not match\n");
            send_container_failed_action(client_id, event_id);
        }
    }
}

inline void container_action_beta_CtoS::handle()
{
    printf("container beta action\n");
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

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

    if (action != FULL_HAND_TO_WORLD)
    {
        ItemContainerInterface* container = get_container(container_id);
        if (container == NULL) return;
        ItemID slot_item = container->get_item(slot);

        if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
            send_container_failed_action(client_id, event_id);
    }
}

inline void nanite_container_action_alpha_CtoS::handle()
{
    printf("nanite alpha action\n");
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

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

    if (action != FULL_HAND_TO_WORLD)
    {
        ItemContainerInterface* container = get_container(container_id);
        if (container == NULL) return;
        ItemID slot_item = container->get_item(slot);
        if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
            send_container_failed_action(client_id, event_id);
    }
}

inline void nanite_container_action_beta_CtoS::handle()
{
    printf("nanite beta action\n");
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

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

    if (action != FULL_HAND_TO_WORLD)
    {
        ItemContainerInterface* container = get_container(container_id);
        if (container == NULL) return;
        ItemID slot_item = container->get_item(slot);

        if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
            send_container_failed_action(client_id, event_id);
    }
}

inline void craft_container_action_alpha_CtoS::handle()
{
    printf("craft alpha action\n");
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = craft_input_alpha_action_decision_tree(a->id, client_id, container_id, slot);

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

    if (action != FULL_HAND_TO_WORLD)
    {
        ItemContainerInterface* container = get_container(container_id);
        if (container == NULL) return;
        ItemID slot_item = container->get_item(slot);
        if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
            send_container_failed_action(client_id, event_id);
    }
}

inline void craft_container_action_beta_CtoS::handle()
{
    printf("craft beta action\n");
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

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

    if (action != FULL_HAND_TO_WORLD)
    {
        ItemContainerInterface* container = get_container(container_id);
        if (container == NULL) return;
        ItemID slot_item = container->get_item(slot);

        if (slot_type != get_item_type(slot_item) || slot_stack != get_stack_size(slot_item))
            send_container_failed_action(client_id, event_id);
    }
}

inline void craft_item_from_bench_action_CtoS::handle()
{
    printf("craft from bench\n");
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

    craft_item_from_bench(a->id, container_id, slot);
}

} // Item
#endif
