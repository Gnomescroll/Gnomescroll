#include "callbacks.hpp"

#include <item/toolbelt/common/types.hpp>
#include <entity/object/main.hpp>
#include <entity/objects/fabs/constants.hpp>

#include <physics/ray_trace/ray_trace.hpp>

#include <t_mech/_interface.hpp>

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

void fire_close_range_weapon(ItemID item_id, int item_type)
{
    ClientState::playerAgent_state.action.fire_close_range_weapon(item_type);    
    update_predicted_durability();
}

// IG_MINING_LASER

void tick_mining_laser(int agent_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    a->event.tick_mining_laser();
}

void tick_local_mining_laser(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::playerAgent_state.action.tick_mining_laser();
}

void trigger_mining_laser(int agent_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    Agent_state* a = ClientState::agent_list->get(agent_id);
    if (a == NULL) return;
    a->event.fired_mining_laser();
}

void trigger_local_mining_laser(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    fire_close_range_weapon(item_id, item_type);
}

void begin_mining_laser(int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::playerAgent_state.action.begin_mining_laser();
}

void end_mining_laser(int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::playerAgent_state.action.end_mining_laser();
}

// IG_PLACER

void trigger_local_block_placer(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_PLACER);
    ClientState::playerAgent_state.action.set_block(item_id);
}

// IG_DEBUG

void trigger_local_location_pointer(ItemID item_id, int item_type)
{
    /*
        DEBUG STUFF HERE!!!
    */
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_DEBUG);
    ClientState::set_location_pointer();


    Vec3 pos = agent_camera->get_position();
    Vec3 dir = agent_camera->forward_vector();

    float d;
    t_mech::ray_cast_mech(pos.x,pos.y,pos.z, dir.x,dir.y,dir.z, &d);

}

void trigger_local_admin_block_placer(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_DEBUG);
    ClientState::playerAgent_state.action.admin_set_block();
}

// IG_GRENADE_LAUNCHER

void trigger_local_plasma_grenade(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_GRENADE_LAUNCHER);
    ClientState::playerAgent_state.action.throw_grenade();
}

// IG_HITSCAN_WEAPON

void trigger_local_hitscan_laser(ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_HITSCAN_WEAPON);
    ClientState::playerAgent_state.action.hitscan_laser(item_type);
}

#endif

#if DC_SERVER

void decrement_durability(int agent_id, ItemID item_id, int item_type)
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

void decrement_stack(int agent_id, ItemID item_id, int item_type)
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

void consume_item(int agent_id, ItemID item_id, int item_type)
{
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    bool consumed = a->status.consume_item(item_id);
    if (!consumed) return;
    decrement_stack(agent_id, item_id, item_type);
}

void apply_charge_pack_to_teammates(int agent_id, ItemID item_id, int item_type)
{
    Agent_state* a = ServerState::agent_list->get(agent_id);
    if (a == NULL) return;
    int teammate_id = Hitscan::against_agents(
        a->get_camera_position(), a->forward_vector(),
        APPLY_REPAIR_KIT_MAX_DISTANCE, a->id);
    if (teammate_id == NO_AGENT) return;
    consume_item(teammate_id, item_id, item_type);
}

// simple creator for objects
static class Objects::Object* place_object(int agent_id, ItemID item_id, int item_type, const ObjectType object_type, const float object_height)
{
    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return NULL;
    
    const int max_dist = 4.0f;
    const int z_low = 4;
    const int z_high = 3;
    int* b = a->nearest_open_block(max_dist, z_low, z_high);
    if (b == NULL) return NULL;
    
    // must be placed on solid block
    if (b[2] <= 0) return NULL;  // can't place on nothing
    if (!isSolid(b[0], b[1], b[2]-1)) return NULL;
    
    // make sure will fit height
    int h = (int)ceil(object_height);
    GS_ASSERT(h > 0);
    if (h <= 0) h = 1;
    for (int i=0; i<h; i++)
        if (t_map::get(b[0], b[1], b[2] + i) != 0)
            return NULL;
    
    // check against all known spawners & energy cores
    if (Objects::point_occupied_by_type(OBJECT_AGENT_SPAWNER, b[0], b[1], b[2]))
        return NULL;
    if (Objects::point_occupied_by_type(OBJECT_ENERGY_CORE, b[0], b[1], b[2]))
        return NULL;

