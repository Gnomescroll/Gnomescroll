#include "turret.hpp"

#include <math.h>

#include <c_lib/common/random.h>
#include <c_lib/agent/net_agent.hpp>
#include <c_lib/entity/network/packets.hpp>
#include <c_lib/t_map/t_map.hpp>
#include <c_lib/ray_trace/handlers.hpp>

#if DC_CLIENT
#include <c_lib/animations/_interface.hpp>
#endif

#if DC_CLIENT
/* Packet handlers */

// TODO - some way to generalize this
void turret_shot_object(object_shot_object_StoC* msg)
{
    if (msg->target_type != OBJ_TYPE_AGENT) return; // remove this once turret can attack other objects

    Turret* t = (Turret*)ClientState::object_list->get((Object_types)msg->type, (int)msg->id);
    if (t == NULL) return;
    Agent_state* a = ClientState::agent_list->get(msg->target_id);
    if (a == NULL || a->vox == NULL) return;
    a->vox->update(a->s.x, a->s.y, a->s.z, a->s.theta, a->s.phi);   // TODO: why is model being updated?
    Voxel_volume* vv = a->vox->get_part(msg->target_part);
    if (vv == NULL) return;

    Vec3 pos = t->get_position();
    Vec3 c = vv->get_center();
    Vec3 p = vec3_init(pos.x, pos.y, t->spatial.camera_z());

    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_sub(c, p);
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(
        p.x, p.y, p.z,
        v.x, v.y, v.z
    );
    int voxel[3] = { msg->voxel_x, msg->voxel_y, msg->voxel_z };
    destroy_object_voxel(
        msg->target_id, msg->target_type, msg->target_part,
        voxel, t->attacker_properties.voxel_damage_radius
    );
    Sound::turret_shoot(pos.x, pos.y, t->spatial.camera_z(), 0,0,0);
}

void turret_shot_terrain(object_shot_terrain_StoC* msg)
{
    Turret *t = (Turret*)ClientState::object_list->get((Object_types)msg->type, msg->id);
    if (t == NULL) return;

    Vec3 pos = t->get_position();
    const float hitscan_effect_speed = 200.0f;
    Vec3 p = vec3_init(pos.x, pos.y, t->spatial.camera_z());
    Vec3 c = vec3_init(msg->x, msg->y, msg->z);
    Vec3 v = vec3_sub(c, p); 
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(
        p.x, p.y, p.z,
        v.x, v.y, v.z
    );

    Animations::block_damage(
        msg->x, msg->y, msg->z, p.x, p.y, p.z,
        msg->cube, msg->side
    );
    Animations::terrain_sparks(msg->x, msg->y, msg->z);
    Sound::laser_hit_block(msg->x, msg->y, msg->z, 0,0,0);
    Sound::turret_shoot(pos.x, pos.y, t->spatial.camera_z(), 0,0,0);
}

void turret_shot_nothing(object_shot_nothing_StoC* msg)
{
    Turret *t = (Turret*)ClientState::object_list->get((Object_types)msg->type, msg->id);
    if (t == NULL) return;

    Vec3 pos = t->get_position();
    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_init(msg->x, msg->y, msg->z);
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    Animations::create_hitscan_effect(
        pos.x, pos.y, t->spatial.camera_z(),
        v.x, v.y, v.z
    );
    Sound::turret_shoot(pos.x, pos.y, t->spatial.camera_z(), 0,0,0);
}
#endif

/* Turrets */

VoxDat turret_vox_dat;
