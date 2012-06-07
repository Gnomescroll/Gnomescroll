#include "CtoS.hpp"

#if DC_CLIENT

namespace ItemContainer
{

// dummies
inline void container_action_alpha_CtoS::handle() {}
inline void container_action_beta_CtoS::handle() {}
inline void nanite_container_action_alpha_CtoS::handle() {}
inline void nanite_container_action_beta_CtoS::handle() {}
inline void purchase_item_from_nanite_action_CtoS::handle(){}
inline void craft_container_action_alpha_CtoS::handle() {}
inline void craft_container_action_beta_CtoS::handle() {}
inline void craft_item_from_bench_action_CtoS::handle() {}
inline void no_container_action_alpha_CtoS::handle(){}
inline void no_container_action_beta_CtoS::handle(){}

inline void open_container_CtoS::handle() {}
inline void close_container_CtoS::handle() {}

inline void create_container_block_CtoS::handle() {}

} // ItemContainer
#endif


#if DC_SERVER

#include <state/server_state.hpp>
#include <item/container/_interface.hpp>
#include <item/container/server.hpp>

namespace ItemContainer
{

inline void container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;
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
    if (hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;
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

    if (hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void nanite_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = nanite_alpha_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void nanite_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;
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

    if (hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void purchase_item_from_nanite_action_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    purchase_item_from_nanite(a->id, slot);
}

inline void craft_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;

    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;
    ContainerActionType action = craft_input_alpha_action_decision_tree(a->id, client_id, container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void craft_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;
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

    if (hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if (slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void craft_item_from_bench_action_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    craft_item_from_bench(a->id, container_id, slot);
}

inline void no_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;

    ContainerActionType action = no_container_alpha_action_decision_tree(a->id, client_id);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }
}

inline void no_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;

    ContainerActionType action = no_container_beta_action_decision_tree(a->id, client_id);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand(a->id);
    if (hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
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
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;

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
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;

    if (!agent_can_access_container(a->id, container_id)) return;

    agent_close_container(a->id, container_id);
}

void create_container_block_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (a->status.team == NO_TEAM || a->status.team == 0) return;

    Item::Item* placer = Item::get_item((ItemID)placer_id);
    if (placer == NULL) return;
    Item::ItemAttribute* attr = Item::get_item_attributes(placer->type);
    if (attr == NULL) return;
    int val = attr->placer_block_type_id;

    ItemContainerType container_type = Item::get_container_type_for_block(val);
    if (container_type == CONTAINER_TYPE_NONE) return;

    GS_ASSERT(orientation >= 0 && orientation <= 3);
    if (orientation < 0 || orientation > 3) orientation = 0;
    x = translate_point(x);
    y = translate_point(y);

    // dont allow block to be set on existing block
    if (t_map::get(x,y,z) != 0) return;

    bool collides = false;
    _set(x,y,z, val); // set temporarily to test against
    if (agent_collides_terrain(a)) collides = true; // test against our agent, most likely to collide
    else
    {
        for (int i=0; i<ServerState::agent_list->n_max; i++)
        {
            Agent_state* agent = ServerState::agent_list->a[i];
            if (agent == NULL || agent == a) continue;
            if (agent_collides_terrain(agent))
            {
                collides = true;
                break;
            }
        }
    }
    _set(x,y,z,0);  // unset

    if (collides) return;

    ItemContainerInterface* container = create_container(container_type);
    if (container == NULL) return;

    Toolbelt::use_block_placer(a->id, (ItemID)placer_id);

    t_map::broadcast_set_block_action(x,y,z, val, t_map::TMA_PLACE_BLOCK);
    t_map::broadcast_set_block_palette(x,y,z,val,orientation);

    init_container(container);
    t_map::create_item_container_block(x,y,z, container->type, container->id);
    broadcast_container_create(container->id);

    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

} // ItemContainer
#endif
