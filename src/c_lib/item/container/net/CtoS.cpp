#include "CtoS.hpp"

#if DC_CLIENT

namespace ItemContainer
{

// dummies
inline void container_action_alpha_CtoS::handle() {}
inline void container_action_beta_CtoS::handle() {}
inline void synthesizer_container_action_alpha_CtoS::handle() {}
inline void synthesizer_container_action_beta_CtoS::handle() {}
inline void purchase_item_from_synthesizer_action_CtoS::handle(){}
inline void craft_container_action_alpha_CtoS::handle() {}
inline void craft_container_action_beta_CtoS::handle() {}
inline void craft_item_from_bench_action_CtoS::handle() {}
inline void no_container_action_alpha_CtoS::handle(){}
inline void no_container_action_beta_CtoS::handle(){}
inline void smelter_container_action_alpha_CtoS::handle(){}
inline void smelter_container_action_beta_CtoS::handle(){}

inline void crusher_container_action_alpha_CtoS::handle(){}
inline void crusher_container_action_beta_CtoS::handle(){}
inline void crusher_crush_item_CtoS::handle(){}

inline void open_container_CtoS::handle() {}
inline void close_container_CtoS::handle() {}

inline void create_container_block_CtoS::handle() {}
inline void admin_create_container_block_CtoS::handle() {}

} // ItemContainer
#endif


#if DC_SERVER

#include <state/server_state.hpp>
#include <item/container/_interface.hpp>
#include <item/container/server.hpp>
#include <item/container/config/crusher_dat.hpp>

namespace ItemContainer
{

inline void container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
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

inline void synthesizer_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = synthesizer_alpha_action_decision_tree(a->id, client_id, container_id, slot);

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

inline void synthesizer_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = synthesizer_beta_action_decision_tree(a->id, client_id, container_id, slot);

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

inline void purchase_item_from_synthesizer_action_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    purchase_item_from_synthesizer(a->id, slot);
}

inline void craft_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

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
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    craft_item_from_bench(a->id, container_id, slot);
}

inline void no_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    ContainerActionType action = no_container_alpha_action_decision_tree(a->id, client_id, NULL_CONTAINER, NULL_SLOT);

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

    ContainerActionType action = no_container_beta_action_decision_tree(a->id, client_id, NULL_CONTAINER, NULL_SLOT);

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

inline void smelter_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = smelter_alpha_action_decision_tree(a->id, client_id, container_id, slot);

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

inline void smelter_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = smelter_beta_action_decision_tree(a->id, client_id, container_id, slot);

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

inline void crusher_container_action_alpha_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = crusher_alpha_action_decision_tree(a->id, client_id, container_id, slot);

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

inline void crusher_container_action_beta_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;

    ItemContainerInterface* container = get_container(container_id);
    if (container == NULL) return;

    ContainerActionType action = crusher_beta_action_decision_tree(a->id, client_id, container_id, slot);

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

static const int sq_normals[6][3] =
{
    {1,0,0},
    {-1,0,0},
    {0,1,0},
    {0,-1,0},
    {0,0,1},
    {0,0,-1}
};

