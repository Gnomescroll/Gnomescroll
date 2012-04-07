#include "player_agent_action.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/animations/hitscan.hpp>
#include <common/enum_types.hpp>
#include <c_lib/agent/net_agent.hpp>
#include <sound/triggers.hpp>

//stuff

//#include <c_lib/t_map/net/t_CtoS.hpp>

#define BLOCK_PICK_MAX_DISTANCE 4.0f
#define MELEE_PICK_MAX_DISTANCE 2.7f

void PlayerAgent_action::fire()
{
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    int type = p->you->weapons.active_type();
    if (!p->you->weapons.fire())
    {
        Sound::out_of_ammo();   // move this to individual weapon fire methods later
        return;
    }
    switch (type)
    {
        case Weapons::TYPE_block_applier:
            set_block();
            break;
        case Weapons::TYPE_hitscan_laser:
            hitscan_laser();
            break;
        case Weapons::TYPE_block_pick:
            hitscan_pick();
            break;
        case Weapons::TYPE_grenade_thrower:
            throw_grenade();
            break;
        case Weapons::TYPE_spawner_placer:
            place_spawner();
            break;
        case Weapons::TYPE_turret_placer:
            place_turret();
            break;
        default:
            printf("PlayerAgent_action::fire -- No action defined for weapon type %d\n", type);
            break;
    }
}

void PlayerAgent_action::hitscan_laser()
{
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    Vec3 pos = vec3_init(p->camera_state.x, p->camera_state.y, p->camera_z());
    Vec3 look = agent_camera->forward_vector();

    struct Voxel_hitscan_target target;
    float vox_distance;
    float collision_point[3];
    int block_pos[3];
    int side[3];
    int tile;
    float block_distance;

    Hitscan::HitscanTargetTypes target_type =
        Hitscan::hitscan_against_world(
            pos, look, this->p->agent_id, OBJ_TYPE_AGENT,
            &target, &vox_distance, collision_point,
            block_pos, side, &tile, &block_distance
        );

    // for hitscan animation:
    // readjust the vector so that the translated position points to target
    // get the right vector for translating the hitscan laser anim
    normalize_vector(&look);
    Vec3 up = vec3_init(0,0,1);
    Vec3 right = vec3_cross(look, up);
    normalize_vector(&right);

    // magic offset numbers found by rotating the laser to the right spot
    // fixed in the bottom right corner
    const float dxy = 0.14;
    const float dz = -0.13;

    // animation origin
    float origin[3];
    origin[0] = pos.x + dxy * right.x;
    origin[1] = pos.y + dxy * right.y;
    origin[2] = pos.z + dz;

    // send packet
    hitscan_block_CtoS block_msg;
    hitscan_none_CtoS none_msg;
    hitscan_object_CtoS obj_msg;

    Agent_state* agent;
    int voxel_blast_radius = 1;
    
    switch (target_type)
    {
        case Hitscan::HITSCAN_TARGET_VOXEL:
            obj_msg.id = target.entity_id;
            obj_msg.type = target.entity_type;
            obj_msg.part = target.part_id;
            obj_msg.vx = target.voxel[0];
            obj_msg.vy = target.voxel[1];
            obj_msg.vz = target.voxel[2];
            obj_msg.send();

            // subtract the collision point from the origin to get the new vector for animation
            look.x = collision_point[0] - origin[0];
            look.y = collision_point[1] - origin[1];
            look.z = collision_point[2] - origin[2];
            normalize_vector(&look);

            if (target.entity_type == OBJ_TYPE_AGENT)
            {
                Animations::blood_spray(
                    collision_point[0], collision_point[1], collision_point[2],
                    look.x, look.y, look.z
                );
                Sound::laser_hit_agent(
                    collision_point[0], collision_point[1], collision_point[2],
                    0,0,0
                );
                agent = ClientState::agent_list->get(target.entity_id);
                if (agent==NULL) break;
                if (agent->status.team == this->p->you->status.team) break;
                voxel_blast_radius = 3;
            }
            else if (target.entity_type == OBJ_TYPE_SLIME)
            {
                voxel_blast_radius = 2;
            }
            destroy_object_voxel(target.entity_id, target.entity_type, target.part_id, target.voxel, voxel_blast_radius);
            break;

        case Hitscan::HITSCAN_TARGET_BLOCK:            
            block_msg.x = block_pos[0];
            block_msg.y = block_pos[1];
            block_msg.z = block_pos[2];
            block_msg.send();

            // multiply look vector by distance to collision
            look = vec3_scalar_mult(look, block_distance);
            // add agent position, now we have collision point
            look = vec3_add(look, pos);
            // copy this to collision_point, for block damage animation
            collision_point[0] = look.x;
            collision_point[1] = look.y;
            collision_point[2] = look.z;
            // subtract translated animation origin from collision point (vec) to get new vector
            look.x -= origin[0];
            look.y -= origin[1];
            look.z -= origin[2];
            normalize_vector(&look);

            Animations::block_damage(
                collision_point[0], collision_point[1], collision_point[2],
                look.x, look.y, look.z,
                tile, side
            );
            Animations::terrain_sparks(collision_point[0], collision_point[1], collision_point[2]);
            Sound::laser_hit_block(collision_point[0], collision_point[1], collision_point[2], 0,0,0);
            
            break;
            
        case Hitscan::HITSCAN_TARGET_NONE:
            // for no target, leave translated animation origin
            none_msg.send();    // server will know to forward a fire weapon packet
            break;
        default:
            break;
    }

    Sound::fire_laser();

    // play laser anim (client viewport)
    const float hitscan_speed = 200.0f;
    look = vec3_scalar_mult(look, hitscan_speed);
    ClientState::hitscan_effect_list->create(
        origin[0], origin[1], origin[2],
        look.x, look.y, look.z
    );
}

