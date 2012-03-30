#pragma once

/*
    #defines, constants for items
*/

#include <c_lib/game/game.hpp>

// Spawners
const int SPAWNER_RADIUS = 5;
const int SPAWNERS_PER_TEAM = 9;
const int MAX_SPAWNERS = SPAWNERS_PER_TEAM * N_TEAMS;
const int SPAWNER_HEALTH = 300;
const float SPAWNER_HEIGHT = 1.9f;
const int BASE_SPAWN_ID = 255;

// Team indexing
const unsigned int TEAM_INDEX_NONE = 255;
const unsigned int TEAM_INDEX_MAX = (TEAM_INDEX_NONE+1) / N_TEAMS;

// Turrets
const int MAX_TURRETS = 256;
const int TURRET_HEALTH = 125;
const float TURRET_HEIGHT = 1.9f;
const float TURRET_CAMERA_HEIGHT = 1.6f;
const float TURRET_SIGHT_RANGE = 128.0f - 16.0f;    // same as agent
const int TURRET_FIRE_LIMIT = 23*2; // ticks
const int TURRET_BLOCK_DAMAGE = 8;
const int TURRET_AGENT_DAMAGE = 10;
const float TURRET_TARGET_LOCK_CHANCE = 0.80f;  // likelihood of acquiring a target in view
const float TURRET_EXPLOSION_RADIUS = 2.0f;
const int TURRET_EXPLOSION_DAMAGE = 40;
const int TURRET_LASER_VOXEL_DAMAGE_RADIUS = 2;
const float TURRET_LASER_BIAS = 3.0f;
