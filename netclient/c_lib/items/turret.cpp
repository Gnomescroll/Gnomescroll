#include "turret.hpp"

#include <math.h>

#include <c_lib/common/random.h>
#include <c_lib/agent/net_agent.hpp>
#include <net_lib/net.hpp>
#include <c_lib/items/packets.hpp>
#include <c_lib/t_map/t_map.hpp>

/* Packets */
class turret_create_StoC: public FixedSizeReliableNetPacketToClient<turret_create_StoC>
{
    public:
        uint8_t id;
        uint8_t owner;
        uint8_t team;
        float x,y,z;
        
        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&owner, buff, buff_n, pack);
            pack_u8(&team, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};

class turret_destroy_StoC: public FixedSizeReliableNetPacketToClient<turret_destroy_StoC>
{
    public:
        uint8_t id;

        inline void packet(char* buff, int* buff_n, bool pack)
        {
            pack_u8(&id, buff, buff_n, pack);
        }
        inline void handle();
};

class turret_state_StoC: public FixedSizeReliableNetPacketToClient<turret_state_StoC>
{
    public:
        uint8_t id;
        float x,y,z;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};


class turret_shot_nothing_StoC: public FixedSizeNetPacketToClient<turret_shot_nothing_StoC>
{
    public:
        uint8_t id;
        float vx,vy,vz;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_float(&vx, buff, buff_n, pack);
            pack_float(&vy, buff, buff_n, pack);
            pack_float(&vz, buff, buff_n, pack);
        }
        inline void handle();
};

class turret_shot_object_StoC: public FixedSizeNetPacketToClient<turret_shot_object_StoC>
{
    public:
        uint8_t id;
        uint8_t target_id;
        uint8_t target_type;
        uint8_t target_part;
        uint8_t vx,vy,vz;   //voxel

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&target_id, buff, buff_n, pack);
            pack_u8(&target_type, buff, buff_n, pack);
            pack_u8(&target_part, buff, buff_n, pack);
            pack_u8(&vx, buff, buff_n, pack);
            pack_u8(&vy, buff, buff_n, pack);
            pack_u8(&vz, buff, buff_n, pack);

        }
        inline void handle();
};

class turret_shot_block_StoC: public FixedSizeNetPacketToClient<turret_shot_block_StoC>
{
    public:
        uint8_t id;
        uint8_t cube;   // might not need this (infer from x,y,z)
        uint8_t side;
        float x,y,z;    // send the actual collision point

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_u8(&id, buff, buff_n, pack);
            pack_u8(&cube, buff, buff_n, pack);
            pack_u8(&side, buff, buff_n, pack);
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);
        }
        inline void handle();
};


#ifdef DC_CLIENT
inline void turret_state_StoC::handle()
{
    
    Turret* t = ClientState::turret_list->get(id);
    if (t==NULL) return;
    t->set_position(x,y,z);
}

inline void turret_create_StoC::handle()
{
    Turret* t = ClientState::turret_list->create(id, x,y,z);
    if (t==NULL)
    {
        printf("WARNING turret_create_StoC::handle() -- could not create turret %d\n", id);
        return;
    }
    t->set_team(team);
    t->set_owner(owner);
    t->init_vox();
    //Sound::turret_placed(x,y,z,0,0,0);
    //system_message->turret_created(t);
}

inline void turret_destroy_StoC::handle()
{
    //turret* t = ClientState::turret_list->get(id);
    //system_message->turret_destroyed(t);
    ClientState::turret_list->destroy(id);
}

inline void turret_shot_object_StoC::handle()
{
    if (target_type != OBJ_TYPE_AGENT) return; // remove this once turret can attack other objects

    Turret* t = ClientState::turret_list->get(id);
    if (t == NULL) return;
    Agent_state* a = ClientState::agent_list->get(target_id);
    if (a == NULL || a->vox == NULL) return;
    a->vox->update(a->s.x, a->s.y, a->s.z, a->s.theta, a->s.phi);
    Voxel_volume* vv = a->vox->get_part(target_part);
    if (vv == NULL) return;

    Vec3 c = vv->get_center();
    Vec3 p = vec3_init(t->x, t->y, t->z + t->camera_height);

    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_sub(c, p);
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    ClientState::hitscan_effect_list->create(
        p.x, p.y, p.z,
        v.x, v.y, v.z
    );
    int voxel[3] = { vx,vy,vz };
    destroy_object_voxel(target_id, target_type, target_part, voxel, 2);
    Sound::turret_shoot(t->x, t->y, t->z + t->camera_height, 0,0,0);
}

