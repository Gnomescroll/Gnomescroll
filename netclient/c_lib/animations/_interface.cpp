#include "_interface.hpp"

#include <c_lib/animations/insect_mob.hpp>

#include <c_lib/animations/hitscan.hpp>
#include <c_lib/animations/hitscan_laser.hpp>
#include <c_lib/animations/mining_laser.hpp>

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

    insect_mob_list->tick();
}



void draw_insect_mob()
{
	//!! only prep if tick has been called!!!
	insect_mob_list->draw();
	insect_mob_list->prep();
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


void create_mining_laser_particle()
{
    //printf("create \n");
    //float hv = 6.0;
    //float vv =  -1.0;

    MiningLaser* ml = mining_laser_effect_list->create();
    if(ml == NULL)
    {
        printf("particle limit reached \n");
        return;
    }

/*
#if 1
    float hv = 1.0;
    float vv =  -2.0;
    ml->init(x13,y13,z13+5.0, hv*randf() - hv/2, hv* randf() - hv/2, (vv/2)* randf() + vv/2  , x13, y13);
#else
    float hv = 6.0;
    float vv =  -1.0;
    const float offset = 0.3;
    ml->init(x13,y13+offset,z13+5.0, hv*randf() - hv/2, 0.0, (vv/2)* randf() + vv/2  , x13, y13);
#endif
*/

    float hv = 0.0;

    float hvv = 0.1;
    float vv =  -4.0;

    float ratio = 0.8;
    //ml->init(x13,y13,z13+5.0, hv*randf() - hv/2, hv* randf() - hv/2, (vv/2)* randf() + vv/2  , x13, y13);

ml->init(x13+ hvv*randf(),y13+ hvv*randf(),z13+5.0, hv*randf() - hv/2, hv* randf() - hv/2, vv*ratio*randf() + vv* (1.0-ratio) );

}

void create_hitscan_effect(float x, float y, float z, float vx, float vy, float vz)
{
	hitscan_effect_list->create(x,y,z,vx,vy,vz);
}

}