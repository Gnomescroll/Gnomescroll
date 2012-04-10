#pragma once

namespace Monsters
{

/* Slime */
const int SLIME_MAX = 256;
const int SLIME_HEALTH = 75;
enum SLIME_BODY_PARTS {
    SLIME_PART_BODY
};
const float SLIME_CAMERA_HEIGHT = 0.7f;
const float SLIME_HEIGHT = 1.0f;
const float SLIME_EXPLOSION_RADIUS = 4.0f;
const int SLIME_EXPLOSION_DAMAGE = 35;
const float SLIME_EXPLOSION_PROXIMITY_RADIUS = 1.0f;
const float SLIME_MOTION_PROXIMITY_RADIUS = 15.0f;
const float SLIME_SPEED = 0.25f;

/* Box */
const int BOX_MAX = 256;
const int BOX_HEALTH = 25;
enum BOX_BODY_PARTS {
    BOX_PART_BODY
};
const float BOX_CAMERA_HEIGHT = 2.5f;
const float BOX_HEIGHT = 3.0f;
const float BOX_MOTION_PROXIMITY_RADIUS = 50.0f;
//const float BOX_FIRING_PROXIMITY_RADIUS = 100.0f;
const float BOX_SPEED = 1.0f;

//const int BOX_FIRE_RATE = 30;
const int BOX_FIRE_RATE = 1;
const float BOX_ACCURACY_BIAS = 0.0f;
const float BOX_SIGHT_RANGE = 15.0f;
const bool BOX_ATTACK_ONLY_ENEMIES = false;    // TODO change attack_enemies to attack_only_enemies
const bool BOX_ATTACK_RANDOM = false;

const int AGENT_BOX_PROTECTION_DURATION = 0;
const int BOX_AGENT_DAMAGE = 0;
const int BOX_TERRAIN_DAMAGE = 5;
const int BOX_VOXEL_DAMAGE_RADIUS = 0;
const float BOX_TARGET_LOCK_CHANGE = 1.0f;


} // Monsters
