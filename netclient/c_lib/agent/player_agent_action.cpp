#include "player_agent_action.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/animations/hitscan.hpp>
#include <common/enum_types.hpp>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/t_map/t_map_net.hpp>

#define BLOCK_PICK_MAX_DISTANCE 4.0f
#define MELEE_PICK_MAX_DISTANCE 2.7f

void PlayerAgent_action::fire() {
    if (p->you == NULL) return;

    int type = p->you->weapons.active_type();
    if (!p->you->weapons.fire()) return;
    switch (type) {
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
        default:
            printf("PlayerAgent_action::fire -- No action defined for weapon type %d\n", type);
            break;
    }
}

void PlayerAgent_action::hitscan_laser() {
    
    // get camera vector
    float vec[3];
    agent_camera->forward_vector(vec);

    // get camera position
    float x,y,z;
    x = p->camera_state.x;
    y = p->camera_state.y;
    z = p->camera_state.z + p->you->camera_height();

    // hitscan against voxels
    float vox_distance = 10000000.0f;
    float collision_point[3];
    Voxel_hitscan_target target;
    bool voxel_hit = ClientState::voxel_hitscan_list.hitscan(
        x,y,z,
        vec[0], vec[1], vec[2],
        p->agent_id, OBJ_TYPE_AGENT,
        collision_point, &vox_distance,
        &target
    );

    //if (voxel_hit)
    //{
        //printf("entity_id=%d entity_type=%d part_id=%d\n", vhe.entity_id, vhe.entity_type, vhe.part_id);
        //if (vhe.entity_type == OBJ_TYPE_SPAWNER) printf("... is a spawner\n");
    //}
    
    // hitscan against terrain
    float block_distance = 10000000.0f;
    int block_pos[3];
    int side[3];
    int tile;
    int target_type;
    target_type = Hitscan::terrain(
        x,y,z,
        vec[0], vec[1], vec[2],
        block_pos, &block_distance,
        side, &tile
    );

    // choose closer collision (or none)
    const int TARGET_NONE = 0;
    const int TARGET_VOXEL = 1;
    const int TARGET_BLOCK = 2;
    
    bool block_hit = (target_type == Hitscan::HITSCAN_TARGET_BLOCK);
    bool voxel_closer = (vox_distance <= block_distance);
    //bool block_closer = (block_distance > vox_distance);

    if (voxel_hit && block_hit) {
        if (voxel_closer) {
            target_type = TARGET_VOXEL;
        } else {
            target_type = TARGET_BLOCK;
        }
    } else if (voxel_hit) {
        target_type = TARGET_VOXEL;
    } else if (block_hit) {
        target_type = TARGET_BLOCK;
    } else {
        target_type = TARGET_NONE;
    }

    // for hitscan animation:
    // readjust the vector so that the translated position points to target
    // get the right vector for translating the hitscan laser anim
    struct Vector look = Vector_init(vec[0], vec[1], vec[2]);
    normalize_vector(&look);
    struct Vector up = Vector_init(0,0,1);
    struct Vector right = vector_cross(look, up);
    normalize_vector(&right);

    // magic offset numbers found by rotating the laser to the right spot
    // fixed in the bottom right corner
    const float dxy = 0.14;
    const float dz = -0.13;

    // animation origin
    float origin[3];
    origin[0] = x + dxy * right.x;
    origin[1] = y + dxy * right.y;
    origin[2] = z + dz;

    // send packet
    hitscan_block_CtoS block_msg;
    hitscan_none_CtoS none_msg;
    hitscan_object_CtoS obj_msg;
    
    switch (target_type) {
        case TARGET_VOXEL:
            obj_msg.id = target.entity_id;
            obj_msg.type = target.entity_type;
            obj_msg.part = target.part_id;
            obj_msg.voxel[0] = target.voxel[0];
            obj_msg.voxel[1] = target.voxel[1];
            obj_msg.voxel[2] = target.voxel[2];
            obj_msg.send();
            //printf("msg: id=%d type=%d part=%d\n", obj_msg.id, obj_msg.type, obj_msg.part);
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
            }
            target.vv->set(target.voxel[0], target.voxel[1], target.voxel[2],0,0,0,0);  // delete voxel in model
            break;

        case TARGET_BLOCK:            
            block_msg.x = block_pos[0];
            block_msg.y = block_pos[1];
            block_msg.z = block_pos[2];
            block_msg.send();

            // multiply look vector by distance to collision
            look.x *= block_distance;
            look.y *= block_distance;
            look.z *= block_distance;
            // add agent position, now we have collision point
            look.x += x;
            look.y += y;
            look.z += z;
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
            
            break;
            
        case TARGET_NONE:
            // for no target, leave translated animation origin
            none_msg.send();    // server will know to forward a fire weapon packet
            break;
        default:
            break;
    }

    // play sound
    char soundfile[] = "laser_01.wav";
    Sound::play_2d_sound(soundfile);

    // play laser anim
    const float hitscan_speed = 200.0f;
    ClientState::hitscan_effect_list.create(
        origin[0], origin[1], origin[2],
        look.x*hitscan_speed, look.y*hitscan_speed, look.z*hitscan_speed
    );
}