void PlayerAgent_action::hitscan_pick()
{
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    Vec3 pos = vec3_init(p->camera_state.x, p->camera_state.y, p->camera_z());
    Vec3 vec = agent_camera->forward_vector();

    struct Voxel_hitscan_target target;
    float vox_distance;
    float collision_point[3];
    int block_pos[3];
    int side[3];
    int tile;
    float block_distance;

    Hitscan::HitscanTargetTypes target_type =
        Hitscan::hitscan_against_world(
            pos, vec, this->p->agent_id, OBJ_TYPE_AGENT,
            &target, &vox_distance, collision_point,
            block_pos, side, &tile, &block_distance
        );

    // send packet
    hit_block_CtoS block_msg;
    melee_object_CtoS obj_msg;

    Agent_state* agent;
    int voxel_blast_radius = 1;

    switch (target_type)
    {
        case Hitscan::HITSCAN_TARGET_VOXEL:
            if (vox_distance > MELEE_PICK_MAX_DISTANCE)
            {
                target_type = Hitscan::HITSCAN_TARGET_NONE;
                break;
            }
        
            obj_msg.id = target.entity_id;
            obj_msg.type = target.entity_type;
            obj_msg.part = target.part_id;
            obj_msg.vx = target.voxel[0];
            obj_msg.vy = target.voxel[1];
            obj_msg.vz = target.voxel[2];
            obj_msg.send();

            if (target.entity_type == OBJ_TYPE_AGENT)
            {
                Animations::blood_spray(
                    collision_point[0], collision_point[1], collision_point[2],
                    vec.x, vec.y, vec.z
                );
                Sound::pick_hit_agent(
                    collision_point[0], collision_point[1], collision_point[2],
                    0,0,0
                );
                voxel_blast_radius = 3;
            }
            else if (target.entity_type == OBJ_TYPE_SLIME)
            {
                voxel_blast_radius = 2;
            }
            agent = ClientState::agent_list->get(target.entity_id);
            if (agent==NULL) break;
            if (agent->status.team == this->p->you->status.team) break;
            destroy_object_voxel(target.entity_id, target.entity_type, target.part_id, target.voxel, voxel_blast_radius);
            break;

        case Hitscan::HITSCAN_TARGET_BLOCK:
            if (block_distance > BLOCK_PICK_MAX_DISTANCE)
            {
                target_type = Hitscan::HITSCAN_TARGET_NONE;
                break;
            }
            block_msg.x = block_pos[0];
            block_msg.y = block_pos[1];
            block_msg.z = block_pos[2];
            block_msg.send();

            collision_point[0] = pos.x + vec.x * block_distance;
            collision_point[1] = pos.y + vec.y * block_distance;
            collision_point[2] = pos.z + vec.z * block_distance;

            Animations::block_damage(
                collision_point[0], collision_point[1], collision_point[2],
                vec.x, vec.y, vec.z,
                tile, side
            );
            Sound::pick_hit_block(collision_point[0], collision_point[1], collision_point[2], 0,0,0);
            
            break;
            
        default:
            break;
    }

    if (target_type == Hitscan::HITSCAN_TARGET_NONE)
    {
        melee_none_CtoS none_msg;
        none_msg.send();
    }

    //Sound::pick_swung();
}

