#include "box.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/objects/mobs/constants.hpp>
#include <c_lib/entity/components/health.hpp>
#include <c_lib/entity/components/dimension.hpp>
#include <c_lib/entity/components/voxel_model.hpp>
#include <c_lib/entity/components/targeting.hpp>

namespace Objects
{

static void set_mob_robot_box_properties(Object* object)
{
    const int n_components = 5;
    object->init(n_components);

    add_component_to_object(object, COMPONENT_POSITION_MOMENTUM_CHANGED);

    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)add_component_to_object(object, COMPONENT_DIMENSION);
    dims->height = MONSTER_BOX_HEIGHT;
    dims->camera_height = MONSTER_BOX_CAMERA_HEIGHT;

    using Components::VoxelModelComponent;
    VoxelModelComponent* vox = (VoxelModelComponent*)add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    vox->vox_dat = &VoxDats::robot_box;
    vox->init_hitscan = MONSTER_BOX_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_BOX_INIT_WITH_DRAW;

    using Components::HitPointsHealthComponent;
    HitPointsHealthComponent* health = (HitPointsHealthComponent*)add_component_to_object(object, COMPONENT_HIT_POINTS);
    health->health = MONSTER_BOX_MAX_HEALTH;
    health->max_health = MONSTER_BOX_MAX_HEALTH;
    
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* target = (WeaponTargetingComponent*)add_component_to_object(object, COMPONENT_WEAPON_TARGETING);
    target->target_acquisition_probability = MONSTER_BOX_TARGET_ACQUISITION_PROBABILITY;
    target->fire_rate_limit = MONSTER_BOX_FIRE_RATE_LIMIT;
    target->accuracy_bias = MONSTER_BOX_ACCURACY_BIAS;
    target->sight_range = MONSTER_BOX_SIGHT_RANGE;
    target->attacks_enemies = MONSTER_BOX_ATTACK_ONLY_ENEMIES;
    target->attack_at_random = MONSTER_BOX_ATTACK_AT_RANDOM;
    // we dont have ID yet, need to set that in the ready() call
    target->attacker_properties.type = OBJECT_MONSTER_BOX;
    target->attacker_properties.block_damage = MONSTER_BOX_TERRAIN_DAMAGE;
    target->attacker_properties.agent_damage = MONSTER_BOX_AGENT_DAMAGE;
    target->attacker_properties.voxel_damage_radius = MONSTER_BOX_VOXEL_DAMAGE_RADIUS;
    target->attacker_properties.agent_protection_duration = MONSTER_BOX_AGENT_IMMUNITY_DURATION;
    target->attacker_properties.terrain_modification_action = t_map::TMA_MONSTER_BOX;

    object->tick = &tick_mob_robot_box;
    object->update = &update_mob_robot_box;

    object->create = create_packet_momentum_angles;
    object->state = state_packet_momentum_angles;
}

Object* create_mob_robot_box()
{
    ObjectType type = OBJECT_MONSTER_BOX;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_robot_box_properties(obj);
    return obj;
}

void ready_mob_robot_box(Object* object)
{
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* target = (WeaponTargetingComponent*)object->get_component_interface(COMPONENT_INTERFACE_TARGETING);
    target->attacker_properties.id = object->id;
    
    using Components::VoxelModelComponent;
    using Components::PhysicsComponent;
    
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();
    
    vox->ready(position, angles.x, angles.y);

    #if DC_SERVER
    object->broadcastCreate();
    #endif
}

void die_mob_robot_box(Object* object)
{
    #if DC_CLIENT
    // animation explode
    #endif

    #if DC_SERVER
    //boxDropItem(); -- todo, item drop component
    object->broadcastDeath();
    // TODO -- spawned_by/parent/child component
    //Object* spawner = object_list->get(OBJECT_MONSTER_SPAWNER, spawned->spawner);
    //if (spawner != NULL)
    //{
        //using Components::MonsterSpawnerComponent;
        //MonsterSpawnerComponent* spawn = (MonsterSpawnerComponent*)spawner->get_component(COMPONENT_MONSTER_SPAWNER);
        //spawn->lose_child(object->type, object->id);
    //}
    #endif
}

