#include "callbacks.hpp"

#include <agent/_interface.hpp>
#include <item/toolbelt/common/types.hpp>
#include <entity/object/main.hpp>
#include <entity/objects/fabs/constants.hpp>
#include <physics/ray_trace/ray_trace.hpp>
#include <t_mech/_interface.hpp>
#include <physics/pathfinding.hpp>
#include <t_map/_interface.hpp>
#include <t_map/t_map.hpp>
#if DC_CLIENT
# include <hud/cube_selector.hpp>
#endif

namespace Toolbelt
{

// no need to validate arguments for callbacks
// all args are prevalidated and processed

#if DC_CLIENT

static void update_predicted_durability()
{
    using ItemContainer::ItemContainerUIInterface;
    ItemContainerUIInterface* container = ItemContainer::get_container_ui(toolbelt_id);
    if (container == NULL) return;
    int durability = container->get_slot_durability(selected_slot);
    if (durability == NULL_DURABILITY) return;
    durability -= 1;
    ItemContainer::set_ui_slot_durability(toolbelt_id, selected_slot, durability);
}

void fire_close_range_weapon(ItemID item_id, ItemType item_type)
{
    ClientState::player_agent.action.fire_close_range_weapon(item_type);
    update_predicted_durability();
}

// IG_MINING_LASER

void trigger_local_mining_laser(ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    fire_close_range_weapon(item_id, item_type);
}

void begin_local_mining_laser(ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::player_agent.action.begin_mining_laser();
}

void end_local_mining_laser(ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::player_agent.action.end_mining_laser();
}

void begin_mining_laser(AgentID agent_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;
    a->event.begin_mining_laser();
}

void end_mining_laser(AgentID agent_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    Agents::Agent* a = Agents::get_agent(agent_id);
    if (a == NULL) return;
    a->event.end_mining_laser();
}

// IG_PLACER

void trigger_local_block_placer(ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_PLACER);
    ClientState::player_agent.action.set_block(item_id);
}

// IG_DEBUG

void trigger_local_location_pointer(ItemID item_id, ItemType item_type)
{
    /*
        DEBUG STUFF HERE!!!
    */
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_DEBUG);
    ClientState::set_location_pointer_open_block();

    Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;
    Vec3 pos = you->get_position();
    struct Vec3i start;
    start.x = pos.x;
    start.y = pos.y;
    start.z = pos.z;
    start.z = t_map::get_nearest_surface_block_below(start);
    struct Vec3i end;
    end.x = ClientState::location_pointer.x;
    end.y = ClientState::location_pointer.y;
    end.z = ClientState::location_pointer.z;
    end.z = t_map::get_nearest_surface_block_below(end);
    //end.z = start.z;    // for 2d
    size_t len = 0;
    struct Vec3i* path = Path::get_path_3d_jump(start, end, len);
    Path::print_path(path, len);
    if (ClientState::path != NULL)
        free(ClientState::path);
    ClientState::path = path;
    ClientState::path_len = len;

    //using t_map::GetNearestSurfaceBlockIter;
    //GetNearestSurfaceBlockIter iter = GetNearestSurfaceBlockIter(start);
    //int xxx = GetNearestSurfaceBlockIter::END;
    //while ((xxx = iter.next()) != GetNearestSurfaceBlockIter::END)
        //printf("Iter next: %d\n", xxx);

    //Vec3 pos = agent_camera->get_position();
    //Vec3 dir = agent_camera->forward_vector();
    //float d = 0.0f;
    //int mech_id = 0;
    //t_mech::ray_cast_mech(pos.x,pos.y,pos.z, dir.x,dir.y,dir.z, &mech_id, &d);
}

void trigger_local_beta_location_pointer(ItemID item_id, ItemType item_type)
{
    /*
        DEBUG STUFF HERE!!!
    */

    using ClientState::path;
    using ClientState::path_len;

    if (path == NULL) return;
    if (path_len <= 1) return;
    size_t len = 0;
    struct Vec3i* _path = Path::get_path_3d_jump(path[0], path[path_len-1], len);
    if (_path == NULL) return;
    if (path != NULL)
        free(path);
    path = _path;
    path_len = len;
    Path::print_path(path, len);
}

