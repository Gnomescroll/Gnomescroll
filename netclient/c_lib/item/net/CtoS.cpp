#include "CtoS.hpp"

#if DC_CLIENT

namespace Item
{

// dummies
inline void container_action_alpha_CtoS::handle() {}
inline void container_action_beta_CtoS::handle() {}

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
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action;
    switch (container->type)
    {
        case AGENT_CONTAINER:
        case AGENT_TOOLBELT:
        case CRAFTING_BENCH:
            action = alpha_action_decision_tree(a->id, client_id, container_id, slot);
            break;
        case AGENT_NANITE:
            action = nanite_alpha_action_decision_tree(a->id, client_id, container_id, slot);
            break;
        default:
            return;
    }

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

inline void container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (container_id != NULL_CONTAINER && !agent_owns_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action;
    switch (container->type)
    {
        case AGENT_CONTAINER:
        case AGENT_TOOLBELT:
        case CRAFTING_BENCH:
            action = beta_action_decision_tree(a->id, client_id, container_id, slot);
            break;
        case AGENT_NANITE:
            action = nanite_beta_action_decision_tree(a->id, client_id, container_id, slot);
            break;
        default:
            return;
    }

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

} // Item
#endif
