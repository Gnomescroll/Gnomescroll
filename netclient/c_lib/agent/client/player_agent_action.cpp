#include "player_agent_action.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <physics/ray_trace/hitscan.hpp>

#include <agent/net_agent.hpp>
#include <sound/triggers.hpp>

#include <particle/_include.hpp>
#include <particle/grenade.hpp>

#include <animations/_interface.hpp>

#include <hud/cube_selector.hpp>
#include <t_map/net/t_CtoS.hpp>

#include <item/_interface.hpp>
#include <item/container/_interface.hpp>
#include <item/container/net/CtoS.hpp>
#include <item/properties.hpp>
#include <item/config/item_attribute.hpp>

void PlayerAgent_action::hitscan_laser()
{
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    Vec3 pos = this->p->camera_position();
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
            pos, look, this->p->agent_id, OBJECT_AGENT,
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
    //int voxel_blast_radius = 1;
    
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

            if (target.entity_type == OBJECT_AGENT)
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
                //voxel_blast_radius = 3;
            }
            //else if (target.entity_type == OBJECT_MONSTER_BOMB)
            //{
                //voxel_blast_radius = 2;
            //}
            //destroy_object_voxel(target.entity_id, target.entity_type, target.part_id, target.voxel, voxel_blast_radius);
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
    Animations::create_hitscan_effect(
        origin[0], origin[1], origin[2],
        look.x, look.y, look.z
    );
}

void PlayerAgent_action::tick_mining_laser()
{
    if (this->p->you == NULL) return;

    int weapon_type = Item::get_item_type((char*)"mining_laser");
    float range = Item::get_weapon_range(weapon_type);

    Vec3 origin = this->p->get_weapon_fire_animation_origin();
    Animations::mining_laser_beam(origin, this->target_direction, range);
}

