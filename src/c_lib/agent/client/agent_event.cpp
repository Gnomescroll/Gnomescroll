#include "agent_event.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <input/handlers.hpp>
#include <state/client_state.hpp>
#include <sound/sound.hpp>
#include <chat/_interface.hpp>
#include <hud/map.hpp>
#include <particle/_interface.hpp>
#include <particle/constants.hpp>
#include <animations/_interface.hpp>
#include <animations/animations.hpp>
#include <animations/emitter.hpp>
#include <item/properties.hpp>
#include <common/color.hpp>

namespace Agents
{

void AgentEvent::name_set()
{
    IF_ASSERT(this->a->status.name == NULL) return;
    this->bb.set_text(this->a->status.name);
}

void AgentEvent::update_hud_name()
{
    const float z_margin = 0.4f;
    Vec3 p = this->a->get_position();
    this->bb.set_state(p.x, p.y, p.z + a->current_height() + z_margin, 0.0f, 0.0f, 0.0f);
    using namespace AgentHudName;
    Color color = HEALTH_TEXT_DEAD_COLOR;
    if (this->a->status.dead)
    {
        this->bb.set_color(color);
        return;
    }

    // calculate interpolated color from health ratio and color control health_color_points
    int health = get_attribute_int(this->a->id, "health");
    int health_max = get_attribute_int(this->a->id, "health_max");
    float h = float(health) / float(health_max);
    if (h >= health_color_points[0])
        color = health_colors[0];    // degenerate case, out of range
    else
    {
        for (size_t i=0; i<COLOR_COUNT-1; i++)
        {
            if (h < health_color_points[i] && h >= health_color_points[i+1])
            {
                float ratio = ((health_color_points[i] - h) /
                               (health_color_points[i] -
                                health_color_points[i+1]));
                color = interpolate_color(health_colors[i], health_colors[i+1],
                                          ratio);
                break;
            }
        }
    }
    this->bb.set_color(color);
}

void AgentEvent::draw_badges()
{
    using TextureSheetLoader::badge_texture_sheet_loader;
    const float margin = 2.0f;
    const float w = badge_texture_sheet_loader->tile_size;
    const float h = badge_texture_sheet_loader->tile_size;
    const float sw = badge_texture_sheet_loader->sprite_width();
    const float sh = badge_texture_sheet_loader->sprite_height();

    // keep it centered with the line
    float x = this->bb.x;
    float y = this->bb.y - h - (h - HudFont::font->data.line_height) / 2.0f;

    for (size_t i=0; i<this->a->status.n_badges; i++)
    {
        x -= w + margin;
        BadgeType badge_id = this->a->status.badges[i];
        int sprite_id = Badges::get_badge_sprite(badge_id);
        float sx = 0.0f;
        float sy = 0.0f;
        badge_texture_sheet_loader->get_sprite_coordinates(sprite_id, &sx, &sy);
        if (!rect_intersects(x, y, w, h, 0, 0, _xresf, _yresf)) continue;
        draw_bound_texture_sprite(x, y, w, h, this->bb.depth, sx, sy, sw, sh);
    }
}

void AgentEvent::took_damage(int amount)
{
    // TODO -- restore calling this for other agents. it got cut out due to
    // the attribute system bypassing it.
    // We need the play damage grunts always,
    // and we need to show damage indicators when we did the damage
    IF_ASSERT(amount <= 0) return;
    Vec3 position = this->a->get_center();
    const char sound_fmt[] = "agent_took_damage_%d";
    static char sound_str[sizeof(sound_fmt)] = {'\0'};
    snprintf(sound_str, sizeof(sound_fmt), sound_fmt, randrange(1, 3));
    sound_str[sizeof(sound_fmt)-1] = '\0';
    if (a->is_you())
        Sound::play_2d_sound(sound_str);
    else
    {
        Animations::create_health_change_indicator(this->a->get_bounding_box(),
                                                   position, -amount);
        Sound::play_3d_sound(sound_fmt, position);
    }
}

void AgentEvent::healed(int amount)
{
    GS_ASSERT(amount >= 0);
    if (a->is_you())
    {
        //Sound::play_2d_sound("restore_health");
        Chat::send_system_message("You healed.");
    }
    else
    {
        Animations::create_health_change_indicator(this->a->get_bounding_box(),
                                                   this->a->get_center(),
                                                   amount);
        //Vec3 p = this->a->get_position();
        //Sound::play_3d_sound("restore_health", p, vec3_init(0,0,0));
    }
}

void AgentEvent::died()
{
    if (!this->a->status.dead)
    {
        this->a->status.dead = true;
        if (a->is_you())
        {
            close_all_containers();
            //Sound::play_2d_sound("died");
        }
        //else
        //{
            //Vec3 p = this->a->get_position();
            //Sound::play_3d_sound("died", p, vec3_init(0,0,0));
        //}
        this->a->vox->set_vox_dat(&VoxDats::agent_dead);
        this->a->vox->reset_skeleton();
    }
    Toolbelt::agent_died(this->a->id);
}

void AgentEvent::born()
{
    if (!this->a->status.dead) return;
    //if (a->is_you())
        //Sound::play_2d_sound("respawned");
    //else
        //Sound::play_3d_sound("respawned", this->a->get_position(), vec3_init(0,0,0));
    this->a->status.dead = false;

    // reset skeleton
    Voxels::VoxDat* vd = this->a->get_vox_dat();
    this->a->vox->set_vox_dat(vd);
    this->a->vox->reset_skeleton();
}

void AgentEvent::life_changing(bool dead)
{
    if (dead) died();
    else born();
}

void AgentEvent::set_spawner(int pt)
{
    this->a->status.spawner = pt;
}

void AgentEvent::crouched()
{
    this->model_was_changed = true;
    this->a->vox->set_vox_dat(&VoxDats::agent_crouched);
    this->a->vox->reset_skeleton();
}

void AgentEvent::uncrouched()
{
    this->model_was_changed = true;
    this->a->vox->set_vox_dat(&VoxDats::agent);
    this->a->vox->reset_skeleton();
}

bool AgentEvent::model_changed()
{
    bool changed = this->model_was_changed;
    this->model_was_changed = false;
    return changed;
}

void AgentEvent::set_agent_vox_status(AgentVoxStatus status)
{
    if (this->vox_status != status) this->model_was_changed = true;
    this->vox_status = status;
}

void AgentEvent::reload_weapon(int type)
{
    //Vec3 p = this->a->get_position();
    //Sound::play_3d_sound("reload", p.x, p.y, p.z, 0,0,0);
    // play reload animation/sound for the weapon
}

void AgentEvent::update_mining_laser()
{
    if (!this->mining_laser_emitter.on) return;
    struct Vec3 p = this->a->arm_center();
    struct Vec3 v = this->a->forward_vector();
    this->mining_laser_emitter.length_position = p;
    this->mining_laser_emitter.length_direction = v;
    this->mining_laser_emitter.set_state(p, v);
    this->mining_laser_emitter.tick();
    this->mining_laser_emitter.prep_draw();
}

void AgentEvent::begin_mining_laser()
{
    ItemType laser_type = Toolbelt::get_agent_selected_item_type(this->a->id);
    GS_ASSERT(isValid(laser_type));
    GS_ASSERT(Item::get_item_group_for_type(laser_type) == IG_MINING_LASER);
    float range = Item::get_weapon_range(laser_type);
    this->mining_laser_emitter.set_base_length(range);
    this->mining_laser_emitter.set_laser_type(laser_type);
    this->mining_laser_emitter.turn_on();
}

void AgentEvent::end_mining_laser()
{
    this->mining_laser_emitter.turn_off();
}

void AgentEvent::spray_blood(const struct Vec3& f, int body_part)
{
    if (this->a->vox == NULL) return;
    Voxels::VoxelVolume* vv = this->a->vox->get_part(body_part);
    if (vv == NULL) return;
    Vec3 c = vv->get_center();
    Animations::blood_spray(c, f);
}

void AgentEvent::fired_weapon_at_object(int id, EntityType type, int part)
{
    Vec3 f = this->a->forward_vector();
    if (type == OBJECT_AGENT)
    {
        Agent* agent = Agents::get_agent((AgentID)id);
        if (agent != NULL)
            agent->event.spray_blood(f, part);
    }
    this->play_laser_rifle_fire_event(this->a->arm_center(), f);
}

void AgentEvent::fired_weapon_at_block(float x, float y, float z, CubeType cube, int side)
{
    IF_ASSERT(!is_boxed_point(x) || !is_boxed_point(y)) return;
    IF_ASSERT(side < 0 || side >= 6) return;

    Vec3 c = this->a->arm_center();
    Vec3 impact = vec3_init(x,y,z);
    impact = quadrant_translate_position(this->a->get_camera_position(), impact);
    if (vec3_equal(impact, c)) return;
    Vec3 f = vec3_sub(impact, c);
    f = vec3_normalize(f);

    this->play_laser_rifle_fire_event(c, f);
    Animations::block_damage(impact, f, cube, side);
    Animations::particle_explode(impact);
}

void AgentEvent::fired_weapon_at_nothing()
{
    this->play_laser_rifle_fire_event(this->a->arm_center(), this->a->forward_vector());
}

void AgentEvent::play_laser_rifle_fire_event(const struct Vec3& c, const struct Vec3& forward)
{
    // play laser anim out of arm
    const float hitscan_speed = 200.0f;
    Vec3 f = vec3_scalar_mult(forward, hitscan_speed);
    Animations::create_hitscan_effect(c, f);
    Sound::play_3d_sound("fire_laser", c);
}

void AgentEvent::threw_grenade()
{
    // play throw grenade animation
    // might need to sync grenades with this?
}

void AgentEvent::placed_block()
{
    // player agent block placement animation
}

void AgentEvent::hit_block()
{
    // play pick swing
    // play block damage animation
    //Vec3 p = this->a->get_camera_position();
    //Sound::play_3d_sound("pick_swung", p.x, p.y, p.z, 0,0,0);

    // TODO -- need collision point
    //Sound::play_3d_sound("block_took_damage", collision_point[0], collision_point[1], collision_point[2], 0,0,0);
}

void AgentEvent::melee_attack_object(int id, EntityType type, int part)
{
    // play pick swing animation
    // play blood animation
    // play swing sound
    // play object's hurt sound

    //Vec3 p = this->a->get_camera_position();
    //Sound::play_3d_sound("pick_swung", p.x,p.y,p.z, 0,0,0);
    //Sound::play_3d_sound("pick_hit_agent", p.x, p.y, p.z, 0,0,0);
}

void AgentEvent::melee_attack_nothing()
{
    // play pick swing animation
    //Vec3 p = this->a->get_camera_position();
    //Sound::play_3d_sound("pick_swung", p.x, p.y, p.z, 0,0,0);
}

void AgentEvent::fire_empty_weapon(int weapon_type)
{
    Vec3 p = this->a->get_camera_position();
    Sound::play_3d_sound("out_of_ammo", p.x, p.y, p.z, 0,0,0);
}

AgentEvent::~AgentEvent()
{
}

AgentEvent::AgentEvent(Agent* owner) :
    a(owner),
    vox_status(AGENT_VOX_IS_STANDING),
    model_was_changed(true),
    color_changed(false)
{
    this->bb.reset();
    this->bb.permanent = true;
    if (this->a->status.name != NULL)
        this->bb.set_text(this->a->status.name);
    this->bb.set_color(Color(200,4,3,255));
    this->bb.set_scale(AgentHudName::SIZE);
}

}   // Agents
