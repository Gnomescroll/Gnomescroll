#pragma once

#include <common/color.hpp>

namespace Entities
{

/* Base */

// dimensions
const float BASE_HEIGHT = 1.0f;

// voxel model
const bool BASE_INIT_WITH_HITSCAN = false;
const bool BASE_INIT_WITH_DRAW = true;
const bool BASE_SHOULD_HITSCAN = false;

// spawner
const int BASE_SPAWN_RADIUS = 4;

/* Agent Spawner */

// dimensions
const float AGENT_SPAWNER_HEIGHT = 1.9f;

// voxel model
const bool AGENT_SPAWNER_INIT_WITH_HITSCAN = true;
const bool AGENT_SPAWNER_INIT_WITH_DRAW = true;
const struct Color ACTIVATED_SPAWNER_COLOR = color_init(240, 20, 20);
const struct Color DEACTIVATED_SPAWNER_COLOR = color_init(20, 20, 20);

// spawner
const int AGENT_SPAWNER_SPAWN_RADIUS = 0;
const int AGENT_SPAWNER_INITIAL_CHILDREN_SIZE = 4;
const size_t MAX_USERS_PER_AGENT_SPAWNER = 4096;
const size_t AGENT_SPAWNER_INITIAL_USERS_SIZE = 8;

// health
const int AGENT_SPAWNER_MAX_HEALTH = 200;

// animation
const struct Color AGENT_SPAWNER_ANIMATION_COLOR = color_init(127,31,223);
const int AGENT_SPAWNER_ANIMATION_COUNT = 35;
const int AGENT_SPAWNER_ANIMATION_COUNT_MAX = 50;
const float AGENT_SPAWNER_ANIMATION_SIZE = 0.22f;
const float AGENT_SPAWNER_ANIMATION_FORCE = 5.0f;

/* Energy Core */

// dimensions
const float ENERGY_CORE_HEIGHT = 1.0f;

// voxel model
const bool ENERGY_CORE_INIT_WITH_HITSCAN = true;
const bool ENERGY_CORE_INIT_WITH_DRAW = true;

// healing
const float ENERGY_CORE_HEALING_RADIUS = 1.0f;

// health
const int ENERGY_CORE_MAX_HEALTH = 200;

// animation
const struct Color ENERGY_CORE_ANIMATION_COLOR = color_init(59,99,17);
const int ENERGY_CORE_ANIMATION_COUNT = 35;
const int ENERGY_CORE_ANIMATION_COUNT_MAX = 50;
const float ENERGY_CORE_ANIMATION_SIZE = 0.22f;
const float ENERGY_CORE_ANIMATION_FORCE = 5.0f;

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

// explosion
const float TURRET_EXPLOSION_RADIUS = 2.0f;
const int TURRET_EXPLOSION_DAMAGE = 40;
const bool TURRET_EXPLOSION_HARMS_OWNER = false;

// animation
const int TURRET_ANIMATION_COUNT = 35;
const int TURRET_ANIMATION_COUNT_MAX = 50;
const float TURRET_ANIMATION_SIZE = 0.1f;
const float TURRET_ANIMATION_FORCE = 5.0f;

} // Entities