void PlayerAgent_action::fire_close_range_weapon(int weapon_type)
{    
    if (p->you == NULL) return;
    if (p->you->status.dead) return;
    if (p->you->status.team == 0) return;

    GS_ASSERT(weapon_type != NULL_ITEM_TYPE);
    if (weapon_type == NULL_ITEM_TYPE) return;

    float range = Item::get_weapon_range(weapon_type);

    Vec3 pos = this->p->camera_position();
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
            pos, look, this->p->agent_id, OBJECT_AGENT,
            &target, &vox_distance, collision_point,
            block_pos, side, &tile, &block_distance
        );

    normalize_vector(&look);    // already normalized?

    // send packet
    hit_block_CtoS block_msg;
    melee_object_CtoS obj_msg;

    Agent_state* agent;
    //int voxel_blast_radius = 1;

    switch (target_type)
    {
        case Hitscan::HITSCAN_TARGET_VOXEL:
            if (vox_distance > range)
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
            obj_msg.weapon_type = weapon_type;
            obj_msg.send();

            if (target.entity_type == OBJECT_AGENT)
            {
                agent = ClientState::agent_list->get(target.entity_id);
                if (agent==NULL)
                {
                    target_type = Hitscan::HITSCAN_TARGET_NONE;
                    break;
                }
                if (agent->status.team == this->p->you->status.team)
                {
                    target_type = Hitscan::HITSCAN_TARGET_NONE;
                    break;
                }
                Animations::blood_spray(
                    collision_point[0], collision_point[1], collision_point[2],
                    look.x, look.y, look.z
                );
                Sound::pick_hit_agent(
                    collision_point[0], collision_point[1], collision_point[2],
                    0,0,0
                );
                //voxel_blast_radius = 3;
            }
            //else if (target.entity_type == OBJECT_MONSTER_BOMB)
            //{
                //voxel_blast_radius = 2;
            //}
            //destroy_object_voxel(target.entity_id, target.entity_type, target.part_id, target.voxel, voxel_blast_radius);
            break;

        case Hitscan::HITSCAN_TARGET_BLOCK:
            if (block_distance > range)
            {
                target_type = Hitscan::HITSCAN_TARGET_NONE;
                break;
            }
            block_msg.x = block_pos[0];
            block_msg.y = block_pos[1];
            block_msg.z = block_pos[2];
            block_msg.weapon_type = weapon_type;
            block_msg.send();

            // FOR SOME REASON COLLISION_POINT IS WORTHLESS AND WE HAVE TO CALCULATE IT HERE.

            // multiply look vector by distance to collision
            look = vec3_scalar_mult(look, block_distance);
            // add agent position, now we have collision point
            look = vec3_add(look, pos);
            // copy this to collision_point, for block damage animation
            collision_point[0] = look.x;
            collision_point[1] = look.y;
            collision_point[2] = look.z;
            
            // subtract translated animation origin from collision point (look) to get new vector
            look = vec3_sub(vec3_init(collision_point[0], collision_point[1], collision_point[2]), this->p->get_weapon_fire_animation_origin());
            normalize_vector(&look);

            Animations::block_damage(
                collision_point[0], collision_point[1], collision_point[2],
                look.x, look.y, look.z,
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

    this->target_direction = look;
}

void PlayerAgent_action::set_block(ItemID placer_id)
{
    GS_ASSERT(placer_id != NULL_ITEM);
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

    int placer_type = Item::get_item_type(placer_id);
    GS_ASSERT(placer_type != NULL_ITEM_TYPE);
    Item::ItemAttribute* attr = Item::get_item_attributes(placer_type);
    GS_ASSERT(attr != NULL);
    int val = attr->placer_block_type_id;
    if (t_map::get_container_type_for_block(val) != CONTAINER_TYPE_NONE)
    {
        ItemContainer::create_container_block_CtoS msg;
        msg.x = b[0];
        msg.y = b[1];
        msg.z = b[2];
        msg.placer_id = placer_id;
        msg.send();
    }
    else
    {
        agent_set_block_CtoS msg;
        msg.x = b[0];
        msg.y = b[1];
        msg.z = b[2];
        msg.placer_id = placer_id;
        msg.send();
    }
}

#if !PRODUCTION
void PlayerAgent_action::admin_set_block()
{
    if (p->you == NULL) return;

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

    // get block value from somewhere
    int val = HudCubeSelector::cube_selector.get_active_id();

    admin_set_block_CtoS msg;
    msg.x = b[0];
    msg.y = b[1];
    msg.z = b[2];
    msg.val = val;
    msg.send();
}
#endif

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
    //if (!this->p->you->weapons.grenades.fire()) return;
    static const float PLAYER_ARM_FORCE = 15.0f; // make agent property
    //create grenade
    f[0] *= PLAYER_ARM_FORCE;
    f[1] *= PLAYER_ARM_FORCE;
    f[2] *= PLAYER_ARM_FORCE;
    Particle::Grenade* g = Particle::grenade_list->create();
    g->set_state(x,y,z, f[0], f[1], f[2]);
    if (g==NULL) return;
    g->owner = this->p->agent_id;
    
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

Vec3 PlayerAgent_action::get_aiming_point()
{
    if (p->you == NULL) return vec3_init(0,0,0);
    if (p->you->status.dead) return vec3_init(0,0,0);
    if (p->you->status.team == 0) return vec3_init(0,0,0);

    Vec3 pos = this->p->camera_position();
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
            pos, look, this->p->agent_id, OBJECT_AGENT,
            &target, &vox_distance, collision_point,
            block_pos, side, &tile, &block_distance
        );
    if (target_type == Hitscan::HITSCAN_TARGET_VOXEL) return vec3_init(collision_point[0], collision_point[1], collision_point[2]);
    else if (target_type == Hitscan::HITSCAN_TARGET_BLOCK) return vec3_add(pos, vec3_scalar_mult(look, block_distance));
    else return vec3_init(0,0,0);
}

PlayerAgent_action::PlayerAgent_action(PlayerAgent_state* player_agent)
:
p(player_agent)
{}