/* MAJOR TODO */
// migrate all data tacked on to Monsters::Box into a component
// traveling component
// attach the rest on targeting

#if DC_SERVER
void server_tick_mob_robot_box(Object* object)
{
    // must stay on ground -- apply terrain collision
    // wander randomly (TODO: network model with destinations)
    // TODO -- aggro component

    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    Vec3 position = physics->get_position();
    
    using Components::DimensionComponent;
    DimensionComponent* dims = (DimensionComponent*)object->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
    position.z += dims->get_camera_height();
    
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* targeting = (WeaponTargetingComponent*)object->get_component(COMPONENT_WEAPON_TARGETING);

    // save current target state, will use targeting to decide if need to send packet
    bool was_on_target = targeting->locked_on_target;
    int old_target_id = targeting->target_id;
    int old_target_type = targeting->target_type;
    
    Agent_state* agent = NULL;
    if (targeting->locked_on_target)
    {   // target locked
        // check target still exists
        if (targeting->target_type == OBJECT_AGENT)
            agent = STATE::agent_list->get(targeting->target_id);
        if (agent == NULL
        || vec3_distance_squared(agent->get_center(), position) > targeting->sight_range*targeting->sight_range)
            targeting->locked_on_target = false;
    }

    if (!targeting->locked_on_target)
    {   // no target found
        // look for target
        targeting->lock_target(position);
    }

    if (targeting->target_type != OBJECT_NONE)
    {   // target found
        // lock target
        targeting->locked_on_target = true;
        //this->en_route = false;

        // send target packet
        if (!was_on_target || old_target_id != targeting->target_id || old_target_type != targeting->target_type)
        {
            targeting->broadcast_target_choice();
        }
    }
    
    if (targeting->locked_on_target)
    {   // target is locked
        // face target
        if (agent != NULL) // TODO -- multiple target
        {
            Vec3 direction = vec3_sub(agent->get_center(), position);
            if (vec3_length(direction))
            {
                normalize_vector(&direction);
                float theta,phi;
                vec3_to_angles(direction, &theta, &phi);
                physics->set_angles(vec3_init(theta, phi, 0));
                targeting->target_direction = direction;
            }

            if (targeting->can_fire())
            {
                targeting->fire_on_target(position);
            }
        }
    }
    //else if (this->en_route)
    //{   // face destination
        //float theta, phi;
        //vec3_to_angles(this->direction, &theta, &phi);
        //this->set_angles(theta, phi, 0);
    //}

    //if (!this->en_route && !this->locked_on_target)
    //{   // no destination, no target
        //// choose destination
        //const int walk_len = BOX_WALK_RANGE;
        //int dx = randrange(0,walk_len) - walk_len/2;
        //int dy = randrange(0,walk_len) - walk_len/2;
        //this->destination = vec3_add(this->get_position(), vec3_init(((float)dx)+randf(), ((float)dy)+randf(),0));
        //// clamp
        //if (this->destination.x < 0) this->destination.x = 0;
        //if (this->destination.x >= map_dim.x) this->destination.x = map_dim.x -1;
        //if (this->destination.y < 0) this->destination.y = 0;
        //if (this->destination.y >= map_dim.y) this->destination.y = map_dim.y -1;
        //this->destination.z = (float)t_map::get_highest_open_block(this->destination.x,this->destination.y);
        //if (this->destination.z < 0) this->destination.z = 0;

        //this->en_route = true;
        //this->at_destination = false;
        
        //Vec3 direction = vec3_sub(this->destination, this->get_position());
        //float len = vec3_length(direction);
        //this->ticks_to_destination = (int)ceil(len/this->speed);
        //if (len)
        //{
            //direction.z = 0;
            //normalize_vector(&direction);
            //this->direction = direction;
        //}

        //// send destination packet
        //// TODO
        //object_choose_destination_StoC msg;
        //msg.x = destination.x;
        //msg.y = destination.y;
        //msg.z = destination.z;
        //msg.id = this->_state.id;
        //msg.type = this->_state.type;
        //msg.ticks = this->ticks_to_destination;
        //msg.broadcast();
        //// TODO
    //}

    //if (!this->at_destination)
    //{   // check if at destination
        //float dist = vec3_distance_squared(this->destination, this->get_position());
        //if (dist < 1.0f)    // TODO Margin
        //{
            //this->en_route = false;
            //this->at_destination = true;
            //this->set_momentum(0,0,0);
        //}
    //}

    //if (this->en_route)
    //{   // move towards destination
        //Vec3 position;
        //Vec3 momentum;
        //this->en_route = moveBox(this->get_position(), this->direction, this->speed, &position, &momentum);
        //this->set_position(position.x, position.y, position.z);
        //this->set_momentum(momentum.x, momentum.y, momentum.z);
    //}

    ////if (this->spatial.properties.changed)
        ////this->broadcastState(); // send state packet if state changed
    ////else if (this->canSendState())
        ////this->broadcastState(); // send state packet every N ticks
}
#endif

