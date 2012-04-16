#pragma once

namespace Animations 
{

extern class HitscanEffect_list* hitscan_effect_list;
extern class HitscanLaserEffect_list* hitscan_laser_effect_list;   
extern class MiningLaserEffect_list* mining_laser_effect_list;
extern class Insect_mob_list* insect_mob_list;


void init();
void teardown();

void animations_tick();

void draw_insect_mob();
void spawn_insect_mob(float x, float y, float z);

void draw_hitscan_effect();
void draw_hitscan_laser_effect();
void draw_mining_laser_effect();

void create_hitscan_effect(float x, float y, float z, float vx, float vy, float vz);
void create_mining_laser_particle();

}