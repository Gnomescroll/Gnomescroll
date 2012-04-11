#include "_interface.hpp"

namespace Animations 
{

class Insect_mob_list* insect_mob_list = NULL;


void init()
{
	insect_mob_list = new Insect_mob_list;

    Animations::init_hitscan();
    Animations::init_hitscan_laser();
    
    //Animations::init_insect_mob();


}

void teardown()
{
	delete insect_mob_list;

    Animations::teardown_hitscan();
    Animations::teardown_hitscan_laser();

}

void spawn_insect_mob(float x, float y, float z)
{


}

}