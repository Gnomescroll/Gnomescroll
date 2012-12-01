#pragma once

namespace Animations 
{

#if DC_CLIENT
extern class HitscanEffectList* hitscan_effect_list;
extern class MiningLaserEffectList* mining_laser_effect_list;
extern class InsectMobList* insect_mob_list;

void animations_tick();

void prep_insect_mob();
void draw_insect_mob();
void spawn_insect_mob(float x, float y, float z);

void draw_hitscan_effect();
void draw_mining_laser_effect();

void create_hitscan_effect(float x, float y, float z, float vx, float vy, float vz);
void create_mining_laser_particle(struct Vec3 position, struct Vec3 orientation, const float speed);

void mining_laser_beam(struct Vec3 position, struct Vec3 orientation, float length);

void play_animation(const char* name, struct Vec3 position);
void play_animation(int animation_id, struct Vec3 position);
#endif

#if DC_SERVER
void send_play_animation(const char* name, ClientID client_id, struct Vec3 position);
void broadcast_play_animation(const char* name, struct Vec3 position);
#endif

void init();
void teardown();

void init_packets();

}   // Animations
