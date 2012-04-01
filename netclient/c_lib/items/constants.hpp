#pragma once

/*
    #defines, constants for items
*/

#include <c_lib/game/game.hpp>

// Spawners
const int SPAWNER_SPAWN_RADIUS = 5;
const int SPAWNERS_PER_TEAM = 9;
const int SPAWNER_MAX = SPAWNERS_PER_TEAM * N_TEAMS;
const int SPAWNER_HEALTH = 300;
const float SPAWNER_HEIGHT = 1.9f;
const int BASE_SPAWN_ID = 255;

// Team indexing
const unsigned int TEAM_INDEX_NONE = 255;
const unsigned int TEAM_INDEX_MAX = (TEAM_INDEX_NONE+1) / N_TEAMS;

// Turrets
const int TURRET_MAX = 256;
const int TURRET_HEALTH = 125;
const float TURRET_HEIGHT = 1.9f;
const float TURRET_CAMERA_HEIGHT = 1.6f;
const float TURRET_SIGHT_RANGE = 128.0f - 16.0f;
const int TURRET_FIRE_LIMIT = 45; // ticks
const int TURRET_BLOCK_DAMAGE = 8;
const int TURRET_AGENT_DAMAGE = 10;
const float TURRET_TARGET_LOCK_CHANCE = 0.85f;  // likelihood of acquiring a target in view
const float TURRET_EXPLOSION_RADIUS = 2.0f;
const int TURRET_EXPLOSION_DAMAGE = 40;
const int TURRET_LASER_VOXEL_DAMAGE_RADIUS = 2;
const float TURRET_LASER_BIAS = 1.0f;

//refills
namespace ItemDrops
{

/* Grenade refill */

const int GRENADE_REFILL_MAX = 256;
const int GRENADE_REFILL_TEXTURE_ID = 240;
const float GRENADE_REFILL_TEXTURE_SCALE = 0.5f;
const float GRENADE_REFILL_MASS = 1.0f;
const int GRENADE_REFILL_TTL = 30 * 12;   // 12 seconds
const float GRENADE_REFILL_DAMP = 0.4f;

/* Laser refill */

const int LASER_REFILL_MAX = 256;
const int LASER_REFILL_TEXTURE_ID = 242;
const float LASER_REFILL_TEXTURE_SCALE = 0.5f;
const float LASER_REFILL_MASS = 1.0f;
const int LASER_REFILL_TTL = 30 * 12;   // 12 seconds
const float LASER_REFILL_DAMP = 0.4f;

} // ItemDrops