inline void turret_shot_nothing_StoC::handle()
{
    Turret *t = ClientState::turret_list->get(id);
    if (t == NULL) return;

    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_init(vx,vy,vz);
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    ClientState::hitscan_effect_list->create(
        t->x, t->y, t->z + t->camera_height,
        v.x, v.y, v.z
    );
    Sound::turret_shoot(t->x, t->y, t->z + t->camera_height, 0,0,0);
}

inline void turret_shot_block_StoC::handle()
{
    Turret *t = ClientState::turret_list->get(id);
    if (t == NULL) return;

    const float hitscan_effect_speed = 200.0f;
    Vec3 p = vec3_init(t->x, t->y, t->z + t->camera_height);
    Vec3 c = vec3_init(x,y,z);
    Vec3 v = vec3_sub(c, p); 
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    ClientState::hitscan_effect_list->create(
        p.x, p.y, p.z,
        v.x, v.y, v.z
    );

    Animations::block_damage(x,y,z, p.x, p.y, p.z, cube, side);
    Animations::terrain_sparks(x,y,z);
    Sound::laser_hit_block(x,y,z, 0,0,0);
    Sound::turret_shoot(t->x, t->y, t->z + t->camera_height, 0,0,0);
}
#endif

#ifdef DC_SERVER
inline void turret_state_StoC::handle(){}
inline void turret_create_StoC::handle(){}
inline void turret_destroy_StoC::handle(){}
inline void turret_shot_object_StoC::handle(){}
inline void turret_shot_nothing_StoC::handle(){}
inline void turret_shot_block_StoC::handle(){}
#endif


/* Turrets */

VoxDat turret_vox_dat;

void Turret::set_position(float x, float y, float z)
{
    if (this->x == x && this->y == y && this->z == z) return;

    this->x = x;
    this->y = y;
    this->z = z;
    
    this->vox->thaw();
    this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
    this->vox->freeze();

    #ifdef DC_SERVER
    turret_state_StoC msg;
    msg.x = this->x;
    msg.y = this->y;
    msg.z = this->z;
    msg.id = this->id;
    msg.broadcast();
    #endif
}

int Turret::get_team()
{
    return this->team;
}

void Turret::set_team(int team)
{
    this->team = team;
    if (this->vox != NULL) this->vox->update_team_color(this->team);
}

void Turret::set_owner(int owner)
{
    switch_agent_ownership(this->type, this->owner, owner);
    this->owner = owner;
}

int Turret::get_owner()
{
    return this->owner;
}

void Turret::init_vox()
{
    if (this->team == 0) printf("WARNING Turret::init_vox() -- team not set\n");
    this->vox = new Voxel_model(&turret_vox_dat, this->id, this->type, this->team);
    this->vox->set_hitscan(true);
    this->vox->register_hitscan();
    #ifdef DC_CLIENT
    this->vox->set_draw(true);
    #endif
    this->vox->thaw();
    this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
    this->vox->freeze();
}

void Turret::update()
{
    if (this->vox == NULL) return;
    
    #ifdef DC_CLIENT
    this->vox->was_updated = false;
    if (current_camera == NULL || !current_camera->in_view(x,y,z))
    {
        this->vox->set_draw(false);
        this->vox->set_hitscan(false);
    }
    else
    {
        this->vox->set_draw(true);
        this->vox->set_hitscan(true);
    }
    if (input_state.skeleton_editor)
    {
        this->vox->thaw();
        this->vox->update(this->x, this->y, this->z, this->theta, this->phi);
        this->vox->freeze();
    }
    #endif

    #if DC_SERVER
    this->vox->was_updated = false;
    this->vox->set_hitscan(true);
    #endif
}

Turret::Turret(int id)
:
fire_limiter(0),
team(0),
owner(0),
id(id),
health(TURRET_HEALTH),
type(OBJ_TYPE_TURRET),
x(0), y(0), z(0),
theta(0), phi(0),
camera_height(TURRET_CAMERA_HEIGHT),
vox(NULL)
{
}