void trigger_local_admin_block_placer(ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_DEBUG);
    ClientState::player_agent.action.admin_set_block();
}

// IG_GRENADE_LAUNCHER

void trigger_local_plasma_grenade(ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_GRENADE_LAUNCHER);
    ClientState::player_agent.action.throw_grenade();
}

// IG_HITSCAN_WEAPON

void trigger_local_hitscan_laser(ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_HITSCAN_WEAPON);
    ClientState::player_agent.action.hitscan_laser(item_type);
}

// block placer

void select_facing_block(ItemID item_id, ItemType item_type)
{
    static int block_placer_type = Item::get_item_type("block_placer");
    GS_ASSERT(block_placer_type != NULL_ITEM_TYPE);
    GS_ASSERT(item_type == block_placer_type);

    int b[3];
    bool collided = ClientState::player_agent.facing_block(b);
    if (!collided) return;
    CubeType cube_type = t_map::get(b[0], b[1], b[2]);
    HudCubeSelector::cube_selector.set_block_type(cube_type);
}

void toggle_scope(ItemID item_id, ItemType item_type)
{
    if (agent_camera != NULL)
        agent_camera->toggle_zoom();
}


#endif

#if DC_SERVER

void decrement_durability(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    int durability = Item::get_item_durability(item_id);
    if (durability == NULL_DURABILITY) return;
    GS_ASSERT(durability > 0);
    int remaining_durability = Item::consume_durability(item_id, 1, false);
    if (remaining_durability <= 0)
    {
        force_remove_selected_item(agent_id);
        Item::destroy_item(item_id);
    }
    else if (durability != remaining_durability)
        Item::send_item_state(item_id);
}

void decrement_stack(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    int stack_size = Item::get_stack_size(item_id);
    GS_ASSERT(stack_size > 0);
    int remaining_stack_size = Item::consume_stack_item(item_id, 1, false);
    if (remaining_stack_size <= 0)
    {
        force_remove_selected_item(agent_id);
        Item::destroy_item(item_id);
    }
    else if (stack_size != remaining_stack_size)
        Item::send_item_state(item_id);
}

// IG_CONSUMABLE

void consume_item(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;
    bool consumed = a->status.consume_item(item_id);
    if (!consumed) return;
    decrement_stack(agent_id, item_id, item_type);
}

void apply_charge_pack_to_teammates(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    AgentID teammate_id = Hitscan::against_agents(
        a->get_camera_position(), a->forward_vector(),
        APPLY_REPAIR_KIT_MAX_DISTANCE, a->id);
    if (teammate_id == NULL_AGENT) return;
    consume_item(teammate_id, item_id, item_type);
}

// simple creator for objects
static class Entities::Entity* place_object(AgentID agent_id, ItemID item_id, ItemType item_type, const EntityType object_type, const float object_height)
{
    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return NULL;

    const int max_dist = 4.0f;
    int b[3];
    bool collided = a->nearest_open_block(max_dist, b);
    if (!collided) return NULL;

    // must be placed on solid block
    if (b[2] <= 0) return NULL;  // can't place on nothing
    if (!t_map::isSolid(b[0], b[1], b[2]-1)) return NULL;

    // make sure will fit height
    int h = (int)ceil(object_height);
    IF_ASSERT(h <= 0) h = 1;
    for (int i=0; i<h; i++)
        if (t_map::get(b[0], b[1], b[2] + i) != 0)
            return NULL;

    // check against all known spawners & energy cores
    if (Entities::point_occupied_by_type(OBJECT_AGENT_SPAWNER, b[0], b[1], b[2]))
        return NULL;
    if (Entities::point_occupied_by_type(OBJECT_ENERGY_CORE, b[0], b[1], b[2]))
        return NULL;

    class Entities::Entity* obj = Entities::create(object_type);
    IF_ASSERT(obj == NULL) return NULL;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        Vec3 pos = vec3_init(b[0] + 0.5f, b[1] + 0.5f, b[2]);
        physics->set_position(pos);
    }

    // WARNING :: MUST CALL Entities::ready(obj);
    return obj;
}

// IG_AGENT_SPAWNER