void PlayerAgent_action::hitscan_pick() {

    // TODO: implement hitscan methods that take in a max distance

    // get camera vector
    float vec[3];
    agent_camera->forward_vector(vec);

    // get camera position
    float x,y,z;
    x = p->camera_state.x;
    y = p->camera_state.y;
    z = p->camera_state.z + p->you->camera_height();

    // hitscan against voxels
    float vox_distance = 10000000.0f;
    float collision_point[3];
    struct Voxel_hitscan_target target;
    bool voxel_hit = ClientState::voxel_hitscan_list.hitscan(
        x,y,z,
        vec[0], vec[1], vec[2],
        p->agent_id, OBJ_TYPE_AGENT,
        collision_point, &vox_distance,
        &target
    );

     //hitscan against terrain
    float block_distance = 10000000.0f;
    int block_pos[3];
    int side[3];
    int tile;
    int target_type;
    target_type = Hitscan::terrain(
        x,y,z,
        vec[0], vec[1], vec[2],
        block_pos, &block_distance,
        side, &tile
    );

    // choose closer collision (or none)
    const int TARGET_NONE = 0;
    const int TARGET_VOXEL = 1;
    const int TARGET_BLOCK = 2;
    
    bool block_hit = (target_type == Hitscan::HITSCAN_TARGET_BLOCK);
    bool voxel_closer = (vox_distance <= block_distance);
    //bool block_closer = (block_distance > vox_distance);

    if (voxel_hit && block_hit)
        if (voxel_closer)
            target_type = TARGET_VOXEL;
        else
            target_type = TARGET_BLOCK;
    else if (voxel_hit)
        target_type = TARGET_VOXEL;
    else if (block_hit)
        target_type = TARGET_BLOCK;
    else
        target_type = TARGET_NONE;

    // send packet
    hit_block_CtoS block_msg;
    melee_object_CtoS obj_msg;
    
    switch (target_type) {
        case TARGET_VOXEL:
            if (vox_distance > MELEE_PICK_MAX_DISTANCE)
            {
                target_type = TARGET_NONE;
                break;
            }
        
            obj_msg.id = target.entity_id;
            obj_msg.type = target.entity_type;
            obj_msg.part = target.part_id;
            obj_msg.voxel[0] = target.voxel[0];
            obj_msg.voxel[1] = target.voxel[1];
            obj_msg.voxel[2] = target.voxel[2];
            obj_msg.send();

            if (target.entity_type == OBJ_TYPE_AGENT)
            {
                Animations::blood_spray(
                    collision_point[0], collision_point[1], collision_point[2],
                    vec[0], vec[1], vec[2]
                );
            }
            target.vv->set(target.voxel[0], target.voxel[1], target.voxel[2],0,0,0,0);  // delete voxel in model
            
            break;

        case TARGET_BLOCK:
            if (block_distance > BLOCK_PICK_MAX_DISTANCE)
            {
                target_type = TARGET_NONE;
                break;
            }
            block_msg.x = block_pos[0];
            block_msg.y = block_pos[1];
            block_msg.z = block_pos[2];
            block_msg.send();

            collision_point[0] = x + vec[0] * block_distance;
            collision_point[1] = y + vec[1] * block_distance;
            collision_point[2] = z + vec[2] * block_distance;

            Animations::block_damage(
                collision_point[0], collision_point[1], collision_point[2],
                vec[0], vec[1], vec[2],
                tile, side
            );
            
            break;
            
        default:
            break;
    }

    if (target_type == TARGET_NONE)
    {
        melee_none_CtoS none_msg;
        none_msg.send();
    }

    //// play sound
    //char soundfile[] = "pick_swing.wav";
    //Sound::play_2d_sound(soundfile);
}