Turret::Turret(int id, float x, float y, float z)
:
fire_limiter(0),
team(0),
owner(0),
id(id),
health(TURRET_HEALTH),
type(OBJ_TYPE_TURRET),
x(x), y(y), z(z),
theta(0), phi(0),
camera_height(TURRET_CAMERA_HEIGHT),
vox(NULL)
{
}

Turret::~Turret()
{
    #ifdef DC_SERVER
    turret_destroy_StoC msg;
    msg.id = this->id;
    msg.broadcast();
    #endif

    #if DC_CLIENT
    Animations::team_item_explode(this->vox->get_part(0)->get_center(), this->team);
    #endif

    if (this->owner != NO_AGENT)
    {
        Agent_state* a = STATE::agent_list->get(owner);
        if (a != NULL)
            a->status.lose_item(this->type);
    }
    if (this->vox != NULL) delete this->vox;
}

#ifdef DC_SERVER
void Turret::create_message(turret_create_StoC* msg)
{
    msg->id = this->id;
    msg->team = this->team;
    msg->owner = this->owner;
    msg->x = this->x;
    msg->y = this->y;
    msg->z = this->z;
}
#endif

int Turret::get_coins_for_kill(int owner, int team)
{
    if ((this->team != team && this->owner != NO_AGENT) || owner == this->owner) // enemy team, or owner, can destroy/reclaim turret
        return get_object_cost(this->type);
    return 0;
}

int Turret::take_damage(int dmg)
{
    if (this->health <= 0) return 0;
    this->health -= dmg;
    this->health = (this->health < 0) ? 0 : this->health;
    #if DC_SERVER
    if (this->health <= 0)
    {
        ServerState::damage_objects_within_sphere(
            this->x, this->y, this->z,
            TURRET_EXPLOSION_RADIUS, TURRET_EXPLOSION_DAMAGE, this->owner,
            this->type, this->id,
            false
        );
    }
    #endif
    return this->health;
}

