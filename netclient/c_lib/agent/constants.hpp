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


/* Main */
const int AGENT_MAX = PLAYERS_MAX;
#define AGENT_HEIGHT 2.8f
#define CAMERA_HEIGHT 2.5f
#define AGENT_HEIGHT_CROUCHED 1.8f
#define CAMERA_HEIGHT_CROUCHED 1.5f;
#define CAMERA_HEIGHT_DEAD 0.3f;
#define AGENT_SPEED 1.9f    // testing this, 2.8 seems too fast
//#define AGENT_SPEED 2.8f  // was default
//#define AGENT_SPEED 28.0f // uncomment for testing high speed
#define AGENT_SPEED_CROUCHED 0.8f
#define AGENT_BOX_RADIUS 0.38f
const int NO_AGENT_OWNER = 255; // works as long as < AGENT_MAX 

/* Status */
#define AGENT_HEALTH 100
const int RESPAWN_TICKS = 30 * 4; // 4 seconds
const int AGENT_BASE_PROXIMITY_EFFECT_RATE = 30 * 3; // 2 seconds
const unsigned int PLAYER_NAME_MAX_LENGTH = 24;
//const int SLIME_KILLS_PER_COIN = 10;
const int SLIME_KILLS_PER_COIN = 1;
const int COINS_PER_AGENT_KILL = 5;
const int AGENT_MAX_TURRETS = (MAX_TURRETS / AGENT_MAX) * 2;    // (256/64)*2 == 8
const int AGENT_MAX_SPAWNERS = SPAWNERS_PER_TEAM / 3;

/* Player */
#if DC_CLIENT
#define AGENT_STATE_HISTORY_SIZE 64
#define AGENT_INTERPOLATION_DECAY 0.8f
const int JETPACK_TICKS_MAX = 30 * 10 * 2;
const int JETPACK_DECAY_TICKS = 30 * 10;
#endif
