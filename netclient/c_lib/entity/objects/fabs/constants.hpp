#pragma once

namespace Objects
{
    
/* Agent Spawner */

// dimensions
const float AGENT_SPAWNER_HEIGHT = 1.9f;

// voxel model
const bool AGENT_SPAWNER_INIT_WITH_HITSCAN = true;
const bool AGENT_SPAWNER_INIT_WITH_DRAW = true;

// spawner
const float AGENT_SPAWNER_SPAWN_RADIUS = 5.0f;

// health
const int AGENT_SPAWNER_MAX_HEALTH = 200;

/* Turret */

// dimensions
const float TURRET_HEIGHT = 1.9f;
const float TURRET_CAMERA_HEIGHT = 1.6f;

// voxel model
const bool TURRET_INIT_WITH_HITSCAN = true;
const bool TURRET_INIT_WITH_DRAW = true;

// health
const int TURRET_MAX_HEALTH = 125;

// targeting
const float TURRET_TARGET_ACQUISITION_PROBABILITY = 0.85f;
const int TURRET_FIRE_RATE_LIMIT = 45; // ticks
const float TURRET_ACCURACY_BIAS = 1.0f;
const float TURRET_SIGHT_RANGE = 128.0f - 16.0f;
const bool TURRET_ATTACKS_ENEMIES = true;
const bool TURRET_ATTACK_AT_RANDOM = true;
const int TURRET_BLOCK_DAMAGE = 8;
const int TURRET_AGENT_DAMAGE = 10;
const int TURRET_LASER_VOXEL_DAMAGE_RADIUS = 2;
const int TURRET_AGENT_IMMUNITY_DURATION = 30 * 5;
const int TURRET_VOXEL_DAMAGE_RADIUS = 1;

// explosion
const float TURRET_EXPLOSION_RADIUS = 2.0f;
const int TURRET_EXPLOSION_DAMAGE = 40;
const bool TURRET_EXPLOSION_HARMS_OWNER = false;

} // Objects
