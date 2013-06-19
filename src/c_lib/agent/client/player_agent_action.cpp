#include "player_agent_action.hpp"

#if DC_SERVER
# error Do not include this file in the server
#endif

#include <physics/ray_trace/hitscan.hpp>
#include <agent/net_agent.hpp>
#include <sound/sound.hpp>
#include <particle/_include.hpp>
#include <particle/grenade.hpp>
#include <animations/_interface.hpp>
#include <animations/block_damage.hpp>
#include <hud/_interface.hpp>
#include <hud/cube_selector.hpp>
#include <t_map/net/t_CtoS.hpp>
#include <item/_interface.hpp>
#include <item/container/_interface.hpp>
#include <item/container/net/CtoS.hpp>
#include <item/properties.hpp>
#include <item/config/item_attribute.hpp>

namespace Agents
{

void PlayerAgentAction::update_mining_laser()
{
    class Agent* you = p->you();
    if (you == NULL) return;
    if (you->status.dead) return;
    if (!you->event.mining_laser_emitter.on) return;
    if (agent_camera == NULL) return;

    Vec3 origin = this->p->get_weapon_fire_animation_origin();
    GS_ASSERT(is_boxed_position(origin));

    const Vec3 position = agent_camera->get_position();
    const Vec3 forward = agent_camera->forward_vector();
    struct Vec3 focal_point = vec3_add(position, vec3_scalar_mult(forward, 50.0f));
    struct Vec3 direction = vec3_normalize(vec3_sub(focal_point, origin));

    you->event.mining_laser_emitter.h_mult = 0.75f;    // sprite scale offset
    you->event.mining_laser_emitter.length_position = position;
    you->event.mining_laser_emitter.length_direction = forward;
    you->event.mining_laser_emitter.set_state(origin, direction);
    you->event.mining_laser_emitter.tick();
    you->event.mining_laser_emitter.prep_draw();
}

void PlayerAgentAction::begin_mining_laser()
{
    class Agent* you = p->you();
    if (you == NULL) return;

    ItemType laser_type = Toolbelt::get_agent_selected_item_type(you->id);
    GS_ASSERT(isValid(laser_type));
    GS_ASSERT(Item::get_item_group_for_type(laser_type) == IG_MINING_LASER);
    float range = Item::get_weapon_range(laser_type);

    you->event.mining_laser_emitter.set_base_length(range);
    you->event.mining_laser_emitter.set_laser_type(laser_type);

    you->event.mining_laser_emitter.turn_on();

    IF_ASSERT(this->mining_laser_sound_id != NULL_SOUND_ID) return;
    this->mining_laser_sound_id = Sound::play_2d_sound("mining_laser");
}

void PlayerAgentAction::end_mining_laser()
{
    class Agent* you = p->you();
    if (you == NULL) return;

    you->event.mining_laser_emitter.turn_off();

    if (this->mining_laser_sound_id == NULL_SOUND_ID) return;
    Sound::stop_sound(this->mining_laser_sound_id);
    this->mining_laser_sound_id = NULL_SOUND_ID;
}

void PlayerAgentAction::fire_weapon(ItemType weapon_type)
{
    IF_ASSERT(weapon_type == NULL_ITEM_TYPE) return;
    class Agent* you = p->you();
    if (you == NULL) return;
    if (you->status.dead) return;

    using ClientState::hitscan;

    const Vec3 pos = this->p->get_weapon_fire_animation_origin();
    const Vec3 collision_point = hitscan.collision_point;
    const Vec3 trans_collision_point = quadrant_translate_position(pos, collision_point);
    Vec3 look;
    if (unlikely(vec3_equal(trans_collision_point, pos)))
        look = agent_camera->forward_vector();
    else
        look = vec3_normalize(vec3_sub(trans_collision_point, pos));

    SoundID sound_id = NULL_SOUND_ID;
    bool sound_3d = false;
    const char* sound_name = NULL;

    ItemGroup weapon_group = Item::get_item_group_for_type(weapon_type);
    float range = Item::get_weapon_range(weapon_type);
    float charge_progress = Toolbelt::get_charge_progress();
    int weapon_block_dmg = Item::get_item_block_damage(weapon_type, hitscan.cube_type);

    hitscan_entity_CtoS entity_msg;
    if (hitscan.type == HITSCAN_TARGET_VOXEL)
    {
        entity_msg.id = hitscan.voxel_target.entity_id;
        entity_msg.type = hitscan.voxel_target.entity_type;
    }
    else
    {
        entity_msg.id = hitscan.sprite_mob_entity_id;
        entity_msg.type = hitscan.sprite_mob_entity_type;
    }
    entity_msg.charge_progress = charge_progress;

    hitscan_block_CtoS block_msg;
    block_msg.position = hitscan.block_position;
    block_msg.charge_progress = charge_progress;

    hitscan_mech_CtoS mech_msg;
    mech_msg.mech_id = hitscan.mech_id;

    hitscan_none_CtoS none_msg;

    HitscanTargetType type = hitscan.type;
    if (hitscan.distance > range)
        type = HITSCAN_TARGET_NONE;

    switch (type)
    {
        case HITSCAN_TARGET_VOXEL:
            entity_msg.send();
            Animations::blood_spray(collision_point, look);
            sound_name = "pick_hit_agent";
            sound_3d = true;
            break;

        case HITSCAN_TARGET_BLOCK:
            block_msg.send();
            if (is_valid_z(hitscan.block_position))
            {   // update predicted dmg
                // TODO -- all of this should be in the toolbelt callback probably
                // get block dmg for selected weapon
                // if block pos matched last requested block pos
                // add it to hud draw settings predicted
                // else, set it to hud draw settings predicted
                Animations::damaging_block = true;
                if (t_map::is_last_requested_block(hitscan.block_position))
                    Animations::predicted_block_damage += weapon_block_dmg;
                else
                    Animations::predicted_block_damage = weapon_block_dmg;
                // make & record dmg request
                t_map::request_block_damage(hitscan.block_position);
            }

            Animations::block_damage(collision_point, look, hitscan.cube_type,
                                     hitscan.block_side);
            if (weapon_group == IG_HITSCAN_WEAPON)
                sound_name = "laser_hit_block";
            else
                sound_name = "block_took_damage";
            sound_3d = true;
            break;

        case HITSCAN_TARGET_MECH:
            mech_msg.send();
            sound_name = "pick_swung";
            break;

        case HITSCAN_TARGET_SPRITE_MOB:
            entity_msg.send();
            sound_name = "pick_hit_agent";
            break;

        case HITSCAN_TARGET_NONE:
            none_msg.send();
            sound_name = "pick_swung";
            break;
    }

    if (weapon_group == IG_HITSCAN_WEAPON)
    {
        Animations::create_railtrail_effect(pos, collision_point);
        sound_name = "fire_laser";
        sound_3d = false;
    }

    if (sound_name != NULL && weapon_group != IG_MINING_LASER)
    {
        if (sound_3d)
            sound_id = Sound::play_3d_sound(sound_name, collision_point);
        else
            sound_id = Sound::play_2d_sound(sound_name);
        float sound_gain_mult = 1.0f + charge_progress;
        Sound::set_gain_multiplier(sound_id, sound_gain_mult);
    }

    this->target_direction = look;
}

bool PlayerAgentAction::set_block(ItemID placer_id)
{
    class Agent* you = this->p->you();
    if (you == NULL || you->status.dead) return false;

    ItemType placer_type = Item::get_item_type(placer_id);
    IF_ASSERT(!isValid(placer_type)) return false;
    IF_ASSERT(placer_id == NULL_ITEM) return false;

    // get nearest empty block
    const float max_dist = 4.0f;
    class RaytraceData data;
    bool collided = raytrace_terrain(agent_camera->get_position(),
                                     agent_camera->forward_vector(),
                                     max_dist, &data);
    if (!collided) return false;

    Vec3i cube_point = data.get_pre_collision_point();
    if (!is_valid_z(cube_point)) return false;  // don't modify the floor

    Vec3 point = vec3_add(vec3_init(cube_point), vec3_init(0.5f));
    int orientation = axis_orientation(agent_camera->get_position(), point);

    Item::ItemAttribute* attr = Item::get_item_attributes(placer_type);
    IF_ASSERT(attr == NULL) return false;
    CubeType val = attr->cube_type;
    if (t_map::isItemContainer(val))
    {
        ItemContainer::create_container_block_CtoS msg;
        msg.position = cube_point;
        msg.placer_id = placer_id;
        msg.orientation = orientation;
        msg.send();
    }
    else
    {
        agent_set_block_CtoS msg;
        msg.position = cube_point;
        msg.placer_id = placer_id;
        msg.send();
    }
    return true;
}

void PlayerAgentAction::throw_grenade()
{
    class Agent* you = p->you();
    if (you == NULL) return;
    if (you->status.dead) return;

    float charge_progress = Toolbelt::get_charge_progress();

    // message to server
    Vec3 pos = you->get_camera_position();
    Vec3 f = you->forward_vector();    // use network state
    launch_projectile_CtoS msg;
    msg.position = pos;
    msg.velocity = f;
    msg.item_type = Toolbelt::get_selected_item_type();
    msg.charge_progress = charge_progress;
    msg.send();

    // local play (copied from launch_projectile_CtoS)
    static const float PLAYER_ARM_FORCE = 15.0f; // TODO make agent property
    f = vec3_scalar_mult(f, PLAYER_ARM_FORCE * (1.0f + charge_progress));

    Particle::Grenade* g = Particle::grenade_list->create();
    IF_ASSERT(g == NULL) return;
    g->set_state(pos, f);
    g->owner = this->p->agent_id;
}

void PlayerAgentAction::place_spawner()
{
    class Agent* you = p->you();
    if (you == NULL) return;
    if (you->status.dead) return;

    // TODO -- get from config
    const float max_dist = 4.0f;

    class RaytraceData data;
    bool collided = raytrace_terrain(agent_camera->get_position(),
                                     agent_camera->forward_vector(),
                                     max_dist, &data);
    if (!collided) return;

    place_spawner_CtoS msg;
    msg.position = data.get_pre_collision_point();
    msg.send();
}

void PlayerAgentAction::place_turret()
{
    class Agent* you = p->you();
    if (you == NULL) return;
    if (you->status.dead) return;

    // TODO -- get from config
    const float max_dist = 4.0f;

    class RaytraceData data;
    bool collided = raytrace_terrain(agent_camera->get_position(),
                                     agent_camera->forward_vector(),
                                     max_dist, &data);
    if (!collided) return;

    place_turret_CtoS msg;
    msg.position = data.get_pre_collision_point();
    msg.send();
}

Vec3 PlayerAgentAction::get_aiming_point()
{
    class Agent* you = p->you();
    if (you == NULL) return vec3_init(0);
    if (you->status.dead) return vec3_init(0);

    Vec3 pos = agent_camera->get_position();
    Vec3 look = agent_camera->forward_vector();

    using ClientState::hitscan;
    if (hitscan.type == HITSCAN_TARGET_VOXEL)
        return hitscan.voxel_collision_point;
    else if (hitscan.type == HITSCAN_TARGET_BLOCK)
        return vec3_add(pos, vec3_scalar_mult(look, hitscan.block_distance));
    else
        return vec3_init(0);
}

PlayerAgentAction::PlayerAgentAction(PlayerAgent* player_agent) :
    p(player_agent), mining_laser_sound_id(NULL_SOUND_ID),
    target_direction(vec3_init(1,0,0))
{
}

}   // Agents