void place_spawner(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_AGENT_SPAWNER);

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    class Entities::Entity* obj = place_object(agent_id, item_id, item_type, OBJECT_AGENT_SPAWNER, Entities::AGENT_SPAWNER_HEIGHT);
    if (obj == NULL) return;
    Entities::ready(obj);

    decrement_stack(agent_id, item_id, item_type);

    // select spawner automatically if spawn pt is base
    if (a->status.spawner == BASE_SPAWN_ID)
        a->status.set_spawner(obj->id);
}

// IG_ENERGY_CORE

void place_energy_core(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_ENERGY_CORE);

    class Entities::Entity* obj = place_object(agent_id, item_id, item_type, OBJECT_ENERGY_CORE, Entities::ENERGY_CORE_HEIGHT);
    if (obj == NULL) return;
    Entities::ready(obj);

    decrement_stack(agent_id, item_id, item_type);
}

//IG_MECH_PLACER

void place_mech(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MECH_PLACER);

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    const int max_dist = 4.0f;
    int b[3];
    bool collided = a->nearest_open_block(max_dist, b);
    if (!collided) return;

    // must be placed on solid block
    if (b[2] <= 0) return;  // can't place on nothing. it CAN be placed on top of the map for now
    if (!t_map::isSolid(b[0], b[1], b[2]-1)) return;

    MechType mech_type = Item::get_mech_type(item_type);
    IF_ASSERT(!isValid(mech_type)) return;

    if (!t_mech::can_place_mech(b[0],b[1],b[2], mech_type)) return;

    //printf("place mech %d: at %d %d %d\n", mech_type, b[0],b[1],b[2]);
    bool ret = t_mech::create_mech(b[0],b[1],b[2], mech_type);
    if (ret)
        decrement_stack(agent_id, item_id, item_type);
}

// magic stick

void use_boon_crank(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    Item::Item* item = Item::get_item(item_id);
    if (item->charges <= 0) return;

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    // find an item type. it should not be the same item or an abstract item
    ItemType random_type = NULL_ITEM_TYPE;
    do {
        random_type = (ItemType)randrange(0, MAX_ITEM_TYPES-1);
    } while (!Item::can_be_booned(random_type));

    // place item in front of player, near the head, but at a distance away
    const float distance_from_player = 1.3f;
    // incase first attempt is inside a block
    const float backup_distance_from_player = a->box.box_r * 0.5f;

    // calculate position
    struct Vec3 forward = a->forward_vector();
    struct Vec3 start_position = a->get_position();
    start_position.z += a->box.height * 0.95f;    // near head
    struct Vec3 position = vec3_add(start_position, vec3_scalar_mult(forward, distance_from_player));
    if (t_map::get(position.x, position.y, position.z) != EMPTY_CUBE)
    {
        start_position = a->get_camera_position();
        position = vec3_add(start_position, vec3_scalar_mult(forward, backup_distance_from_player));
    }

    // create item particle, no velocity
    Item::Item* new_item = ItemParticle::create_item_particle(random_type, position, vec3_init(0,0,0));
    IF_ASSERT(new_item == NULL) return;

    // give it a random stack size
    int stack_size = randrange(1, Item::get_max_stack_size(random_type));
    new_item->stack_size = stack_size;

    // consume charge
    item->charges--;
    Item::send_item_charges(item_id);

    Sound::send_play_2d_sound("boon_crank", a->client_id);
    Sound::broadcast_exclude_play_3d_sound("boon_crank", a->get_center(), a->client_id);
}

// IG_PLANT_PLACER

void plant_placer_action(AgentID agent_id, ItemID item_id, ItemType item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_PLANT_PLACER);

    Agents::Agent* a = Agents::get_agent(agent_id);
    IF_ASSERT(a == NULL) return;

    const int max_dist = 4.0f;
    int b[3];
    bool collided = a->nearest_open_block(max_dist, b);
    if (!collided) return;

    // must be placed on solid block
    if (b[2] <= 0) return;  // can't place on nothing. it CAN be placed on top of the map for now
    if (!t_map::isSolid(b[0], b[1], b[2]-1)) return;

    //printf("plant_placer: %d %d %d \n", b[0],b[1],b[2]);

    t_plant::create_plant(b[0],b[1],b[2], 0);   //make plant type variable in future
}

#endif

}    // Toolbelt
