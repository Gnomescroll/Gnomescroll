#include "player_agent_action.hpp"

#include <c_lib/ray_trace/hitscan.hpp>
#include <c_lib/animations/hitscan.hpp>

#include <c_lib/t_map/t_map_net.hpp>

void PlayerAgent_action::fire() {
    Agent_state* a = ClientState::agent_list.get(p->agent_id);
    if (a==NULL) return;

    int type = a->weapons.active_type();
    if (!a->weapons.fire()) return;
    switch (type) {
        case Weapons::TYPE_block_applier:
            set_block();
            break;
        case Weapons::TYPE_hitscan_laser:
            hitscan();
            break;
        case Weapons::TYPE_block_pick:
            hit_block();
            break;
        case Weapons::TYPE_grenade_thrower:
            throw_grenade();
            break;
        default:
            printf("PlayerAgent_action::fire -- No action defined for weapon type %d\n", type);
            break;
    }
    fire_weapon_CtoS msg;
    msg.id = p->agent_id;
    msg.type = a->weapons.active_type();   // just send type for now. weapons arent in a list
    msg.send();
}

void PlayerAgent_action::hitscan() {

    // play sound
    char soundfile[] = "laser_01.wav";
    Sound::play_2d_sound(soundfile);
    
    // do hitscan detection
    int target;
    int data[3];
    float vec[3];
    p->camera_state.forward_vector(vec);

    target = Hitscan::resolve_hitscan_target(
        p->camera_state.x, p->camera_state.y, p->camera_state.z + p->camera_height(),
        vec[0], vec[1], vec[2],
        data,
        p->agent_id
    );

    hitscan_agent_CtoS agent_msg;
    hitscan_block_CtoS block_msg;

    switch (target) {
        case Hitscan::HITSCAN_TARGET_AGENT:
            agent_msg.id = p->agent_id;
            agent_msg.agent_id = data[0];   // target
            agent_msg.body_part = data[1];
            agent_msg.send();
            break;

        case Hitscan::HITSCAN_TARGET_BLOCK:
            block_msg.id = p->agent_id;
            block_msg.x = data[0];
            block_msg.y = data[1];
            block_msg.z = data[2];
            block_msg.send();
            break;
            
        case Hitscan::HITSCAN_TARGET_NONE:
            break;
        default:
            break;
    }

    // play laser anim
    const float hitscan_speed = 120.0f;
    ClientState::hitscan_effect_list.create(
        p->camera_state.x, p->camera_state.y, p->camera_state.z + p->camera_height(),
        vec[0]*hitscan_speed, vec[1]*hitscan_speed, vec[2]*hitscan_speed
    );
}

void PlayerAgent_action::throw_grenade() {
    ThrowGrenade_CtoS msg;
    msg.id = p->agent_id;

    msg.x = p->camera_state.x;
    msg.y = p->camera_state.y;
    msg.z = p->camera_state.z + p->camera_height();

    float f[3];
    p->camera_state.forward_vector(f);
    msg.vx = f[0];
    msg.vy = f[1];
    msg.vz = f[2];
    
    msg.send();
}

#define BLOCK_PICK_MAX_DISTANCE 4.0f
void PlayerAgent_action::hit_block() {
    const int z_low = 4;
    const int z_high = 3;
    float f[3];
    p->camera_state.forward_vector(f);
    int *pos = _nearest_block(p->camera_state.x, p->camera_state.y, p->camera_state.z + p->camera_height(), f[0], f[1], f[2], BLOCK_PICK_MAX_DISTANCE, z_low, z_high);
    if (pos != NULL) {
        hit_block_CtoS msg;
        msg.id = p->agent_id;
        msg.x = pos[0];
        msg.y = pos[1];
        msg.z = pos[2];
        msg.send();
    }
}

void PlayerAgent_action::set_block() {
    Agent_state* a = ClientState::agent_list.get(p->agent_id);
    if (a==NULL) return;

    // get nearest empty block
    const float max_dist = 4.0f;
    const int z_low = 4;
    const int z_high = 3;

    float f[3];
    a->s.forward_vector(f);
    int* b = _farthest_empty_block(
        a->s.x, a->s.y, a->s.z + a->camera_height(),
        f[0], f[1], f[2],
        max_dist, z_low, z_high
    );
    if (b==NULL) return;
    
    block_CtoS msg;
    // UPDATE WITH FACING POSITION
    msg.x = b[0];
    msg.y = b[1];
    msg.z = b[2];
    msg.val = a->weapons.blocks.block;
    msg.send();
}

void PlayerAgent_action::reload() {
    Agent_state* a = ClientState::agent_list.get(p->agent_id);
    if (a==NULL) return;

    AgentReloadWeapon_CtoS msg;
    msg.id = p->agent_id;
    msg.type = a->weapons.active_type();
    msg.send();
}

void PlayerAgent_action::switch_weapon(int i) {
    static const int UP = -1;
    static const int DOWN = -2;

    Agent_state* a = ClientState::agent_list.get(p->agent_id);
    if (a==NULL) return;

    if (i == UP) {
        a->weapons.switch_up();
    } else if (i == DOWN) {
        a->weapons.switch_down();
    } else {
        a->weapons.set_active(i);
    }
}
