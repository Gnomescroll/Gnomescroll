#include "box.hpp"

#include <c_lib/agent/agent_list.hpp>
#include <c_lib/objects/common/interface/list.hpp>
#include <c_lib/state/client_state.hpp>

namespace Monsters
{

VoxDat box_vox_dat;

#if DC_CLIENT
void box_shot_object(object_shot_object_StoC* msg)
{
    if (msg->target_type != OBJ_TYPE_AGENT) return; // remove this once turret can attack other objects

    Box* t = (Box*)ClientState::object_list->get((Object_types)msg->type, (int)msg->id);
    if (t == NULL) return;
    Agent_state* a = ClientState::agent_list->get(msg->target_id);
    if (a == NULL || a->vox == NULL) return;
    a->vox->update(a->s.x, a->s.y, a->s.z, a->s.theta, a->s.phi);   // TODO: why is model being updated?
    Voxel_volume* vv = a->vox->get_part(msg->target_part);
    if (vv == NULL) return;

    Vec3 pos = t->get_position();
    Vec3 c = vv->get_center();
    Vec3 p = vec3_init(pos.x, pos.y, t->camera_z());

    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_sub(c, p);
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    ClientState::hitscan_effect_list->create(
        p.x, p.y, p.z,
        v.x, v.y, v.z
    );
    int voxel[3] = { msg->voxel_x, msg->voxel_y, msg->voxel_z };
    destroy_object_voxel(
        msg->target_id, msg->target_type, msg->target_part,
        voxel, t->attacker_properties.voxel_damage_radius
    );
    Sound::turret_shoot(pos.x, pos.y, t->camera_z(), 0,0,0);
}
#endif

} // Monsters