void Turret::acquire_target()
{
#if DC_SERVER
    using STATE::agent_list;
    // find enemies in range
    agent_list->enemies_within_sphere(x,y,z, TURRET_SIGHT_RANGE, team);
    //agent_list->objects_within_sphere(x,y,z, TURRET_SIGHT_RANGE);
    if (!agent_list->n_filtered) return;

    Vec3 source = vec3_init(x,y, z + camera_height);
    Vec3 sink;
    int chosen[agent_list->n_filtered];
    for (int i=0; i<agent_list->n_filtered; i++)
        chosen[i] = i;
    shuffle_int_array(chosen, agent_list->n_filtered);  // randomize
    
    Agent_state* agent = NULL;
    for (int i=0; i<agent_list->n_filtered; i++)
    {   // ray cast to agent
        agent = agent_list->filtered_objects[chosen[i]];
        if (agent->in_sight_of(source, &sink, 1-TURRET_TARGET_LOCK_CHANCE))
            break;
        agent = NULL;
    }
    if (agent == NULL) return;

    // get vector to sink
    Vec3 v = vec3_sub(sink, source);
    normalize_vector(&v);
    //add random bias
    const float bias = 0.3f;
    float arc = bias / 360.0f;
    float theta = randf() * kPI * 2 - kPI;
    float phi = randf() * kPI * 2 - kPI;
    float rho = randf() * kPI * 2 - kPI;
    v = vec3_euler_rotation(v, theta*arc, phi*arc, rho*arc);

    // hitscan biased laser against world
    struct Voxel_hitscan_target target;
    float vox_distance;
    float collision_point[3];
    int block_pos[3];
    int side[3];
    int tile;
    float block_distance;
    Hitscan::HitscanTargetTypes
    target_type = Hitscan::hitscan_against_world(
        source, v,
        this->id, OBJ_TYPE_TURRET,
        &target, &vox_distance, collision_point,
        block_pos, side, &tile, &block_distance
    );

    // create/send packets; apply damage
    turret_shot_object_StoC object_msg;
    turret_shot_block_StoC block_msg;
    Vec3 collision_pt;
    switch (target_type)
    {
        case Hitscan::HITSCAN_TARGET_VOXEL:
            if (vox_distance > TURRET_SIGHT_RANGE)
            {
                target_type = Hitscan::HITSCAN_TARGET_NONE;
                break;
            }
            // fire packet
            object_msg.id = this->id;
            object_msg.target_id = target.entity_id;
            object_msg.target_type = target.entity_type;
            object_msg.target_part = target.part_id;
            object_msg.vx = target.voxel[0];
            object_msg.vy = target.voxel[1];
            object_msg.vz = target.voxel[2];
            object_msg.broadcast();
            if (agent->status.lifetime > AGENT_TURRET_PROTECTION_DURATION && !agent->near_base())
            {
                agent->status.apply_damage(TURRET_AGENT_DAMAGE, this->id, OBJ_TYPE_TURRET, target.part_id);     // damage agent
                destroy_object_voxel(target.entity_id, target.entity_type, target.part_id, target.voxel, 2);    // damage model
            }
            break;
            
        case Hitscan::HITSCAN_TARGET_BLOCK:
            if (block_distance > TURRET_SIGHT_RANGE)
            {
                target_type = Hitscan::HITSCAN_TARGET_NONE;
                break;
            }
            collision_pt = vec3_add(source, vec3_scalar_mult(v, block_distance));
            // damage
            t_map::apply_damage_broadcast(collision_pt.x, collision_pt.y, collision_pt.z, TURRET_BLOCK_DAMAGE, t_map::TMA_LASER);
            // fire packet
            block_msg.x = collision_pt.x;
            block_msg.y = collision_pt.y;
            block_msg.z = collision_pt.z;
            block_msg.id = this->id;
            block_msg.cube = tile;
            block_msg.side = get_cube_side_from_side_array(side);
            block_msg.broadcast();
            break;

        default:
            target_type = Hitscan::HITSCAN_TARGET_NONE;
            break;
    }

    if (target_type == Hitscan::HITSCAN_TARGET_NONE)
    {
        turret_shot_nothing_StoC msg;
        msg.id = this->id;
        msg.vx = v.x;
        msg.vy = v.y;
        msg.vz = v.z;
        msg.broadcast();
    }
#endif
}

/*
 *
 *
 * TARGET LOCK HITSCAN ABSTRACTION
 *
 *
 *
 * */

struct HitscanTarget
{
    Hitscan::HitscanTargetTypes hitscan;
    int voxel[3];
    Vec3 collision_point;

    // voxel
    int id;
    Object_types type;
    int part;

    // terrain
    int cube;
    int side;
};

// replace this with full state dat
struct AttackerProperties
{
    int id;
    Object_types type;
    int block_damage;
    int agent_damage;
    int voxel_damage_radius;
    int agent_protection_duration;
    t_map::TerrainModificationAction terrain_modification_action;
};


Agent_state* lock_agent_target(
    Vec3 firing_position, Vec3 &firing_direction, int team,
    const float range, const float acquisition_probability,
    const bool enemies=false, const bool random=false
)
{
    // find agents in range
    using STATE::agent_list;
    if (enemies)
    {
        agent_list->enemies_within_sphere(
            firing_position.x, firing_position.y, firing_position.z, range, team
        );
    }
    else
    {
        agent_list->objects_within_sphere(
            firing_position.x, firing_position.y, firing_position.z, range
        );
    }
    if (!agent_list->n_filtered) return NULL;

    int chosen[agent_list->n_filtered];
    if (random)
    {
        for (int i=0; i<agent_list->n_filtered; i++)
            chosen[i] = i;
        shuffle_int_array(chosen, agent_list->n_filtered);  // randomize
    }
    
    Agent_state* agent = NULL;
    Vec3 sink;
    for (int i=0; i<agent_list->n_filtered; i++)
    {   // ray cast to agent
        if (random)
            agent = agent_list->filtered_objects[chosen[i]];
        else
            agent = agent_list->filtered_objects[i];
        if (agent->in_sight_of(firing_position, &sink, acquisition_probability))
        {
            firing_direction = vec3_sub(sink, firing_position);
            break;
        }
        agent = NULL;
    }
    return agent;
}

