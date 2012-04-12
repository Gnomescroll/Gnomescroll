#pragma once

#include <math.h>

#include <c_lib/common/enum_types.hpp>
#include <c_lib/common/common.hpp>
#include <c_lib/monsters/constants.hpp>
#include <c_lib/objects/components/target_acquisition/component.hpp>
#include <c_lib/objects/components/voxel/component.hpp>
#include <c_lib/objects/components/tick/components.hpp>
#include <c_lib/objects/common/api/born.hpp>
#include <c_lib/objects/common/api/update.hpp>
#include <c_lib/objects/common/api/tick.hpp>
#include <c_lib/objects/common/net/packets.hpp>
#include <c_lib/agent/agent.hpp>
#include <c_lib/monsters/slime.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/t_map/t_map.hpp>
#include <c_lib/objects/common/net/interfaces.hpp>

#if DC_SERVER
//forward decl
namespace ServerState
{
    extern GameObject_list* object_list;
}
#endif

/* TODO MOVE */
class object_choose_target_StoC: public FixedSizeReliableNetPacketToClient<object_choose_target_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t target_id;
        uint8_t target_type;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&target_id, buff, buff_n, pack);
        pack_u8(&target_type, buff, buff_n, pack);
    }
    inline void handle();
};

class object_choose_destination_StoC: public FixedSizeReliableNetPacketToClient<object_choose_destination_StoC>
{
    public:
        uint16_t id;
        uint8_t type;
        uint16_t ticks;
        float x,y,z;

