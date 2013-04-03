#include "_interface.hpp"

#if DC_CLIENT
# include <animations/insect_mob.hpp>
# include <animations/hitscan.hpp>
# include <animations/mining_laser.hpp>
# include <animations/weapon.hpp>
# include <animations/voxel_particle.hpp>
#endif

#include <animations/packets.hpp>
#include <animations/config/_interface.hpp>

namespace Animations
{

#if DC_CLIENT
class InsectMobList* insect_mob_list = NULL;
class HitscanEffectList* hitscan_effect_list = NULL;
class RailRayEffectList* rail_ray_effect_list = NULL;
class MiningLaserEffectList* mining_laser_effect_list = NULL;
#endif

#if DC_CLIENT
void animations_tick()
{
    hitscan_effect_list->tick();
    rail_ray_effect_list->tick();
    mining_laser_effect_list->tick();

    //insect_mob_list->tick();

    tick_equipped_item_animation();
}

void prep_insect_mob()
{
    insect_mob_list->prep();
}

void draw_insect_mob()
{
    insect_mob_list->draw();
}

void draw_hitscan_effect()
{
    hitscan_effect_list->draw();
}

void draw_rail_ray_effect()
{
    rail_ray_effect_list->draw();
}

void mining_laser_effect_tick()
{
    mining_laser_effect_list->tick();
}

void draw_mining_laser_effect()
{
    mining_laser_effect_list->prep();
    mining_laser_effect_list->draw();
}

void play_animation(AnimationType animation_type, struct Vec3 position)
{
    class AnimationProperty* data = get_animation_data(animation_type);
    IF_ASSERT(data == NULL) return;
    IF_ASSERT(data->callback == NULL || data->metadata == NULL) return;

    switch (data->metadata_type)
    {
        case AnimDataNone:
            break;

        case AnimDataSimple:
            ((class AnimationStateMetadata*)data->metadata)->position = position;
            ((class AnimationStateMetadata*)data->metadata)->velocity = vec3_scalar_mult(vec3_init(1,1,1), data->momentum);
            break;

        default:
            GS_ASSERT(false);
            break;
    }

    data->callback(animation_type, data->metadata);
}

void play_animation(const char* name, struct Vec3 position)
{
    AnimationType animation_type = get_animation_type(name);
    play_animation(animation_type, position);
}

float x13 = 0.0f;
float y13 = 0.0f;
float z13 = 0.0f;

void spawn_insect_mob(float x, float y, float z)
{
    x13 = x;
    y13 = y;
    z13 = z;

    //InsectMob* im = insect_mob_list->create();
    //if (im == NULL) return;

    //im->init(x,y,z+2.5f);

    ////printf("insect at: %f %f %f \n", x,y,z);
}

void create_mining_laser_particle(struct Vec3 position, struct Vec3 orientation, const float speed, const float length)
{
    MiningLaser* effect = mining_laser_effect_list->create();
    if (effect == NULL) return;
    effect->set_state(position.x, position.y, position.z, orientation.x, orientation.y, orientation.z, speed, length);
}

void create_hitscan_effect(struct Vec3 pos, struct Vec3 fwd)
{
    HitscanEffect* he = hitscan_effect_list->create();
    if (he == NULL) return;

    //start = translate_position(start);  // ASK ABOUT: doesn't this just magnify(hah!  magnitude!), or make it pos * 2?  WHY SHOULD IT CHANGE FROM WHATS GIVEN?!
    he->set_state(pos, fwd);
}

void create_rail_ray_effect(struct Vec3 start, struct Vec3 end)
{
    RailRayEffect* rre = rail_ray_effect_list->create();
    if (rre == NULL) return;

    //start = translate_position(start);  // ASK ABOUT: doesn't this just magnify(hah!  magnitude!), or make it pos * 2?  WHY SHOULD IT CHANGE FROM WHATS GIVEN?!
    rre->set_state(start, end);
}

void mining_laser_beam(struct Vec3 position, struct Vec3 orientation, float length)
{
    if (Options::animation_level <= 0) return;

    const float speed = 6.0f;
    int n = Options::animation_level * 2;

    const float spread = 0.05f;
    Vec3 step = vec3_scalar_mult(orientation, spread);
    for (int i=0; i<n; i++)
    {
        Animations::create_mining_laser_particle(translate_position(position), orientation, speed, length);
        // move the particle a little bit ahead of the last
        // to get an even distribution
        position = vec3_add(position, step);
    }
}
#endif

#if DC_SERVER
void send_play_animation(const char* name, ClientID client_id, struct Vec3 position)
{
    GS_ASSERT(is_boxed_position(position));
    AnimationType animation_type = get_animation_type(name);
    IF_ASSERT(!isValid(animation_type)) return;
    play_animation_StoC msg;
    msg.animation_type = animation_type;
    msg.position = position;
    msg.sendToClient(client_id);
}

void broadcast_play_animation(const char* name, struct Vec3 position)
{
    GS_ASSERT(is_boxed_position(position));
    AnimationType animation_type = get_animation_type(name);
    IF_ASSERT(!isValid(animation_type)) return;
    play_animation_StoC msg;
    msg.animation_type = animation_type;
    msg.position = position;
    msg.broadcast();
}
#endif

// Common

void init()
{
    #if DC_CLIENT
    GS_ASSERT(hitscan_effect_list == NULL);
    GS_ASSERT(rail_ray_effect_list == NULL);
    GS_ASSERT(mining_laser_effect_list == NULL);

    hitscan_effect_list = new HitscanEffectList;
    rail_ray_effect_list = new RailRayEffectList;
    mining_laser_effect_list = new MiningLaserEffectList;

    //insect_mob_list = new InsectMobList(INSECT_MOB_MAX);

    Animations::init_hitscan();
    Animations::init_mining_laser();
    Animations::init_sprite_voxelizer();

    //aAnimations::init_insect_mob();

    init_weapon_sprite();
    init_voxel_particle();
    init_block_damage();
    #endif
}

void init_packets()
{
    play_animation_StoC::register_client_packet();
}

void teardown()
{
    #if DC_CLIENT
    delete hitscan_effect_list;
    delete rail_ray_effect_list;
    delete mining_laser_effect_list;
    Animations::teardown_hitscan();
    Animations::teardown_sprite_voxelizer();
    teardown_voxel_particle();
    #endif
}

}   // Animations
