/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
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

} // ItemContainer
#endif


#if DC_SERVER

#include <agent/_interface.hpp>
#include <state/server_state.hpp>
#include <item/container/_interface.hpp>
#include <item/container/server.hpp>
#include <item/container/config/crusher_dat.hpp>
#include <sound/sound.hpp>
#include <animations/_interface.hpp>
#include <agent/net_agent.hpp>

namespace ItemContainer
{

inline void container_action_alpha_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)(ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = alpha_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);
    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void container_action_beta_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = beta_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);

    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void synthesizer_container_action_alpha_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = synthesizer_alpha_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);
    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void synthesizer_container_action_beta_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = synthesizer_beta_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);

    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void purchase_item_from_synthesizer_action_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    purchase_item_from_synthesizer(a->id, slot);
}

inline void craft_container_action_alpha_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;
    ContainerActionType action = craft_input_alpha_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);
    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void craft_container_action_beta_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = craft_input_beta_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);

    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void craft_item_from_bench_action_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    craft_item_from_bench(a->id, (ItemContainerID)container_id, slot);
}

inline void no_container_action_alpha_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    ContainerActionType action = no_container_alpha_action_decision_tree(a->id, client_id, NULL_CONTAINER, NULL_SLOT);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);
    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }
}

inline void no_container_action_beta_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    ContainerActionType action = no_container_beta_action_decision_tree(a->id, client_id, NULL_CONTAINER, NULL_SLOT);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);
    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }
}

inline void smelter_container_action_alpha_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = smelter_alpha_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);
    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void smelter_container_action_beta_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = smelter_beta_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);

    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void crusher_container_action_alpha_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = crusher_alpha_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);
    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
        send_container_failed_action(client_id, event_id);
}

inline void crusher_container_action_beta_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    if (container == NULL) return;

    ContainerActionType action = crusher_beta_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, slot);

    if (this->action != action)
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID hand_item = get_agent_hand_item(a->id);

    if ((ItemType)hand_type != Item::get_item_type(hand_item) || hand_stack != Item::get_stack_size(hand_item))
    {
        send_container_failed_action(client_id, event_id);
        return;
    }

    ItemID slot_item = container->get_item(slot);
    if ((ItemType)slot_type != Item::get_item_type(slot_item) || slot_stack != Item::get_stack_size(slot_item))
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