inline void crusher_crush_item_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if (container_id != NULL_CONTAINER && !agent_can_access_container(a->id, container_id)) return;
    
    if (!crusher_crush_alpha_action_decision_tree(a->id, client_id, container_id, NULL_SLOT)) return;

    ItemContainerInterface* container = get_container(container_id);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;
    GS_ASSERT(container->type == CONTAINER_TYPE_CRUSHER);
    if (container->type != CONTAINER_TYPE_CRUSHER) return;

    ItemContainerCrusher* crusher = (ItemContainerCrusher*)container;
    
    ItemID item_id = crusher->get_input_slot();
    GS_ASSERT(item_id != NULL_ITEM);
    if (item_id == NULL_ITEM) return;
    int type = Item::get_item_type(item_id);
    GS_ASSERT(type != NULL_ITEM_TYPE);
    if (type == NULL_ITEM_TYPE) return;

    class Item::ItemDrop* drop = get_crusher_drop(type);
    if (drop == NULL) return;   // TOOD -- send "failed" packet

    // unset any velocity state
    drop->vx = 0.0f;
    drop->vy = 0.0f;
    drop->vz = 0.0f;

    int b[3];
    t_map::get_container_location(crusher->id, b);
    Vec3 p = vec3_add(vec3_init(b[0], b[1], b[2]), vec3_init(0.5f, 0.5f, 0.5f));
    if (t_map::get(b[0], b[1], b[2]+1) == 0)
    {   // pop out of the top
        p.x += randf() - 0.5f; 
        p.y += randf() - 0.5f; 
        p.z += 0.51f;
        drop->vz = crusher_item_jump_out_velocity();
    }
    else
    {   // calculate face nearest agent
        int side[3];
        int* c = _farthest_empty_block(a->get_camera_position(), a->forward_vector(), side, AGENT_CONTAINER_REACH, 4, 3);
        if (c == NULL
         || (c[0] != b[0] || c[1] != b[1] || c[2] != b[2])
         || t_map::get(b[0]+side[0], b[1]+side[1], b[2]+side[2]) != 0)
        {   // use any open face
            int i=0;
            for (; i<6; i++)
            {
                if (t_map::get(b[0] + sq_normals[i][0], b[1] + sq_normals[i][1], b[2] + sq_normals[i][2]) == 0)
                {   // velocity in this direction
                    float v = crusher_item_jump_out_velocity();
                    drop->vx = v * side[0];
                    drop->vy = v * side[1];
                    drop->vz = v * side[2];                
                    p.x += 0.51f * side[0] + (abs(side[0])-1) * (randf()-0.5f);
                    p.y += 0.51f * side[1] + (abs(side[1])-1) * (randf()-0.5f);
                    p.z += 0.51f * side[2] + (abs(side[2])-1) * (randf()-0.5f);
                    break;
                }
            }
            if (i == 6)
            {   // no open blocks, give up
                // TODO -- send error message
                return;
            }
        }
        else
        {   // use the side
            float v = crusher_item_jump_out_velocity();
            drop->vx = v * side[0];
            drop->vy = v * side[1];
            drop->vz = v * side[2];
            p.x += 0.51f * side[0] + (abs(side[0])-1) * (randf()-0.5f);
            p.y += 0.51f * side[1] + (abs(side[1])-1) * (randf()-0.5f);
            p.z += 0.51f * side[2] + (abs(side[2])-1) * (randf()-0.5f);
        }
    }

    int stack = Item::get_stack_size(item_id);
    int remaining_stack = Item::consume_stack_item(item_id);
    if (stack > 0 && stack != remaining_stack) Item::send_item_state(item_id);

    drop->drop_item(p);
}

inline void open_container_CtoS::handle()
{
    if (container_id == NULL_CONTAINER) return;
    
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

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

    if (container_id != opened_containers[a->id]) return;

    if (!agent_can_access_container(a->id, container_id)) return;

    agent_close_container(a->id, container_id);
}

void create_container_block_CtoS::handle()
{
    //if (z < 0 || z >= map_dim.z) return;  // this comparison is not needed, because of value range for the data type
    if (z == 0) return; // no floor

    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    Item::Item* placer = Item::get_item((ItemID)placer_id);
    if (placer == NULL) return;
    Item::ItemAttribute* attr = Item::get_item_attributes(placer->type);
    if (attr == NULL) return;
    int val = attr->placer_block_type_id;

    ItemContainerType container_type = Item::get_container_type_for_block(val);
    GS_ASSERT(container_type != CONTAINER_TYPE_NONE);
    if (container_type == CONTAINER_TYPE_NONE) return;

    // these comparisons not needed due to value range of data type
    //GS_ASSERT(orientation >= 0 && orientation <= 3);
    //if (orientation < 0 || orientation > 3) orientation = 0;
    x = translate_point(x);
    y = translate_point(y);

    // dont set on existing block
    if (!t_map::block_can_be_placed(x,y,z,val)) return;

    bool collides = false;
    t_map::set_fast(x,y,z, val); // set temporarily to test against
    if (agent_collides_terrain(a))
        collides = true;  // test against our agent, most likely to collide
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
    t_map::set_fast(x,y,z,0);  // unset

    if (collides) return;

    ItemContainerInterface* container = create_container(container_type);
    GS_ASSERT(container != NULL);
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

void admin_create_container_block_CtoS::handle()
{
    // comparison not needed due to value range of data type
    //if (z < 0 || z >= map_dim.z) return;
 
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    ItemContainerType container_type = Item::get_container_type_for_block(val);
    GS_ASSERT(container_type != CONTAINER_TYPE_NONE);
    if (container_type == CONTAINER_TYPE_NONE) return;

    // comparison not needed due to value range of data type
    //GS_ASSERT(orientation >= 0 && orientation <= 3);
    //if (orientation < 0 || orientation > 3) orientation = 0;
    x = translate_point(x);
    y = translate_point(y);

    // TODO -- when this is a /real/ admin tool, remove this check
    // since we're giving it to players, do this check
    if (!t_map::block_can_be_placed(x,y,z,val)) return;

    bool collides = false;
    t_map::set_fast(x,y,z, val); // set temporarily to test against
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
    t_map::set_fast(x,y,z,0);  // unset

    if (collides) return;

    ItemContainerInterface* container = create_container(container_type);
    GS_ASSERT(container != NULL);
    if (container == NULL) return;

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