HitscanTarget shoot_at_enemy_agent(
    Vec3 source, Vec3 firing_direction, int id, Object_types type,
    Agent_state* agent, const float range
)
{
    // hitscan vector against world
    struct Voxel_hitscan_target target;
    float vox_distance;
    float collision_point[3];
    int block_pos[3];
    int side[3];
    int tile;
    float block_distance;
    Hitscan::HitscanTargetTypes
    target_type = Hitscan::hitscan_against_world(
        source, firing_direction,
        id, type,
        &target, &vox_distance, collision_point,
        block_pos, side, &tile, &block_distance
    );

    // process target information
    HitscanTarget target_information;
    switch (target_type)
    {
        case Hitscan::HITSCAN_TARGET_VOXEL:
            if (vox_distance > range)
            {
                target_type = Hitscan::HITSCAN_TARGET_NONE;
                break;
            }
            target_information.id = target.entity_id;
            target_information.type = (Object_types)target.entity_type;
            target_information.part = target.part_id;
            for (int i=0; i<3; i++)
                target_information.voxel[i] = target.voxel[i];
            break;
            
        case Hitscan::HITSCAN_TARGET_BLOCK:
            if (block_distance > range)
            {
                target_type = Hitscan::HITSCAN_TARGET_NONE;
                break;
            }
            target_information.collision_point = vec3_add(source,
                        vec3_scalar_mult(firing_direction, block_distance));
            target_information.cube = tile;
            target_information.side = get_cube_side_from_side_array(side);
            break;

        default:
            target_type = Hitscan::HITSCAN_TARGET_NONE;
            break;
    }

    if (target_type == Hitscan::HITSCAN_TARGET_NONE)
        target_information.collision_point = vec3_copy(firing_direction);

    target_information.hitscan = target_type;
    return target_information;
}

void handle_hitscan_target(HitscanTarget t, struct AttackerProperties p)
{
    #if DC_SERVER
    Agent_state* agent;
    switch (t.hitscan)
    {
        case Hitscan::HITSCAN_TARGET_BLOCK:
            t_map::apply_damage_broadcast(
                t.collision_point.x, t.collision_point.y, t.collision_point.z,
                p.block_damage, p.terrain_modification_action
            );
            break;

        case Hitscan::HITSCAN_TARGET_VOXEL:
            if (t.type == OBJ_TYPE_AGENT)
            {
                agent = STATE::agent_list->get(t.id);
                if (agent == NULL) break;
                if (agent->status.lifetime > p.agent_protection_duration
                  && !agent->near_base())
                {
                    agent->status.apply_damage(
                        p.agent_damage, p.id, p.type, t.part
                    );
                    destroy_object_voxel(
                        t.id, t.type, t.part, t.voxel,
                        p.voxel_damage_radius
                    );
                }
            }
            break;

        case Hitscan::HITSCAN_TARGET_NONE:
            break;
            
        default: break;
    }
    #endif
}

void broadcast_object_fired(int id, Object_types type, HitscanTarget t)
{
    object_shot_object_StoC obj_msg;
    object_shot_terrain_StoC terrain_msg;
    object_shot_nothing_StoC none_msg;
    switch (t.type)
    {
        case Hitscan::HITSCAN_TARGET_VOXEL:
            obj_msg.id = id;
            obj_msg.type = type;
            obj_msg.target_id = t.id;
            obj_msg.target_type = t.type;
            obj_msg.target_part = t.part;
            obj_msg.voxel_x = t.voxel[0];
            obj_msg.voxel_y = t.voxel[1];
            obj_msg.voxel_z = t.voxel[2];
            obj_msg.broadcast();
            break;
            
        case Hitscan::HITSCAN_TARGET_BLOCK:
            terrain_msg.id = id;
            terrain_msg.type = type;
            terrain_msg.x = t.collision_point.x;
            terrain_msg.y = t.collision_point.y;
            terrain_msg.z = t.collision_point.z;
            terrain_msg.cube = t.cube;
            terrain_msg.side = t.side;
            terrain_msg.broadcast();
            break;
            
        case Hitscan::HITSCAN_TARGET_NONE:
            none_msg.id = id;
            none_msg.x = t.collision_point.x;
            none_msg.y = t.collision_point.y;
            none_msg.z = t.collision_point.z;
            none_msg.broadcast();
            break;
            
        default: break;
    }
}

