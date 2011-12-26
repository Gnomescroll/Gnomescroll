#include "player_agent_action.hpp"

#include <c_lib/ray_trace/hitscan.hpp>

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
}

void PlayerAgent_action::hitscan() {

    // play sound
    char soundfile[] = "laser_01.wav";
    Sound::play_2d_sound(soundfile);
    
    // send fire weapon packet
    const int weapon_id = 1;    // TODO: move this
    fire_weapon_CtoS* fire_msg = new fire_weapon_CtoS(p->agent_id, weapon_id);
    fire_msg->send();

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

    hitscan_agent_CtoS* agent_msg;
    hitscan_block_CtoS* block_msg;

    switch (target) {
        case Hitscan::HITSCAN_TARGET_AGENT:
            agent_msg = new hitscan_agent_CtoS(p->agent_id, data[0], data[1]);
            agent_msg->send();
            //printf("hit agent\n");
            break;

        case Hitscan::HITSCAN_TARGET_BLOCK:
            block_msg = new hitscan_block_CtoS(p->agent_id, data[0], data[1], data[2]);
            block_msg->send();
            //printf("hit block\n");
            break;
            
        case Hitscan::HITSCAN_TARGET_NONE:
            //printf("hit nothin\n");
        default:
            break;
    }
}

void PlayerAgent_action::throw_grenade() {
    static ThrowGrenade_CtoS msg;
    msg.id = p->agent_id;

    msg.x = p->camera_state.x;
    msg.y = p->camera_state.y;
    msg.z = p->camera_state.z + p->camera_height();

    static float f[3];
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
        static hit_block_CtoS msg;
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

    static block_CtoS msg;
    // UPDATE WITH FACING POSITION
    msg.x = 0;
    msg.y = 0;
    msg.z = 0;
    msg.val = a->weapons.blocks.block;
    msg.send();
}

void PlayerAgent_action::reload() {
    Agent_state* a = ClientState::agent_list.get(p->agent_id);
    if (a==NULL) return;

    static AgentReloadWeapon_CtoS msg;
    msg.id = p->agent_id;
    msg.type = a->weapons.active_type();
    msg.send();
}
