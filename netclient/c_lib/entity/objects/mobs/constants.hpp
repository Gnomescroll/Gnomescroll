#pragma once

namespace Objects
{

/* Robot Bomb */

/* Common/Defaults */

// health
const int MONSTER_BOMB_MAX_HEALTH = 75;

// vox
const bool MONSTER_BOMB_INIT_WITH_HITSCAN = true;
const bool MONSTER_BOMB_INIT_WITH_DRAW = true;

// dims
const float MONSTER_BOMB_HEIGHT = 1.0f;

// explosion
const float MONSTER_BOMB_EXPLOSION_RADIUS = 4.0f;
const int MONSTER_BOMB_EXPLOSION_DAMAGE = 35;
const float MONSTER_BOMB_EXPLOSION_PROXIMITY_RADIUS = 1.0f;

// targeting
const float MONSTER_BOMB_MOTION_PROXIMITY_RADIUS = 15.0f;

// motion
const float MONSTER_BOMB_SPEED = 0.25f;
const int MONSTER_BOMB_MOTION_MAX_Z_DOWN = 5;
const int MONSTER_BOMB_MOTION_MAX_Z_UP = 5;

// animations
const struct Color MONSTER_BOMB_ANIMATION_COLOR = { 31,223,223 };
const int MONSTER_BOMB_ANIMATION_COUNT = 50;
const int MONSTER_BOMB_ANIMATION_COUNT_MAX = 100;
const float MONSTER_BOMB_ANIMATION_SIZE = 0.7f;
const float MONSTER_BOMB_ANIMATION_FORCE = 20.0f;

// state broadcast rate
const int MONSTER_BOMB_BROADCAST_RATE = 30; // once every 30 ticks

// item drop
const char* MONSTER_BOMB_ITEM_DROP_NAME = "grenades";
// avg drop size; 5.5, 5.5 * 0.8 = 4.4 grenades per slime
const float MONSTER_BOMB_ITEM_DROP_PROBABILITY = 0.8;
const int MONSTER_BOMB_ITEM_DROP_MAX_AMOUNT = 10;    // TODO --  a good item drop table

/* MONSTER_BOX */

// health
const int MONSTER_BOX_MAX_HEALTH = 25;

//vox
const bool MONSTER_BOX_INIT_WITH_HITSCAN = true;
const bool MONSTER_BOX_INIT_WITH_DRAW = true;

//dims
const float MONSTER_BOX_HEIGHT = 3.0f;
const float MONSTER_BOX_CAMERA_HEIGHT = 2.5f;

// targeting
const float MONSTER_BOX_TARGET_ACQUISITION_PROBABILITY = 0.9f;
const int MONSTER_BOX_FIRE_RATE_LIMIT = 45;
const float MONSTER_BOX_USES_BIAS = false;
const float MONSTER_BOX_ACCURACY_BIAS = 0.0f;
const float MONSTER_BOX_FIRING_SIGHT_RANGE = 15.0f;
const bool MONSTER_BOX_ATTACK_ONLY_ENEMIES = false;
const bool MONSTER_BOX_ATTACK_AT_RANDOM = false;
const int MONSTER_BOX_TERRAIN_DAMAGE = 5;
const int MONSTER_BOX_AGENT_DAMAGE = 5;
const int MONSTER_BOX_AGENT_IMMUNITY_DURATION = 30 * 3;
//const int MONSTER_BOX_VOXEL_DAMAGE_RADIUS = 1;

// animations
const struct Color MONSTER_BOX_ANIMATION_COLOR = { 223,31,31 };
const int MONSTER_BOX_ANIMATION_COUNT = 5*5*5;
//const int MONSTER_BOX_ANIMATION_COUNT_MAX = 5*5*5;
const float MONSTER_BOX_ANIMATION_SIZE = 0.2f;
const float MONSTER_BOX_ANIMATION_FORCE = 20.0f;

// motion??
const float MONSTER_BOX_MOTION_SIGHT_RANGE = 50.0f;
const float MONSTER_BOX_SPEED = 0.2f;
const int MONSTER_BOX_WALK_RANGE = 30;
const int MONSTER_BOX_MOTION_MAX_Z_DOWN = 4;
const int MONSTER_BOX_MOTION_MAX_Z_UP = 3;

// state broadcast rate
const int MONSTER_BOX_BROADCAST_RATE = 30; // once every 30 ticks

/* Monster spawners */

// health
const int MONSTER_SPAWNER_MAX_HEALTH = 150;

//vox
const bool MONSTER_SPAWNER_INIT_WITH_HITSCAN = true;
const bool MONSTER_SPAWNER_INIT_WITH_DRAW = true;

// dim
const float MONSTER_SPAWNER_HEIGHT = 1.9f;

// spawner
const int MONSTER_SPAWNER_MAX_CHILDREN = 10;
const float MONSTER_SPAWNER_SPAWN_RADIUS = 2.5f;

// animations
const struct Color MONSTER_SPAWNER_ANIMATION_COLOR = { 223,31,31 };
const int MONSTER_SPAWNER_ANIMATION_COUNT = 5*5*5;
//const int MONSTER_SPAWNER_ANIMATION_COUNT_MAX = 5*5*5;
const float MONSTER_SPAWNER_ANIMATION_SIZE = 0.4f;

} // Objects
