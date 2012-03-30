#include "turret.hpp"

#include <math.h>

#include <c_lib/common/random.h>
#include <c_lib/agent/net_agent.hpp>
#include <net_lib/net.hpp>
#include <c_lib/items/packets.hpp>
#include <c_lib/t_map/t_map.hpp>
#include <c_lib/ray_trace/handlers.hpp>

#if DC_CLIENT
/* Packet handlers */

void turret_state(object_state_StoC_model* msg)
{
    Turret* t = (Turret*)ClientState::object_list->get(msg->id);
    if (t == NULL) return;
    t->state()->set_position(msg->x, msg->y, msg->z);
}

void turret_create(object_create_owner_team_StoC_model* msg)
{
    Turret* t = (Turret*)ClientState::object_list->create(
        (int)msg->id,
        msg->x, msg->y, msg->z,
        0,0,0,
        (Object_types)msg->type
    );
    if (t == NULL)
    {
        printf("WARNING turret_create() -- could not create turret %d\n", msg->id);
        return;
    }
    t->state()->set_team(msg->team);
    t->state()->set_owner(msg->owner);
    t->born();
    //Sound::turret_placed(x,y,z,0,0,0);
    //system_message->turret_created(t);
}

void turret_destroy(int id)
{
    //Turret* t = (Turret*)ClientState::object_list->get(id);
    //system_message->turret_destroyed(t);
    ClientState::object_list->destroy(id);
}

void turret_shot_object(object_shot_object_StoC* msg)
{
    if (msg->target_type != OBJ_TYPE_AGENT) return; // remove this once turret can attack other objects

    Turret* t = (Turret*)ClientState::object_list->get((int)msg->id);
    if (t == NULL) return;
    Agent_state* a = ClientState::agent_list->get(msg->target_id);
    if (a == NULL || a->vox == NULL) return;
    a->vox->update(a->s.x, a->s.y, a->s.z, a->s.theta, a->s.phi);
    Voxel_volume* vv = a->vox->get_part(msg->target_part);
    if (vv == NULL) return;

    Vec3 pos = t->state()->get_position();
    Vec3 c = vv->get_center();
    Vec3 p = vec3_init(pos.x, pos.y, pos.z + t->state()->camera_height);

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
        voxel, t->state()->attacker_properties.voxel_damage_radius
    );
    Sound::turret_shoot(pos.x, pos.y, pos.z + t->state()->camera_height, 0,0,0);
}

void turret_shot_terrain(object_shot_terrain_StoC* msg)
{
    Turret *t = (Turret*)ClientState::object_list->get(msg->id);
    if (t == NULL) return;

    Vec3 pos = t->state()->get_position();
    const float hitscan_effect_speed = 200.0f;
    Vec3 p = vec3_init(pos.x, pos.y, pos.z + t->state()->camera_height);
    Vec3 c = vec3_init(msg->x, msg->y, msg->z);
    Vec3 v = vec3_sub(c, p); 
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    ClientState::hitscan_effect_list->create(
        p.x, p.y, p.z,
        v.x, v.y, v.z
    );

    Animations::block_damage(
        msg->x, msg->y, msg->z, p.x, p.y, p.z,
        msg->cube, msg->side
    );
    Animations::terrain_sparks(msg->x, msg->y, msg->z);
    Sound::laser_hit_block(msg->x, msg->y, msg->z, 0,0,0);
    Sound::turret_shoot(pos.x, pos.y, pos.z + t->state()->camera_height, 0,0,0);
}

void turret_shot_nothing(object_shot_nothing_StoC* msg)
{
    Turret *t = (Turret*)ClientState::object_list->get(msg->id);
    if (t == NULL) return;

    Vec3 pos = t->state()->get_position();
    const float hitscan_effect_speed = 200.0f;
    Vec3 v = vec3_init(msg->x, msg->y, msg->z);
    normalize_vector(&v);
    v = vec3_scalar_mult(v, hitscan_effect_speed);
    ClientState::hitscan_effect_list->create(
        pos.x, pos.y, pos.z + t->state()->camera_height,
        v.x, v.y, v.z
    );
    Sound::turret_shoot(pos.x, pos.y, pos.z + t->state()->camera_height, 0,0,0);
}
#endif

/* Turrets */

VoxDat turret_vox_dat;

/* Turret list */

//TODO -- implement this for general objects
//bool Turret_list::point_occupied(int x, int y, int z)
//{
    //for (int i=0; i<this->n_max; i++)
    //{
        //Turret *s = this->a[i];
        //if (s == NULL) continue;
        //if ((int)s->x == x && (int)s->y == y)
            //for (int j=0; j<(int)ceil(TURRET_HEIGHT); j++)
                //if ((int)s->z+j == z)
                    //return true;
    //}
    //return false;
//}

/* TargetAcquisition */

void TargetAcquisitionComponent::acquire_target(ObjectState* state)
{
    // firing properties (will be from dat/state)
    const float range = state->sight_range; 
    const float bias = state->accuracy_bias;
    const float acquisition_probability = state->target_acquisition_probability;
    const bool enemies = state->attack_enemies;
    const bool random = state->attack_random;

    Vec3 p = state->get_position();
    
    // lock on agent
    Vec3 firing_position = vec3_init(p.x, p.y, state->camera_z());
    Vec3 firing_direction;
    Agent_state* agent = Hitscan::lock_agent_target(
        firing_position, &firing_direction, state->team,
        range, acquisition_probability, enemies, random
    );
    if (agent == NULL) return;
    
    // normalize and bias vector
    normalize_vector(&firing_direction);
    if (bias)   // apply bias
        firing_direction = vec3_bias_random(firing_direction, bias);

    // get target
    Hitscan::HitscanTarget t = Hitscan::shoot_at_agent(
        firing_position, firing_direction, state->id, state->type,
        agent, range
    );

    // let handle target hit based on attacker properties
    Hitscan::handle_hitscan_target(t, state->attacker_properties);

    // send firing packet
    Hitscan::broadcast_object_fired(state->id, state->type, t);

    // apply custom handling
    // play sounds
    // play animations
}
