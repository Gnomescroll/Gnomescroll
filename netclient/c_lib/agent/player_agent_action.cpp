#include "player_agent_action.hpp"

#include <c_lib/ray_trace/hitscan.hpp>

void PlayerAgent_action::fire() {
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

#define BLOCK_PICK_MAX_DISTANCE 4.0f
void PlayerAgent_action::hit_block() {
    const int z_low = 4;
    const int z_high = 3;
    float f[3];
    p->camera_state.forward_vector(f);
    int *pos = _nearest_block(p->camera_state.x, p->camera_state.y, p->camera_state.z + p->camera_height(), f[0], f[1], f[2], BLOCK_PICK_MAX_DISTANCE, z_low, z_high);
    if (pos != NULL) {
        hit_block_CtoS* msg = new hit_block_CtoS(p->agent_id, pos[0], pos[1], pos[2]);
        msg->send();
    }
}
