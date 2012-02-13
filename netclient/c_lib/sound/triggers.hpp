#pragma once

namespace Sound
{

void fire_laser();
void fire_laser_3d(float x, float y, float z, float vx, float vy, float vz);
void out_of_ammo();
void reload();
void laser_hit_block();
void laser_hit_agent();
void pick_hit_block();
void pick_hit_agent();
void pick_swung();
void block_destroyed(float x, float y, float z, float vx, float vy, float vz);
void block_took_damage();
void flag_picked_up();
void flag_returned_to_base();
void spawner_placed();
void respawned();
void died();
void walking();
void running();
void jetpack_on();
void soft_landing();
void medium_landing();
void hard_landing();
void victory();
void restock_at_base();
void place_block();
void grenade_explode(float x, float y, float z, float vx, float vy, float vz);
void grenade_bounce();

}
