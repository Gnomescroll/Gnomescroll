#pragma once

/*
    Agent #defines, constants
*/


typedef enum CS_KEYS
{
    CS_FORWARD = 1,
    CS_BACKWARD = 2,
    CS_LEFT = 4,
    CS_RIGHT = 8,
    CS_JETPACK = 16,
    CS_JUMP = 32,
    CS_CROUCH = 64,
    CS_BOOST = 128,
    CS_MISC1 = 256,
    CS_MISC2 = 512,
    CS_MISC3 = 1024
} CS_KEYS;

typedef enum
{
    DEATH_NORMAL = 0,
    DEATH_HEADSHOT,
    DEATH_GRENADE,
    DEATH_FALL,
    DEATH_BELOW_MAP,
    DEATH_TURRET,
    DEATH_SLIME
} AgentDeathMethod;

/* Main */
const int AGENT_MAX = PLAYERS_MAX;
//const float AGENT_HEIGHT = 2.8f;
//const float CAMERA_HEIGHT = 2.5f;
const float AGENT_HEIGHT = 1.8f;
const float CAMERA_HEIGHT = 1.5f;
const float AGENT_HEIGHT_CROUCHED = 1.8f;
const float CAMERA_HEIGHT_CROUCHED = 1.5f;
const float CAMERA_HEIGHT_DEAD = 0.3f;
const float AGENT_SPEED = 0.9f;
//const float AGENT_SPEED = 28.0f; // uncomment for testing high speed
const float AGENT_SPEED_CROUCHED = 0.9f;
const float AGENT_JUMP_POWER = 0.25f;

const float AGENT_BOX_RADIUS = 0.38f;

const int NO_AGENT = 255; // works as long as < AGENT_MAX 

/* Status */
const int AGENT_HEALTH = 100;
const int RESPAWN_TICKS = 30 * 4; // 4 seconds
const int AGENT_BASE_PROXIMITY_EFFECT_RATE = 30 * 3; // 2 seconds
const unsigned int PLAYER_NAME_MAX_LENGTH = 24;
const int AGENT_MAX_TURRETS = 10;    // (256/64)*2 == 8
const int AGENT_MAX_SPAWNERS = 3;

/* Player */
#if DC_CLIENT
const int AGENT_STATE_HISTORY_SIZE = 64;
const float AGENT_INTERPOLATION_DECAY  = 0.8f;
const int JETPACK_TICKS_MAX = 30 * 10 * 2;
const int JETPACK_DECAY_TICKS = 30 * 10;
#endif

const float MINING_LASER_HITSCAN_RANGE = 4.0f;
