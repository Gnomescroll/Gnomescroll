#pragma once


#include <c_lib/common/enum_types.hpp>
#include <c_lib/common/common.hpp>
#include <c_lib/monsters/constants.hpp>
#include <c_lib/objects/components/target_acquisition/component.hpp>
#include <c_lib/objects/components/voxel/component.hpp>
#include <c_lib/objects/components/tick/components.hpp>
#include <c_lib/objects/common/interface/layers.hpp>
#include <c_lib/objects/common/api/born.hpp>
#include <c_lib/objects/common/api/update.hpp>
#include <c_lib/objects/common/api/tick.hpp>
#include <c_lib/objects/common/net/packets.hpp>
#include <c_lib/agent/agent.hpp>
#include <c_lib/monsters/slime.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/t_map/t_map.hpp>

namespace Monsters {

extern VoxDat box_vox_dat;

void box_shot_object(object_shot_object_StoC* msg);

//void box_received_shoot(Box* box, object_shot_object_StoC* msg)
//{
    //box->en_route = false;  // cancel all motion
    //box->locked_on_target = true;   // flag target lock

    //box->target_id = msg->target_id;    // set target
    //box->target_type = (Object_types)msg->target_type;
//}

//void box_received_move(Box* box, object_destination_StoC* msg)
//{
    //box->en_route = true;
    //box->locked_on_target = false;  // TODO -- moving and locked on target?
//}

class Box:
    public VoxelComponent,
    public TargetAcquisitionComponent,
    public RateLimitedStateBroadcastComponent,
    #if DC_CLIENT
    public AnimationVoxelComponent,
    #endif
    public MonsterInterface
{
    public:

    Vec3 destination;
    bool at_destination;
    bool en_route;

    int target_id;
    Object_types target_type;
    bool locked_on_target;

    void tick()
    {
        // must stay on ground -- apply terrain collision
        // wander randomly (TODO: network model with destinations)
        // TODO -- aggro component
        #if DC_SERVER
        Agent_state* agent = NULL;
        if (this->locked_on_target)
        {   // target locked
            // check target still exists
            if (this->target_type == OBJ_TYPE_AGENT)
                agent = STATE::agent_list->get(this->target_id);
            if (agent == NULL
            || vec3_distance_squared(agent->get_center(), this->get_center(BOX_PART_BODY)) > BOX_SPEED*BOX_SPEED)
                this->locked_on_target = false;
        }

        if (!this->locked_on_target)
        {   // no target found
            // look for target
            ObjectState* state = this->state();
            Vec3 direction;
            agent = this->acquire_target(
                state->id, state->type, this->get_team(), this->camera_z(),
                this->get_position(),
                state->accuracy_bias, state->sight_range,
                state->attack_enemies, state->attack_random,
                &direction
            );
        }

        if (agent != NULL)
        {   // target found
            // lock target
            this->locked_on_target = true;
            this->en_route = false;

            this->target_id = agent->id;
            this->target_type = OBJ_TYPE_AGENT;

            // send target packet
        }
        
        if (this->locked_on_target)
        {   // target is locked
            // face target
            float theta,phi;
            Vec3 direction = vec3_sub(agent->get_center(), this->get_center(BOX_PART_BODY));
            vec3_to_angles(direction, &theta, &phi);
            //Vec3 angles = this->get_angles(); // rho is unused for Box, otherwise, reuse rho from here
            this->set_angles(theta, phi, 0);
        }
        else if (this->en_route)
        {   // face destination
            float theta, phi;
            vec3_to_angles(this->get_momentum(), &theta, &phi);
            this->set_angles(theta, phi, 0);
        }

        if (!this->en_route && !this->locked_on_target)
        {   // no destination, no target
            // choose destination
            const int len = BOX_WALK_RANGE;
            float dx = randrange(0,len) - len/2;
            float dy = randrange(0,len) - len/2;
            float dz = 0;
            this->destination = vec3_add(this->get_position(), vec3_init(dx,dy,dz));
            this->en_route = true;
            this->at_destination = false;

            // clamp
            if (this->destination.x < 0) this->destination.x = 0;
            if (this->destination.x >= map_dim.x) this->destination.x = map_dim.x -1;
            if (this->destination.y < 0) this->destination.y = 0;
            if (this->destination.y >= map_dim.y) this->destination.y = map_dim.y -1;
            if (this->destination.z < 0) this->destination.z = 0;
            if (this->destination.z >= map_dim.z) this->destination.z = map_dim.z -1;

            Vec3 direction = vec3_sub(this->destination, this->get_position());
            normalize_vector(&direction);
            Vec3 momentum = vec3_scalar_mult(direction, BOX_SPEED);
            this->set_momentum(momentum.x, momentum.y, momentum.z);
            // send destination packet
        }

        if (!this->at_destination)
        {
            float dist = vec3_distance_squared(this->destination, this->get_position());
            if (dist < 1.0f)    // TODO Margin
            {
                this->en_route = false;
                this->at_destination = true;
                this->set_momentum(0,0,0);
            }
        }

        if (this->en_route)
        {   // destination set
            // move towards destination
            Vec3 position = vec3_add(this->get_position(), this->get_momentum());
            this->set_position(position.x, position.y, position.z);
        }

        if (this->spatial_properties.changed)
            this->broadcastState(); // send state packet if state changed
        else if (this->canSendState())
            this->broadcastState(); // send state packet every N ticks
        #endif
    }

    void die()
    {
        #if DC_CLIENT
        Vec3 position = this->get_center(BOX_PART_BODY);
        this->animation_voxel_explode(position);
        #endif

        #if DC_SERVER
        //boxDropItem(this->get_position());
        this->broadcastDeath();
        #endif
    }

    void born()
    {
        ObjectState* state = this->state();
        this->voxel_properties.vox = bornVox(this->voxel_properties.vox_dat, state->id, state->type);
        bornSetVox(
            this->voxel_properties.vox,
            this->voxel_properties.init_hitscan,
            this->voxel_properties.init_draw
        );
        bornUpdateVox(this->voxel_properties.vox, this->get_position(),
            this->spatial_properties.angles.x, this->spatial_properties.angles.y); 
    }

    void update()
    {
        updateVox(
            this->voxel_properties.vox, this->get_position(),
            this->spatial_properties.angles, this->spatial_properties.changed
        );
        this->spatial_properties.set_changed(false);
    }
    
    void draw() {/*Empty*/}

    explicit Box(int id)
    :
    at_destination(false), en_route(false),
    target_id(NO_AGENT), target_type(OBJ_TYPE_NONE),
    locked_on_target(false)
    {
        this->_state.id = id;

        this->_state.coin_rule = COINS_ANYONE;

        this->_state.motion_proximity_radius = BOX_MOTION_PROXIMITY_RADIUS;

        // todo -- firing rate / properties
        this->_state.fire_rate_limit = BOX_FIRE_RATE;
        this->_state.accuracy_bias = BOX_ACCURACY_BIAS;
        this->_state.sight_range = BOX_SIGHT_RANGE;
        this->_state.attack_enemies = BOX_ATTACK_ONLY_ENEMIES;    // TODO change attack_enemies to attack_only_enemies
        this->_state.attack_random = BOX_ATTACK_RANDOM;

        // target acquisition stuff
        this->attacker_properties.id = id;
        this->attacker_properties.type = OBJ_TYPE_MONSTER_BOX;
        this->attacker_properties.agent_protection_duration = AGENT_BOX_PROTECTION_DURATION;
        this->attacker_properties.agent_damage = BOX_AGENT_DAMAGE;
        this->attacker_properties.block_damage = BOX_TERRAIN_DAMAGE;
        this->attacker_properties.voxel_damage_radius = BOX_VOXEL_DAMAGE_RADIUS;
        this->attacker_properties.terrain_modification_action = t_map::TMA_MONSTER_BOX;
        this->target_acquisition_probability = BOX_TARGET_LOCK_CHANGE;


        this->voxel_properties.init_hitscan = MONSTER_INIT_HITSCAN;
        this->voxel_properties.init_draw = MONSTER_INIT_DRAW;
        this->voxel_properties.vox_dat = &box_vox_dat;

        this->spatial_properties.camera_height = BOX_CAMERA_HEIGHT;
        this->spatial_properties.height = BOX_HEIGHT;

        this->health_properties.health = BOX_HEALTH;

        // TODO -- make speed a base property
        // momentum should not be used this way (can be overwriiten, is only init etc)
        this->set_momentum(BOX_SPEED, BOX_SPEED, BOX_SPEED);

        this->rate_limit_state_interval = MONSTER_BROADCAST_INTERVAL;

        #if DC_CLIENT
        this->animation_size = BOX_ANIMATION_PARTICLE_SIZE;
        this->animation_count = BOX_ANIMATION_PARTICLE_COUNT;
        this->animation_color = BOX_ANIMATION_COLOR;
        #endif
    }
};

} // Monsters