void PlayerAgent_action::throw_grenade()
{
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    // message to server
    //float x = p->camera_state.x;
    //float y = p->camera_state.y;
    //float z = p->camera_z();
    float x = p->you->s.x;
    float y = p->you->s.y;
    float z = p->you->camera_z();
    ThrowGrenade_CtoS msg;
    msg.x = x;
    msg.y = y;
    msg.z = z;
    float f[3];
    //agent_camera->forward_vector(f);
    p->you->s.forward_vector(f);    // use network state
    msg.vx = f[0];
    msg.vy = f[1];
    msg.vz = f[2];
    msg.send();

    // local play (copied from ThrowGrenade_CtoS)
    if (!this->p->you->weapons.grenades.fire()) return;
    static const float PLAYER_ARM_FORCE = 15.0f; // make agent property
    //create grenade
    f[0] *= PLAYER_ARM_FORCE;
    f[1] *= PLAYER_ARM_FORCE;
    f[2] *= PLAYER_ARM_FORCE;
    Grenade* g = ClientState::grenade_list->create(x,y,z, f[0], f[1], f[2]);
    if (g==NULL) return;
    g->owner = this->p->agent_id;
    
}

void PlayerAgent_action::set_block() {
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    // get nearest empty block
    const float max_dist = 4.0f;
    const int z_low = 4;
    const int z_high = 3;

    Vec3 f = agent_camera->forward_vector();
    int* b = _farthest_empty_block(
        p->camera_state.x, p->camera_state.y, p->camera_z(),
        f.x, f.y, f.z,
        max_dist, z_low, z_high
    );
    if (b==NULL) return;

    agent_set_block_CtoS msg;
    msg.x = b[0];
    msg.y = b[1];
    msg.z = b[2];
    msg.val = p->you->weapons.blocks.block;
    msg.send();
    return;
}

void PlayerAgent_action::reload() {
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    AgentReloadWeapon_CtoS msg;
    msg.type = p->you->weapons.active_type();
    msg.send();
    Sound::reload();
}

bool PlayerAgent_action::switch_weapon(int i) {
    if (p->you == NULL) return false;
    if (p->you->status.dead) return false;
    if (p->you->status.team == 0) return false;

    static const int UP = -1;
    static const int DOWN = -2;

    bool switched = false;
    int old_active = p->you->weapons.active;

    if (i == UP) {
        p->you->weapons.switch_up();
    } else if (i == DOWN) {
        p->you->weapons.switch_down();
    } else {
        p->you->weapons.set_active(i);
    }

    if (p->you->weapons.active != old_active)
        switched = true;

    return switched;
}

void PlayerAgent_action::place_spawner()
{
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    Vec3 v = agent_camera->forward_vector();

    const float max_dist = 4.0f;
    const int z_low = 4;
    const int z_high = 3;
    int* block = _farthest_empty_block(
        this->p->camera_state.x, this->p->camera_state.y, this->p->camera_z(),
        v.x, v.y, v.z,
        max_dist, z_low, z_high
    );
    if (block == NULL) return;
    
    place_spawner_CtoS msg;
    msg.x = block[0];
    msg.y = block[1];
    msg.z = block[2];
    msg.send();
}

void PlayerAgent_action::place_turret()
{
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    Vec3 v = agent_camera->forward_vector();

    const float max_dist = 4.0f;
    const int z_low = 4;
    const int z_high = 3;
    int* block = _farthest_empty_block(
        this->p->camera_state.x, this->p->camera_state.y, this->p->camera_z(),
        v.x, v.y, v.z,
        max_dist, z_low, z_high
    );
    if (block == NULL) return;
    
    place_turret_CtoS msg;
    msg.x = block[0];
    msg.y = block[1];
    msg.z = block[2];
    msg.send();
}

int PlayerAgent_action::select_block()
{
    if (p->you == NULL) return 0;
    if (p->you->status.dead) return 0;
    if (p->you->status.team == 0) return 0;

    int block_type = this->p->you->get_facing_block_type();
    if (block_type)
        this->p->you->weapons.set_active_block(block_type);

    return block_type;
}

void PlayerAgent_action::remove_item_from_inventory()
{
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    Inventory* inv = this->p->you->status.inventory;
    inv->client_remove_any();   // TODO -- temp moethod
}

void PlayerAgent_action::add_item_to_inventory()
{
}


PlayerAgent_action::PlayerAgent_action(PlayerAgent_state* player_agent)
:
p(player_agent)
{}