void crusher_crush_item_CtoS::handle()
{
    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;
    if ((ItemContainerID)container_id != NULL_CONTAINER && !agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    if (!crusher_crush_alpha_action_decision_tree(a->id, client_id, (ItemContainerID)container_id, NULL_SLOT)) return;

    ItemContainerInterface* container = get_container((ItemContainerID)container_id);
    IF_ASSERT(container == NULL) return;
    IF_ASSERT(container->type != name::crusher) return;

    ItemContainerCrusher* crusher = (ItemContainerCrusher*)container;

    ItemID item_id = crusher->get_input_slot();
    IF_ASSERT(item_id == NULL_ITEM) return;
    ItemType type = Item::get_item_type(item_id);
    IF_ASSERT(type == NULL_ITEM_TYPE) return;

    Vec3i b;
    bool found = t_map::get_container_location(crusher->id, b);
    IF_ASSERT(!found) return;
    Vec3 p = vec3_add(vec3_init(b), vec3_init(0.5f));

    int plasma_grenade_type = Item::get_item_type("plasma_grenade");
    GS_ASSERT(plasma_grenade_type != NULL_ITEM_TYPE);
    if (type == plasma_grenade_type)
    {   // explde
        Particle::Grenade* g = Particle::grenade_list->create();
        IF_ASSERT(g == NULL) return;
        g->set_position(p);
        g->owner = a->id;
        g->broadcast();
        int stack_size = Item::get_stack_size(item_id);
        IF_ASSERT(stack_size < 1) return;
        g->explode(stack_size);

        Sound::broadcast_play_3d_sound("plasma_grenade_explode", p);
        if (t_map::get(b) == 0)  // only play anim if block got destroyed
            Animations::broadcast_play_animation("plasma_grenade_explode", p);

        Particle::grenade_list->destroy(g->id);
        Item::destroy_item(item_id);
        return;
    }

    class Item::ItemDrop* drop = get_crusher_drop((ItemType)type);
    if (drop == NULL) return;   // TOOD -- send "failed" packet

    // unset any velocity state
    drop->velocity = vec3_init(0);

    if (t_map::get(b.x, b.y, b.z + 1) == 0)
    {   // pop out of the top
        p.x += randf() - 0.5f;
        p.y += randf() - 0.5f;
        p.z += 0.51f;
        drop->velocity.z = crusher_item_jump_out_velocity();
    }
    else
    {   // calculate face nearest agent
        class RaytraceData data;
        bool collided = raytrace_terrain(a->get_camera_position(),
                                         a->forward_vector(),
                                         AGENT_CONTAINER_REACH, &data);
        Vec3i c = data.get_pre_collision_point();
        Vec3i side = data.get_sides();
        Vec3 vside = vec3_init(side);
        Vec3 avside = vec3_abs(vside);
        Vec3 cside = vec3_add(avside, vec3_init(-1));
        Vec3 offset = vec3_scalar_mult(vside, 0.51f);

        if (!collided || !is_equal(b, c) ||
            t_map::get(vec3i_add(b, side)) != EMPTY_CUBE)
        {   // use any open face
            int i=0;
            for (; i<6; i++)
            {
                Vec3i x;
                for (int k=0; k<3; k++)
                    x.i[k] = b.i[k] + sq_normals[i][k];
                if (t_map::get(x) == EMPTY_CUBE)
                {   // velocity in this direction
                    float v = crusher_item_jump_out_velocity();
                    drop->velocity = vec3_scalar_mult(vside, v);
                    Vec3 jitter = vec3_mult(cside, vec3_rand_center());
                    jitter = vec3_add(jitter, offset);
                    p = vec3_add(p, jitter);
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
            drop->velocity = vec3_scalar_mult(vside, v);
            Vec3 jitter = vec3_mult(cside, vec3_rand_center());
            jitter = vec3_add(jitter, offset);
            p = vec3_add(p, jitter);
        }
    }

    int stack = Item::get_stack_size(item_id);
    int remaining_stack = Item::consume_stack_item(item_id);
    if (stack > 0 && stack != remaining_stack)
        Item::send_item_state(item_id);

    p = translate_position(p);
    drop->drop_item(p);
}

inline void open_container_CtoS::handle()
{
    if ((ItemContainerID)container_id == NULL_CONTAINER) return;

    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    bool in_reach = agent_in_container_range(a->id, (ItemContainerID)container_id);
    if (!in_reach)
    {
        send_open_container_failed(a->client_id, (ItemContainerID)container_id, event_id);
        return;
    }

    bool opened = agent_open_container(a->id, (ItemContainerID)container_id);
    if (!opened)
    {
        send_open_container_failed(a->client_id, (ItemContainerID)container_id, event_id);
        return;
    }

    send_container_open(a->id, (ItemContainerID)container_id);
}

inline void close_container_CtoS::handle()
{
    if ((ItemContainerID)container_id == NULL_CONTAINER) return;

    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL) return;
    if (a->status.dead) return;

    if ((ItemContainerID)container_id != opened_containers[a->id]) return;

    if (!agent_can_access_container(a->id, (ItemContainerID)container_id)) return;

    agent_close_container_silent(a->id, (ItemContainerID)container_id);
}

void create_container_block_CtoS::handle()
{
    IF_ASSERT(!is_valid_z(this->position)) return;
    if (this->position.z == 0) return;  // no floor

    Agents::Agent* a = NetServer::agents[client_id];
    if (a == NULL || a->status.dead) return;

    Item::Item* placer = Item::get_item((ItemID)placer_id);
    if (placer == NULL) return;
    Item::ItemAttribute* attr = Item::get_item_attributes(placer->type);
    IF_ASSERT(attr == NULL) return;
    CubeType cube_type = attr->cube_type;

    ItemContainerType container_type = t_map::get_container_type_for_cube(cube_type);
    IF_ASSERT(container_type == NULL_CONTAINER_TYPE) return;
    IF_ASSERT(orientation < 0 || orientation > 3) orientation = 0;

    Vec3i position = translate_position(this->position);

    // dont set on existing block
    if (!t_map::block_can_be_placed(position, cube_type)) return;

    bool collides = false;
    if (t_map::isSolid(cube_type))
    {
        t_map::set_fast(position, ERROR_CUBE); // set temporarily to test against
        if (agent_collides_terrain(a))
            collides = true;  // test against our agent, most likely to collide
        else
        {
            for (size_t i=0; i<Agents::agent_list->max; i++)
            {
                class Agents::Agent* agent = &Agents::agent_list->objects[i];
                if (agent->id == Agents::agent_list->null_id || agent == a) continue;
                if (agent_collides_terrain(agent))
                {
                    collides = true;
                    break;
                }
            }
        }
        t_map::set_fast(position, EMPTY_CUBE);  // unset
    }

    if (collides) return;

    ItemContainerInterface* container = create_container(container_type);
    IF_ASSERT(container == NULL) return;

    Toolbelt::use_block_placer(a->id, (ItemID)placer_id);

    t_map::broadcast_set_block_action(position, cube_type, TMA_PLACE_BLOCK);
    t_map::broadcast_set_block_palette(position, cube_type, orientation);

    t_map::create_item_container_block(position, container->type, container->id);
    broadcast_container_create(container->id);

    agent_placed_block_StoC msg;
    msg.id = a->id;
    msg.broadcast();
}

} // ItemContainer
#endif