#if DC_CLIENT
void client_tick_mob_robot_box(Object* object)
{
    using Components::WeaponTargetingComponent;
    WeaponTargetingComponent* targeting = (WeaponTargetingComponent*)object->get_component(COMPONENT_WEAPON_TARGETING);

    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    
    if (targeting->locked_on_target)
    {   // target locked
        if (targeting->target_type != OBJECT_AGENT) return;    // TODO -- more objects
        Agent_state* agent = ClientState::agent_list->get(targeting->target_id);
        if (agent == NULL) return;
        Vec3 agent_position = agent->get_center();

        using Components::VoxelModelComponent;
        VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component(COMPONENT_VOXEL_MODEL);
        
        // face target
        Vec3 position = vox->get_center();
        Vec3 direction = vec3_sub(agent_position, position);
        if (vec3_length(direction))
        {
            float theta,phi;
            normalize_vector(&direction);
            targeting->target_direction = direction;
            vec3_to_angles(direction, &theta, &phi);
            physics->set_angles(vec3_init(theta, phi, 0));
        }
        return; // do nothing else
    }

    //if (!this->at_destination)
    //{   // check if at destination
        //float dist = vec3_distance_squared(this->destination, this->get_position());
        //if (dist < 1.0f)    // TODO Margin
        //{   // at destination, stop
            //this->en_route = false;
            //this->at_destination = true;
            //this->set_momentum(0,0,0);
        //}
    //}

    //if (this->en_route)
    //{   // move towards destination
        //Vec3 position;
        //Vec3 momentum;
        //this->en_route = moveBox(this->get_position(), this->direction, this->speed, &position, &momentum);
        //this->set_position(position.x, position.y, position.z);
        //this->set_momentum(momentum.x, momentum.y, momentum.z);

        //if (vec3_length(momentum))
        //{
            //momentum.z = 0;
            //normalize_vector(&momentum);
            //this->direction = momentum;
        //}

        //// face in direction of movement
        //float theta, phi;
        //vec3_to_angles(this->direction, &theta, &phi);
        //this->set_angles(theta, phi, 0);
    //}

}
#endif


void tick_mob_robot_box(Object* object)
{
    #if DC_SERVER
    server_tick_mob_robot_box(object);
    #endif
    #if DC_CLIENT
    client_tick_mob_robot_box(object);
    #endif
}

void update_mob_robot_box(Object* object)
{
    typedef Components::PositionMomentumChangedPhysicsComponent PCP;
    using Components::VoxelModelComponent;
    
    PCP* physics = (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->changed);
    physics->changed = false;    // reset changed state
}


} // Objects

