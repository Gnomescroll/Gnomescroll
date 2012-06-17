#include "_interface.hpp"

#include <animations/insect_mob.hpp>

#include <animations/hitscan.hpp>
#include <animations/hitscan_laser.hpp>
#include <animations/mining_laser.hpp>

namespace Animations 
{

class Insect_mob_list* insect_mob_list = NULL;

class HitscanEffect_list* hitscan_effect_list = NULL;
class HitscanLaserEffect_list* hitscan_laser_effect_list = NULL;
class MiningLaserEffect_list* mining_laser_effect_list = NULL;

void init()
{

    hitscan_effect_list = new HitscanEffect_list;
    hitscan_laser_effect_list = new HitscanLaserEffect_list;
    mining_laser_effect_list = new MiningLaserEffect_list;

    insect_mob_list = new Insect_mob_list;

    Animations::init_hitscan();
    Animations::init_hitscan_laser();
    Animations::init_mining_laser();

    Animations::init_insect_mob();


}

void teardown()
{
    delete insect_mob_list;

    delete hitscan_effect_list;
    delete hitscan_laser_effect_list;
    delete mining_laser_effect_list;

    Animations::teardown_hitscan();
    Animations::teardown_hitscan_laser();

    Animations::teardown_insect_mob();

}

void animations_tick()
{
    ClientState::ctf->animate_flags();

    hitscan_effect_list->tick();
    hitscan_laser_effect_list->tick();
    mining_laser_effect_list->tick();
    
    insect_mob_list->tick();
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

void draw_hitscan_laser_effect()
{
    hitscan_laser_effect_list->draw();
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


float x13 = 0.0;
float y13 = 0.0;
float z13 = 0.0;

void spawn_insect_mob(float x, float y, float z)
{
    x13 = x;
    y13 = y;
    z13 = z;

    return;
    InsectMob* im = insect_mob_list->create();
    if(im == NULL) return;

    im->init(x,y,z+2.5);

    //printf("insect at: %f %f %f \n", x,y,z);
}


void create_mining_laser_particle(Vec3 position, Vec3 orientation, const float speed, const float length)
{
    MiningLaser* effect = mining_laser_effect_list->create();
    if (effect == NULL) return;
    effect->init(position.x, position.y, position.z, orientation.x, orientation.y, orientation.z, speed, length);
}

void create_hitscan_effect(float x, float y, float z, float vx, float vy, float vz)
{
    HitscanEffect* he = hitscan_effect_list->create();
    if (he == NULL) return;
    he->init();
    he->set_state(x,y,z,vx,vy,vz);
}

void mining_laser_beam(Vec3 position, Vec3 orientation, float length)
{
    const float speed = 6.0f;
    const int n = 2;
    const float spread = 0.05;
    Vec3 step = vec3_scalar_mult(orientation, spread);
    for (int i=0; i<n; i++)
    {
        Animations::create_mining_laser_particle(position, orientation, speed, length);
        // move the particle a little bit ahead of the last
        // to get an even distribution
        position = vec3_add(position, step);
    }
}

}   // Animations
