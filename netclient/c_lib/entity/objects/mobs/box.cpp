#include "robot_box.hpp"

#include <c_lib/entity/object/object.hpp>
#include <c_lib/entity/object/helpers.hpp>
#include <c_lib/entity/constants.hpp>

namespace Objects
{

static void set_mob_robot_box_properties(Object* object)
{
    const int n_components = 5;
    object->init(n_components):
    add_component_to_object(object, COMPONENT_VOXEL_MODEL);
    add_component_to_object(object, COMPONENT_HIT_POINTS);
    add_component_to_object(object, COMPONENT_POSITION_MOMENTUM_CHANGED);
    add_component_to_object(object, COMPONENT_TARGETING);
    add_component_to_object(object, COMPONENT_DIMENSION);

    object->tick = &tick_mob_robot_box;
    object->update = &update_mob_robot_box;
}

Object* create_mob_robot_box()
{
    // initialize object
    ObjectType type = OBJECT_MONSTER_BOX;
    Object* obj = object_list->create(type);
    if (obj == NULL) return NULL;
    set_mob_robot_box_properties(obj);
    return obj;
}

void ready_mob_robot_box(Object* object)
{
    using Components::VoxelModelComponent;
    using Components::PhysicsComponent;
    
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
    PhysicsComponent* physics = (PhysicsComponent*)object->get_component_interface(COMPONENT_INTERFACE_PHYSICS);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();
    
    vox->vox = bornVox(vox->vox_dat, object->id, object->type);
    bornSetVox(vox->vox, vox->init_hitscan, vox->init_draw);
    bornUpdateVox(vox->vox, position, angles.x, angles.y);

    #if DC_SERVER
    //object->broadcastCreate();
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
void client_tick_mob_robot_box()
{
    //// must stay on ground -- apply terrain collision
    //// wander randomly (TODO: network model with destinations)
    //// TODO -- aggro component

    //// save current target state, will use this to decide if need to send packet
    //bool was_on_target = this->locked_on_target;
    //int old_target_id = this->target_id;
    //int old_target_type = this->target_type;
    
    //Agent_state* agent = NULL;
    //if (this->locked_on_target)
    //{   // target locked
        //// check target still exists
        //if (this->target_type == OBJECT_AGENT)
            //agent = STATE::agent_list->get(this->target_id);
        //if (agent == NULL
        //|| vec3_distance_squared(agent->get_center(), this->get_center(BOX_PART_BODY)) > this->speed*this->speed)
            //this->locked_on_target = false;
    //}

    //if (!this->locked_on_target)
    //{   // no target found
        //// look for target
        //ObjectState* state = this->state();
        //Vec3 direction;
        //agent = this->acquire_target(
            //state->id, state->type, this->get_team(), this->spatial.camera_z(),
            //this->get_position(),
            //state->accuracy_bias, state->sight_range,
            //state->attack_enemies, state->attack_random,
            //&direction
        //);
    //}

    //if (agent != NULL)
    //{   // target found
        //// lock target
        //this->locked_on_target = true;
        //this->en_route = false;

        //this->target_id = agent->id;
        //this->target_type = OBJECT_AGENT;

        //// send target packet
        //if (!was_on_target || old_target_id != this->target_id || old_target_type != this->target_type)
        //{
            //object_choose_target_StoC msg;
            //msg.id = this->_state.id;
            //msg.type = this->_state.type;
            //msg.target_id = this->target_id;
            //msg.target_type = this->target_type;
            //msg.broadcast();
        //}
    //}
    
    //if (this->locked_on_target)
    //{   // target is locked
        //// face target
        //Vec3 direction = vec3_sub(agent->get_center(), this->get_center(BOX_PART_BODY));
        //if (vec3_length(direction))
        //{
            //normalize_vector(&direction);
            //float theta,phi;
            //vec3_to_angles(direction, &theta, &phi);
            //this->set_angles(theta, phi, 0);
            //this->direction = direction;
        //}

        //if (this->can_fire())
        //{
            //ObjectState* state = this->state();
            //this->fire_on_known_target(
                //state->id, state->type, this->spatial.camera_z(), this->get_position(),
                //this->direction, state->accuracy_bias, state->sight_range, agent
            //);
        //}
    //}
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
void client_tick_mob_robot_box()
{
    ////return;
    //if (this->locked_on_target)
    //{   // target locked
        //if (this->target_type != OBJECT_AGENT) return;    // TODO -- more objects
        //Agent_state* agent = ClientState::agent_list->get(this->target_id);
        //if (agent == NULL) return;
    
        //// face target
        //Vec3 agent_position = agent->get_center();
        //Vec3 position = this->get_center(BOX_PART_BODY);
        //Vec3 direction = vec3_sub(agent_position, position);
        //if (vec3_length(direction))
        //{
            //float theta,phi;
            //normalize_vector(&direction);
            //this->direction = direction;
            //vec3_to_angles(direction, &theta, &phi);
            //this->set_angles(theta, phi, 0);
        //}
        //return; // do nothing else
    //}

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
    
    PCP* physics =
        (PCP*)object->get_component(COMPONENT_POSITION_MOMENTUM_CHANGED);
    VoxelModelComponent* vox = (VoxelModelComponent*)object->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);

    updateVox(vox->vox, pcp->get_position(), pcp->get_angles(), pcp->changed);
    pcp->set_changed(false);    // reset changed state
}


} // Objects