void PlayerAgent_action::throw_grenade() {
    ThrowGrenade_CtoS msg;

    msg.x = p->camera_state.x;
    msg.y = p->camera_state.y;
    msg.z = p->camera_state.z + p->you->camera_height();

    float f[3];
    agent_camera->forward_vector(f);
    msg.vx = f[0];
    msg.vy = f[1];
    msg.vz = f[2];
    
    msg.send();
}

//void PlayerAgent_action::hit_block() {
    //const int z_low = 4;
    //const int z_high = 3;
    //float f[3];
    //agent_camera->forward_vector(f);
    //int *pos = _nearest_block(p->camera_state.x, p->camera_state.y, p->camera_state.z + p->you->camera_height(), f[0], f[1], f[2], BLOCK_PICK_MAX_DISTANCE, z_low, z_high);
    //if (pos != NULL) {
        //hit_block_CtoS msg;
        //msg.x = pos[0];
        //msg.y = pos[1];
        //msg.z = pos[2];
        //msg.send();
    //}
//}

void PlayerAgent_action::set_block() {
    if (p->you == NULL) return;

    // get nearest empty block
    const float max_dist = 4.0f;
    const int z_low = 4;
    const int z_high = 3;

    float f[3];
    agent_camera->forward_vector(f);
    int* b = _farthest_empty_block(
        p->you->s.x, p->you->s.y, p->you->s.z + p->you->camera_height(),
        f[0], f[1], f[2],
        max_dist, z_low, z_high
    );
    if (b==NULL) return;

    agent_block_CtoS msg;
    msg.x = b[0];
    msg.y = b[1];
    msg.z = b[2];
    msg.val = p->you->weapons.blocks.block;
    msg.send();
    return;
}

void PlayerAgent_action::reload() {
    if (p->you == NULL) return;

    AgentReloadWeapon_CtoS msg;
    msg.type = p->you->weapons.active_type();
    msg.send();
}

void PlayerAgent_action::switch_weapon(int i) {
    static const int UP = -1;
    static const int DOWN = -2;

    if (p->you == NULL) return;

    if (i == UP) {
        p->you->weapons.switch_up();
    } else if (i == DOWN) {
        p->you->weapons.switch_down();
    } else {
        p->you->weapons.set_active(i);
    }
}

void PlayerAgent_action::place_spawner()
{
    if (this->p->you == NULL) return;
    
    AgentState* state = &this->p->s1;
    float v[3];
    agent_camera->forward_vector(v);

    int* _farthest_empty_block(float x, float y, float z, float vx, float vy, float vz, float max_distance, int z_low, int z_high);
    const float max_dist = 4.0f;
    const int z_low = 4;
    const int z_high = 3;
    int* block = _farthest_empty_block(
        state->x, state->y, state->z + this->p->you->camera_height(),
        v[0], v[1], v[2],
        max_dist, z_low, z_high
    );
    if (block == NULL) return;
    
    place_spawner_CtoS msg;
    msg.x = block[0];
    msg.y = block[1];
    msg.z = block[2];
    msg.send();
}

PlayerAgent_action::PlayerAgent_action(PlayerAgent_state* player_agent)
:
p(player_agent)
{}