    class Objects::Object* obj = Objects::create(object_type);
    GS_ASSERT(obj != NULL);
    if (obj == NULL) return NULL;
    using Components::PhysicsComponent;
    PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
    GS_ASSERT(physics != NULL);
    if (physics != NULL)
    {
        Vec3 pos = vec3_init(b[0] + 0.5f, b[1] + 0.5f, b[2]);
        physics->set_position(pos);
    }
    
    // WARNING :: MUST CALL Objects::ready(obj);
    return obj;    
}

// IG_AGENT_SPAWNER

void place_spawner(int agent_id, ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_AGENT_SPAWNER);

    Agent_state* a = ServerState::agent_list->get(agent_id);
    GS_ASSERT(a != NULL);
    if (a == NULL) return;

    class Objects::Object* obj = place_object(agent_id, item_id, item_type, OBJECT_AGENT_SPAWNER, Objects::AGENT_SPAWNER_HEIGHT);
    if (obj == NULL) return;
    Objects::ready(obj);
    
    decrement_stack(agent_id, item_id, item_type);
    
    // select spawner automatically if spawn pt is base
    if (a->status.spawner == BASE_SPAWN_ID)
        a->status.set_spawner(obj->id);
}

// IG_ENERGY_CORE

void place_energy_core(int agent_id, ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_ENERGY_CORE);
    
    class Objects::Object* obj = place_object(agent_id, item_id, item_type, OBJECT_ENERGY_CORE, Objects::ENERGY_CORE_HEIGHT);
    if (obj == NULL) return;
    Objects::ready(obj);
    
    decrement_stack(agent_id, item_id, item_type);
}


void place_mech(int agent_id, ItemID item_id, int item_type)
{
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MECH);
    

    //class Objects::Object* obj = place_object(agent_id, item_id, item_type, OBJECT_ENERGY_CORE, Objects::ENERGY_CORE_HEIGHT);
    //if (obj == NULL) return;
    //Objects::ready(obj);

    //printf("fuck: %i %i \n", item_type, Item::dat_get_item_type("crystal_seed") );

    static int crystal = Item::get_item_type("crystal_seed");

    if(item_type == crystal)
    {
        Agent_state* a = ServerState::agent_list->get(agent_id);
        GS_ASSERT(a != NULL);
        if (a == NULL) return;
        
        const int max_dist = 4.0f;
        const int z_low = 4;
        const int z_high = 3;
        int* b = a->nearest_open_block(max_dist, z_low, z_high);
        if (b == NULL) return;
        
        // must be placed on solid block
        if (b[2] <= 0) return;  // can't place on nothing
        if (!isSolid(b[0], b[1], b[2]-1)) return;

        
        static int crystal_id = t_mech::get_mech_type("blue_crystal");

        if( t_mech::can_place_crystal(b[0],b[1],b[2], 0) == true)
        {
            printf("place crystal: at %d %d %d \n", b[0],b[1],b[2] );
            t_mech::create_crystal(b[0],b[1],b[2], crystal_id);
        
        }
        else
        {

        }
        //decrement_stack(agent_id, item_id, item_type);
    }

    static int crystal2 = Item::get_item_type("crystal_seed2");

    if(item_type == crystal2)
    {
        Agent_state* a = ServerState::agent_list->get(agent_id);
        GS_ASSERT(a != NULL);
        if (a == NULL) return;
    #if 1
        const int max_dist = 4.0f;
        const int z_low = 4;
        const int z_high = 3;
        
        int solid_pos[3];
        int open_pos[3];
        int side = a->get_facing_side(solid_pos, open_pos, max_dist, z_low, z_high);
        
        printf("block: %i %i %i open: %i %i %i side: %i\n", 
            solid_pos[0],solid_pos[1],solid_pos[2], 
            open_pos[0],open_pos[1],open_pos[2],
            side);

        if (side < 0) return;
    #else

        Vec3 pos = a->camera_position();
        Vec3 dir = a->forward_vector();

        float d;

        t_mech::ray_cast_mech(pos.x,pos.y,pos.z, dir.x,dir.y,dir.z, &d)


    #endif
        //t_mech::create_crystal(b[0],b[1],b[2] );
        //decrement_stack(agent_id, item_id, item_type);
    }
}

#endif

}    // Toolbelt
