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
    // TODO -- ownership checks
    ItemContainer* container = item_container_list->get(container_id);
    if (container == NULL) return;
    // check if failed
    ContainerActionType action = alpha_action_decision_tree(a->id, client_id, container_id, slot);
    ItemID slot_item = container->get_item(slot);
    ItemID hand_item = get_agent_hand(a->id);
    if (
        this->action != action
     || slot_type != get_item_type(slot_item)
     || slot_stack != get_stack_size(slot_item)
     || hand_type != get_item_type(hand_item)
     || hand_stack != get_stack_size(hand_item)
    ) send_container_failed_action(client_id, event_id);
}

inline void container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    // check if failed
    ItemContainer* container = item_container_list->get(container_id);
    if (container == NULL) return;
    // check if failed
    ContainerActionType action = beta_action_decision_tree(a->id, client_id, container_id, slot);
    ItemID slot_item = container->get_item(slot);
    ItemID hand_item = get_agent_hand(a->id);
    if (
        this->action != action
     || slot_type != get_item_type(slot_item)
     || slot_stack != get_stack_size(slot_item)
     || hand_type != get_item_type(hand_item)
     || hand_stack != get_stack_size(hand_item)
    ) send_container_failed_action(client_id, event_id);
}

} // Item
#endif