    inline void packet(char* buff, int* buff_n, bool pack)
    {
        pack_u16(&id, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
        pack_u16(&ticks, buff, buff_n, pack);
        pack_float(&x, buff, buff_n, pack);
        pack_float(&y, buff, buff_n, pack);
        pack_float(&z, buff, buff_n, pack);
    }
    inline void handle();
};
/* END TODO MOVE */

namespace Monsters {

extern VoxDat box_vox_dat;

void box_shot_object(object_shot_object_StoC* msg);

void boxDropItem(Vec3 position)
{   // TODO -- some dat format for thiss
    #if DC_SERVER
    //const float drop_probability = 0.33f;
    const float drop_probability = 1.00f;
    float p = randf();
    if (p > drop_probability) return;
    
    const int n_types = 1;    
    Object_types types[n_types] = {
        OBJ_TYPE_LASER_REFILL,
    };
    Object_types type = types[randrange(0,n_types-1)];
    const float mom = 5.0f;
    ObjectPolicyInterface* obj = ServerState::object_list->create(type);
    if (obj != NULL)
    {
        obj->set_position(position.x, position.y, position.z+1.0f);
        obj->set_momentum((randf()-0.5f)*mom, (randf()-0.5f)*mom, mom);
        obj->born();
    }

    #endif
}

bool moveBox(Vec3 position, Vec3 direction, float speed, Vec3* new_position, Vec3* new_momentum)
{
    // attempt to move to location defined by direction * speed
    // if z_level diff too high, set momentum 0, copy position, return
    // else calculate new destination, reorient direction, multiply by speed, and set to new_momentum
    // add new_momentum to position to get new_position

    // assumes direction is normalized
    const int max_z_level_diff_up = 4; // maximum height to climb up
    const int max_z_level_diff_down = -8; // maximum height to climb down

    //printf("direction %0.2f,%0.2f,%0.2f\n", direction.x, direction.y, direction.z);
    Vec3 move_to = vec3_add(position, vec3_scalar_mult(direction, speed));
    int z = t_map::get_highest_open_block(move_to.x, move_to.y);

    float z_diff = position.z - z;
    if (z_diff > max_z_level_diff_up || z_diff < max_z_level_diff_down)
    {   // cant move
    //printf("cant move\n");
        *new_position = position;
        *new_momentum = vec3_init(0,0,0);
        return false;
    }

    move_to.z = z;
    Vec3 new_direction = vec3_sub(move_to, position);
    normalize_vector(&new_direction);
    *new_momentum = vec3_scalar_mult(new_direction, speed);
    *new_position = vec3_add(position, *new_momentum);
    return true;
}

class Box:
    public VoxelComponent,
    public TargetAcquisitionComponent,
    public RateLimitedStateBroadcastComponent,
    #if DC_CLIENT
    public AnimationVoxelComponent,
    #endif
    public ObjectStateLayer
{
    public:

        HealthComponent health;
        PositionMomentumChangedComponent spatial;

        Vec3 destination;
        bool at_destination;
        bool en_route;
        int ticks_to_destination;
        float speed;

        int target_id;
        Object_types target_type;
        bool locked_on_target;

        Vec3 direction;

    #if DC_SERVER
    void server_tick()
    {
        // must stay on ground -- apply terrain collision
        // wander randomly (TODO: network model with destinations)
        // TODO -- aggro component

        // save current target state, will use this to decide if need to send packet
        bool was_on_target = this->locked_on_target;
        int old_target_id = this->target_id;
        int old_target_type = this->target_type;
        
        Agent_state* agent = NULL;
        if (this->locked_on_target)
        {   // target locked
            // check target still exists
            if (this->target_type == OBJ_TYPE_AGENT)
                agent = STATE::agent_list->get(this->target_id);
            if (agent == NULL
            || vec3_distance_squared(agent->get_center(), this->get_center(BOX_PART_BODY)) > this->speed*this->speed)
                this->locked_on_target = false;
        }

        if (!this->locked_on_target)
        {   // no target found
            // look for target
            ObjectState* state = this->state();
            Vec3 direction;
            agent = this->acquire_target(
                state->id, state->type, this->get_team(), this->spatial.camera_z(),
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
            if (!was_on_target || old_target_id != this->target_id || old_target_type != this->target_type)
            {
                object_choose_target_StoC msg;
                msg.id = this->_state.id;
                msg.type = this->_state.type;
                msg.target_id = this->target_id;
                msg.target_type = this->target_type;
                msg.broadcast();
            }
        }
        
        if (this->locked_on_target)
        {   // target is locked
            // face target
            Vec3 direction = vec3_sub(agent->get_center(), this->get_center(BOX_PART_BODY));
            if (vec3_length(direction))
            {
                normalize_vector(&direction);
                float theta,phi;
                vec3_to_angles(direction, &theta, &phi);
                this->set_angles(theta, phi, 0);
                this->direction = direction;
            }

            if (this->can_fire())
            {
                ObjectState* state = this->state();
                this->fire_on_known_target(
                    state->id, state->type, this->spatial.camera_z(), this->get_position(),
                    this->direction, state->accuracy_bias, state->sight_range, agent
                );
            }
        }
        else if (this->en_route)
        {   // face destination
            float theta, phi;
            vec3_to_angles(this->direction, &theta, &phi);
            this->set_angles(theta, phi, 0);
        }

        if (!this->en_route && !this->locked_on_target)
        {   // no destination, no target
            // choose destination
            const int walk_len = BOX_WALK_RANGE;
            int dx = randrange(0,walk_len) - walk_len/2;
            int dy = randrange(0,walk_len) - walk_len/2;
            this->destination = vec3_add(this->get_position(), vec3_init(((float)dx)+randf(), ((float)dy)+randf(),0));
            // clamp
            if (this->destination.x < 0) this->destination.x = 0;
            if (this->destination.x >= map_dim.x) this->destination.x = map_dim.x -1;
            if (this->destination.y < 0) this->destination.y = 0;
            if (this->destination.y >= map_dim.y) this->destination.y = map_dim.y -1;
            this->destination.z = (float)t_map::get_highest_open_block(this->destination.x,this->destination.y);
            if (this->destination.z < 0) this->destination.z = 0;

            this->en_route = true;
            this->at_destination = false;
            
            Vec3 direction = vec3_sub(this->destination, this->get_position());
            float len = vec3_length(direction);
            this->ticks_to_destination = (int)ceil(len/this->speed);
            if (len)
            {
                direction.z = 0;
                normalize_vector(&direction);
                this->direction = direction;
            }

            // send destination packet
            // TODO
            object_choose_destination_StoC msg;
            msg.x = destination.x;
            msg.y = destination.y;
            msg.z = destination.z;
            msg.id = this->_state.id;
            msg.type = this->_state.type;
            msg.ticks = this->ticks_to_destination;
            msg.broadcast();
            // TODO
        }

        if (!this->at_destination)
        {   // check if at destination
            float dist = vec3_distance_squared(this->destination, this->get_position());
            if (dist < 1.0f)    // TODO Margin
            {
                this->en_route = false;
                this->at_destination = true;
                this->set_momentum(0,0,0);
            }
        }

        if (this->en_route)
        {   // move towards destination
            Vec3 position;
            Vec3 momentum;
            this->en_route = moveBox(this->get_position(), this->direction, this->speed, &position, &momentum);
            this->set_position(position.x, position.y, position.z);
            this->set_momentum(momentum.x, momentum.y, momentum.z);
        }

        //if (this->spatial.properties.changed)
            //this->broadcastState(); // send state packet if state changed
        //else if (this->canSendState())
            //this->broadcastState(); // send state packet every N ticks
    }
    #endif

    #if DC_CLIENT
    void client_tick()
    {
        //return;
        if (this->locked_on_target)
        {   // target locked
            if (this->target_type != OBJ_TYPE_AGENT) return;    // TODO -- more objects
            Agent_state* agent = ClientState::agent_list->get(this->target_id);
            if (agent == NULL) return;
        
            // face target
            Vec3 agent_position = agent->get_center();
            Vec3 position = this->get_center(BOX_PART_BODY);
            Vec3 direction = vec3_sub(agent_position, position);
            if (vec3_length(direction))
            {
                float theta,phi;
                normalize_vector(&direction);
                this->direction = direction;
                vec3_to_angles(direction, &theta, &phi);
                this->set_angles(theta, phi, 0);
            }
            return; // do nothing else
        }

        if (!this->at_destination)
        {   // check if at destination
            float dist = vec3_distance_squared(this->destination, this->get_position());
            if (dist < 1.0f)    // TODO Margin
            {   // at destination, stop
                this->en_route = false;
                this->at_destination = true;
                this->set_momentum(0,0,0);
            }
        }

        if (this->en_route)
        {   // move towards destination
            Vec3 position;
            Vec3 momentum;
            this->en_route = moveBox(this->get_position(), this->direction, this->speed, &position, &momentum);
            this->set_position(position.x, position.y, position.z);
            this->set_momentum(momentum.x, momentum.y, momentum.z);

            if (vec3_length(momentum))
            {
                momentum.z = 0;
                normalize_vector(&momentum);
                this->direction = momentum;
            }

            // face in direction of movement
            float theta, phi;
            vec3_to_angles(this->direction, &theta, &phi);
            this->set_angles(theta, phi, 0);
        }

    }
    #endif

    void tick()
    {
        #if DC_SERVER
        this->server_tick();
        #endif
        #if DC_CLIENT
        this->client_tick();
        #endif
    }

    void die()
    {
        #if DC_CLIENT
        Vec3 position = this->get_center(BOX_PART_BODY);
        this->animation_voxel_explode(position);
        #endif

        #if DC_SERVER
        boxDropItem(this->get_position());
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
            this->spatial.properties.angles.x, this->spatial.properties.angles.y); 
    }

    void update()
    {
        updateVox(
            this->voxel_properties.vox, this->get_position(),
            this->spatial.properties.angles,
            this->spatial.properties.changed
        );
        this->spatial.properties.set_changed(false);
    }
    
    void draw() {/*Empty*/}

    explicit Box(int id)
    : ObjectStateLayer(Objects::create_packet_momentum_angles, Objects::state_packet_momentum_angles, Objects::owned_none, Objects::team_none, &health, &spatial),
    at_destination(false), en_route(false), ticks_to_destination(1), speed(BOX_SPEED),
    target_id(NO_AGENT), target_type(OBJ_TYPE_NONE),
    locked_on_target(false)
    {
        this->_state.id = id;

        this->_state.coin_rule = COINS_ANYONE;

        this->_state.motion_proximity_radius = BOX_MOTION_PROXIMITY_RADIUS;

        // todo -- firing rate / properties
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
        this->fire_rate_limit = BOX_FIRE_RATE;
        this->set_random_fire_tick();

        this->voxel_properties.init_hitscan = MONSTER_INIT_HITSCAN;
        this->voxel_properties.init_draw = MONSTER_INIT_DRAW;
        this->voxel_properties.vox_dat = &box_vox_dat;

        this->spatial.properties.camera_height = BOX_CAMERA_HEIGHT;
        this->spatial.properties.height = BOX_HEIGHT;

        this->health.properties.health = BOX_HEALTH;

        this->rate_limit_state_interval = MONSTER_BROADCAST_INTERVAL;

        #if DC_CLIENT
        this->animation_size = BOX_ANIMATION_PARTICLE_SIZE;
        this->animation_count = BOX_ANIMATION_PARTICLE_COUNT;
        this->animation_color = BOX_ANIMATION_COLOR;
        #endif

        this->direction = vec3_init(1,0,0);
    }
};

} // Monsters
