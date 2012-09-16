#pragma once

#include <common/color.hpp>

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
    DEATH_SLIME,
    DEATH_KILLME
} AgentDeathMethod;

enum AGENT_BODY_PARTS
{
    AGENT_PART_TORSO,
    AGENT_PART_HEAD,
    AGENT_PART_LARM,
    AGENT_PART_RARM,
    AGENT_PART_LLEG,
    AGENT_PART_RLEG
};

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
const int AGENT_HEALTH = 99;
const int RESPAWN_TICKS = 30 * 4; // 4 seconds
const int AGENT_MAX_TURRETS = 10;    // (256/64)*2 == 8
const int AGENT_MAX_SPAWNERS = 3;

// DONT CHANGE THESE. WILL BREAK STUFF AND YOU WONT KNOW UNTIL ITS TOO LATE
const unsigned int PLAYER_NAME_MIN_LENGTH = 3;
const unsigned int PLAYER_NAME_MAX_LENGTH = 15;


/* Player */
#if DC_CLIENT
const int AGENT_STATE_HISTORY_SIZE = 64;
const float AGENT_INTERPOLATION_DECAY  = 0.8f;
//const int JETPACK_TICKS_MAX = 30 * 10 * 2;
const int JETPACK_TICKS_MAX = 30 * 10 * 2 * 10000;
const int JETPACK_DECAY_TICKS = 30 * 10;

namespace AgentHudName
{
// setup const config data for coloring
const struct Color green = color_init(0, 206, 0);
const struct Color yellow = color_init(228, 222, 20);
const struct Color orange = color_init(244, 161, 35);
const struct Color orange_red = color_init(244, 88, 35);
const struct Color red = color_init(219, 29, 20);
const struct Color black = color_init(0,0,0);
const unsigned int COLOR_COUNT = 5;
const struct Color health_colors[COLOR_COUNT] = { green, yellow, orange, orange_red, red };
const float health_color_points[COLOR_COUNT] = { 1.0f, 0.6f, 0.4f, 0.3f, 0.0f };    // anchor health_color_points

const struct Color HEALTH_TEXT_DEAD_COLOR = black;
const float SIZE = 0.7f;

void verify_configuration()
{
    // validate config state
    GS_ASSERT(COLOR_COUNT > 0);
    for (unsigned int i=1; i<COLOR_COUNT; i++)
        GS_ASSERT(health_color_points[i] < health_color_points[i-1]); // must be descending order
}

}   // AgentHealthColor
#endif
