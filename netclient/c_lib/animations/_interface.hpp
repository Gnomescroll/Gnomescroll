#pragma once

namespace Animations 
{

extern class  Animations::HitscanEffect_list* hitscan_effect_list;
extern class  Animations::HitscanLaserEffect_list* hitscan_laser_effect_list;   

extern class Insect_mob_list* insect_mob_list;


void init();
void teardown();

void animations_tick();
void draw_insect_mob();

void spawn_insect_mob(float x, float y, float z);

}