/* example */
void do_acquire_target()
{
    // firing properties (will be from dat/state)
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    int id = 1;
    int team = 1;
    Object_types type = OBJ_TYPE_AGENT;
    const float camera_height = 0.5f;
    const float range = 128.0f;
    const float bias = 1.5f;
    const float acquisition_probability = 1.0f;
    const bool enemies = true;
    const bool random = true;
    
    // lock on agent
    Vec3 firing_position = vec3_init(x, y, z + camera_height);
    Vec3 firing_direction;
    Agent_state* agent = lock_agent_target(
        firing_position, firing_direction, team,
        range, acquisition_probability, enemies, random
    );
    if (agent == NULL) return;

    // normalize and bias vector
    normalize_vector(&firing_direction);
    if (bias)   // apply bias
        firing_direction = vec3_bias_random(firing_direction, bias);

    // get target
    HitscanTarget t = shoot_at_enemy_agent(
        firing_position, firing_direction, id, type,
        agent, range
    );

    // attacker properties (will be from dat)
    struct AttackerProperties attacker_properties;
    attacker_properties.agent_protection_duration = 0;
    attacker_properties.agent_damage = 25;
    attacker_properties.block_damage = 16;
    attacker_properties.voxel_damage_radius = 2;
    attacker_properties.terrain_modification_action = t_map::TMA_LASER;

    // let handle target hit based on attacker properties
    handle_hitscan_target(t, attacker_properties);

    // send firing packet
    broadcast_object_fired(id, type, t);

    // apply custom handling
    // play sounds
    // play animations
}

/*
 *
 *
 * END TARGET LOCK HITSCAN ABSTRACTION
 *
 *
 *
 * */

void Turret::tick()
{    
#ifdef DC_SERVER
    if (this->health <= 0)
    {
        STATE::turret_list->destroy(this->id);
        return;
    }

    // fall/climb with terrain
    int x,y,z;
    x = (int)this->x;
    y = (int)this->y;
    z = (int)this->z;
    if (isSolid(_get(x,y,z)))
    {   // move up
        while (isSolid(_get(x,y,++z)))
        {
            if (z >= map_dim.z)
            {
                z = map_dim.z;
                break;
            }
        }
    }
    else
    {   // fall down
        while (!isSolid(_get(x,y,--z)))
        {
            if (z<=0)
            {
                z = 0;
                break;
            }
        }
        z++;
    }
    this->set_position(this->x, this->y, (float)z);

    if (fire_limiter % TURRET_FIRE_LIMIT == 0)
        this->acquire_target();
    fire_limiter++;
#endif
}

/* Turret list */

bool Turret_list::point_occupied(int x, int y, int z)
{
    for (int i=0; i<n_max; i++)
    {
        Turret *s = this->a[i];
        if (s == NULL) continue;
        if ((int)s->x == x && (int)s->y == y)
        {
            // turret is 2 blocks tall
            for (int j=0; j<(int)ceil(TURRET_HEIGHT); j++)
            {
                if ((int)s->z+j == z) return true;
            }
        }
    }
    return false;
}

#ifdef DC_SERVER
void Turret_list::send_to_client(int client_id)
{
    for (int i=0; i<this->n_max; i++)
    {
        Turret *s = this->a[i];
        if (s == NULL) continue;

        turret_create_StoC msg;
        s->create_message(&msg);
        msg.sendToClient(client_id);
    }
}
#endif

void Turret_list::tick()
{
    for (int i=0; i<n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->tick();
}

void Turret_list::update()
{
    for (int i=0; i<n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->update();
}

void Turret_list::alter_owner(int owner, int new_owner)
{
    #if DC_SERVER
    if (owner == new_owner)
    {
        printf("WARNING -- Turret_list::alter_owner -- owner == new_owner %d\n", owner);
        return;
    }
    for (int i=0; i<this->n_max; i++)
    {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->get_owner() != owner) continue;
        this->a[i]->set_owner(new_owner);
        alter_item_ownership_StoC msg;
        msg.owner = new_owner;
        msg.id = this->a[i]->id;
        msg.type = this->a[i]->type;
        msg.broadcast();
    }
    #endif
}
