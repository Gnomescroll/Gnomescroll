#include "box.hpp"

#include <c_lib/agent/agent_list.hpp>
#include <c_lib/objects/common/interface/list.hpp>
#include <c_lib/state/client_state.hpp>

#include <c_lib/animations/_interface.hpp>

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
    Animations::create_hitscan_effect(
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


/* TODO : MOVE NET STUFF */

using Monsters::Box;
void box_chose_target(Box* box, object_choose_target_StoC* msg)
{
    box->en_route = false;  // cancel all motion

    box->target_id = msg->target_id;    // set target
    box->target_type = (Object_types)msg->target_type;
    box->locked_on_target = true;   // flag target lock
}

void box_chose_destination(Box* box, object_choose_destination_StoC* msg)
{
    box->destination = vec3_init(msg->x, msg->y, msg->z);
    box->ticks_to_destination = msg->ticks;

    // set momentum from destination :: TODO MOVE
    Vec3 direction = vec3_sub(box->destination, box->get_position());
    if (msg->ticks)
    {
        float len = vec3_length(direction);
        float speed = len / ((float)msg->ticks);
        box->speed = speed;
        box->at_destination = false;
        box->en_route = true;
        if (len)
        {
            normalize_vector(&direction);
            box->direction = direction;
        }
    }
    else
    {   // no ticks is equivalent to a teleport
        box->set_position(box->destination.x, box->destination.y, box->destination.z);
        box->speed = 0.0f;
        box->at_destination = true;
        box->en_route = false;
    }

    box->locked_on_target = false;  // TODO -- moving and locked on target?
}

#if DC_CLIENT

inline void object_choose_target_StoC::handle()
{
    using Monsters::Box;
    ObjectPolicyInterface* obj = ClientState::object_list->get((Object_types)type, id);
    if (obj == NULL) return;
    switch (type)
    {
        case OBJ_TYPE_MONSTER_BOX:
            box_chose_target((Box*)obj, this);
            break;
        default: return;
    }
}

inline void object_choose_destination_StoC::handle()
{
    using Monsters::Box;
    ObjectPolicyInterface* obj = ClientState::object_list->get((Object_types)type, id);
    if (obj == NULL) return;
    switch (type)
    {
        case OBJ_TYPE_MONSTER_BOX:
            box_chose_destination((Box*)obj, this);
            break;
        default: return;
    }
}

#endif

#if DC_SERVER

inline void object_choose_target_StoC::handle() {}
inline void object_choose_destination_StoC::handle() {}

#endif
