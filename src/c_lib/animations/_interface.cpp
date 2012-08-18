#include "_interface.hpp"

#if DC_CLIENT
#include <animations/insect_mob.hpp>
#include <animations/hitscan.hpp>
#include <animations/mining_laser.hpp>
#include <animations/weapon.hpp>
#include <animations/voxel_particle.hpp>
#endif

#include <animations/packets.hpp>
#include <animations/config/_interface.hpp>

namespace Animations 
{

#if DC_CLIENT
class Insect_mob_list* insect_mob_list = NULL;
class HitscanEffect_list* hitscan_effect_list = NULL;
class MiningLaserEffect_list* mining_laser_effect_list = NULL;
#endif

#if DC_CLIENT
void animations_tick()
{
    hitscan_effect_list->tick();
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

void mining_laser_effect_tick()
{
    mining_laser_effect_list->tick();
}

void draw_mining_laser_effect()
{
    mining_laser_effect_list->prep();
    mining_laser_effect_list->draw();
}


float x13 = 0.0f;
float y13 = 0.0f;
float z13 = 0.0f;

void spawn_insect_mob(float x, float y, float z)
{
    x13 = x;
    y13 = y;
    z13 = z;

    return;
    InsectMob* im = insect_mob_list->create();
    if(im == NULL) return;

    im->init(x,y,z+2.5f);

    //printf("insect at: %f %f %f \n", x,y,z);
}


void create_mining_laser_particle(Vec3 position, Vec3 orientation, const float speed, const float length)
{
    MiningLaser* effect = mining_laser_effect_list->create();
    if (effect == NULL) return;
    effect->set_state(position.x, position.y, position.z, orientation.x, orientation.y, orientation.z, speed, length);
}

void create_hitscan_effect(float x, float y, float z, float vx, float vy, float vz)
{
    HitscanEffect* he = hitscan_effect_list->create();
    if (he == NULL) return;
    x = translate_point(x);
    y = translate_point(y);
    he->set_state(x,y,z,vx,vy,vz);
}

void mining_laser_beam(Vec3 position, Vec3 orientation, float length)
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
void send_play_animation(const char* name, int client_id, struct Vec3 position)
{
    ASSERT_BOXED_POSITION(position);
    int animation_id = get_animation_id(name);
    GS_ASSERT(animation_id >= 0);
    if (animation_id < 0) return;
    play_animation_StoC msg;
    msg.animation_id = animation_id;
    msg.position = position;
    msg.sendToClient(client_id);
}

void broadcast_play_animation(const char* name, struct Vec3 position)
{
    ASSERT_BOXED_POSITION(position);
    int animation_id = get_animation_id(name);
    GS_ASSERT(animation_id >= 0);
    if (animation_id < 0) return;
    play_animation_StoC msg;
    msg.animation_id = animation_id;
    msg.position = position;
    msg.broadcast();
}
#endif

// Common

void init()
{
    #if DC_CLIENT
    hitscan_effect_list = new HitscanEffect_list;
    mining_laser_effect_list = new MiningLaserEffect_list;

    //insect_mob_list = new Insect_mob_list;

    Animations::init_hitscan();
    Animations::init_mining_laser();

    //aAnimations::init_insect_mob();

    init_weapon_sprite();
    init_voxel_particle();
    init_block_damage();
    #endif

    init_config();
    play_animation_StoC::register_client_packet();
}

void teardown()
{
    #if DC_CLIENT
    //delete insect_mob_list;

    delete hitscan_effect_list;
    delete mining_laser_effect_list;

    Animations::teardown_hitscan();

    //Animations::teardown_insect_mob();

    teardown_voxel_particle();
    #endif

    teardown_config();
}

}   // Animations
