#pragma once

namespace Animations
{

#if DC_CLIENT
extern class HitscanEffectList* hitscan_effect_list;
extern class RailTrailEffectList* railtrail_effect_list;
extern class MiningLaserEffectList* mining_laser_effect_list;
extern class InsectMobList* insect_mob_list;

void animations_tick();

void prep_insect_mob();
void draw_insect_mob();
void spawn_insect_mob(float x, float y, float z);

void draw_hitscan_effect();
void draw_railtrail_effect();
void draw_mining_laser_effect();

void create_hitscan_effect(struct Vec3 start, struct Vec3 velocity);
void create_railtrail_effect(const Vec3& start, const Vec3& end);
void create_mining_laser_particle(struct Vec3 position, struct Vec3 orientation, const float speed);

void mining_laser_beam(struct Vec3 position, struct Vec3 orientation, float length);

void play_animation(const char* name, struct Vec3 position);
void play_animation(AnimationType animation_type, struct Vec3 position);

void create_health_change_indicator(const BoundingBox& box, const Vec3& position, int amount);
#endif

#if DC_SERVER
void send_play_animation(const char* name, ClientID client_id, struct Vec3 position);
void broadcast_play_animation(const char* name, struct Vec3 position);
#endif

void init();
void teardown();

void init_packets();

}   // Animations
