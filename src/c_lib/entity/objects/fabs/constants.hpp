#pragma once

namespace Objects
{

/* Base */

// dimensions
const float BASE_HEIGHT = 1.0f;

// voxel model
const bool BASE_INIT_WITH_HITSCAN = false;
const bool BASE_INIT_WITH_DRAW = true;
const bool BASE_SHOULD_HITSCAN = false;

// spawner
const float BASE_SPAWN_RADIUS = 10.0f;

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

// animation
const int AGENT_SPAWNER_ANIMATION_COUNT = 35;
const int AGENT_SPAWNER_ANIMATION_COUNT_MAX = 50;
const float AGENT_SPAWNER_ANIMATION_SIZE = 0.1f;
const float AGENT_SPAWNER_ANIMATION_FORCE = 5.0f;

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
const bool TURRET_USES_BIAS = true;
const float TURRET_ACCURACY_BIAS = 1.0f;
const float TURRET_SIGHT_RANGE = 128.0f - 16.0f;
const bool TURRET_ATTACK_AT_RANDOM = true;
const int TURRET_TERRAIN_DAMAGE = 8;
const int TURRET_AGENT_DAMAGE_MIN = 8;
const int TURRET_AGENT_DAMAGE_MAX = 12;
const int TURRET_AGENT_IMMUNITY_DURATION = 30 * 5;
//const int TURRET_VOXEL_DAMAGE_RADIUS = 1;

// explosion
const float TURRET_EXPLOSION_RADIUS = 2.0f;
const int TURRET_EXPLOSION_DAMAGE = 40;
const bool TURRET_EXPLOSION_HARMS_OWNER = false;

// animation
const int TURRET_ANIMATION_COUNT = 35;
const int TURRET_ANIMATION_COUNT_MAX = 50;
const float TURRET_ANIMATION_SIZE = 0.1f;
const float TURRET_ANIMATION_FORCE = 5.0f;

} // Objects
