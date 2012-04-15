#include "_interface.hpp"

#include <c_lib/animations/insect_mob.hpp>
#include <c_lib/animations/hitscan.hpp>
#include <c_lib/animations/hitscan_laser.hpp>

namespace Animations 
{

class Insect_mob_list* insect_mob_list = NULL;

class HitscanEffect_list* hitscan_effect_list = NULL;
class HitscanLaserEffect_list* hitscan_laser_effect_list = NULL;

void init()
{

    hitscan_effect_list = new HitscanEffect_list;
    hitscan_laser_effect_list = new HitscanLaserEffect_list;

	insect_mob_list = new Insect_mob_list;

    Animations::init_hitscan();
    Animations::init_hitscan_laser();
    
    Animations::init_insect_mob();


}

void teardown()
{
	delete insect_mob_list;

    delete hitscan_effect_list;
    delete hitscan_laser_effect_list;

    Animations::teardown_hitscan();
    Animations::teardown_hitscan_laser();

}

void animations_tick()
{
    ClientState::ctf->animate_flags();
    hitscan_effect_list->tick();
    hitscan_laser_effect_list->tick();

    insect_mob_list->tick();
}

/*
void animations_draw()
{
    Animations::hitscan_effect_list->draw();
    Animations::hitscan_laser_effect_list->draw();
}
*/

void draw_insect_mob()
{
	//!! only prep if tick has been called!!!
	insect_mob_list->draw();
	insect_mob_list->prep();
}

void spawn_insect_mob(float x, float y, float z)
{
    InsectMob* im = insect_mob_list->create();
    im->init(x,y,z+2.5);

    //printf("insect at: %f %f %f \n", x,y,z);
}

void create_hitscan_effect(float x, float y, float z, float vx, float vy, float vz)
{
	hitscan_effect_list->create(x,y,z,vx,vy,vz);
}

}