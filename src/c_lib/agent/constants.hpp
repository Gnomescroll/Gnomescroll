#pragma once

#include <common/color.hpp>

/*
    Agent #defines, constants
*/


typedef enum
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
} CSKeys;

typedef enum
{
    DEATH_NORMAL = 0,
    DEATH_HEADSHOT,
    DEATH_GRENADE,
    DEATH_FALL,
    DEATH_BELOW_MAP,
    DEATH_TURRET,
    DEATH_SLIME,
    DEATH_KILLME,
    DEATH_PLASMAGEN,
    DEATH_STARVATION,
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
const int MAX_AGENTS = PLAYERS_MAX;
//const float AGENT_HEIGHT = 2.8f;
//const float CAMERA_HEIGHT = 2.5f;
const float AGENT_HEIGHT = 1.8f;
const float CAMERA_HEIGHT = 1.5f;
const float AGENT_HEIGHT_CROUCHED = 1.8f;
const float CAMERA_HEIGHT_CROUCHED = 1.5f;
const float CAMERA_HEIGHT_DEAD = 0.3f;
const float AGENT_SPEED = 1.8f;
//const float AGENT_SPEED = 28.0f; // uncomment for testing high speed
const float AGENT_SPEED_CROUCHED = 0.9f;
const float AGENT_JUMP_POWER = 0.25f;

//const float AGENT_BOX_RADIUS = 0.38f;
const float AGENT_BOX_RADIUS = 0.19f;

const int HUNGER_INCREASE_RATE = ONE_MINUTE * 2;
const int HUNGER_REGEN_THRESHOLD = 2;
const int HUNGER_REGEN_RATE = ONE_SECOND * 30;
const int HUNGER_REGEN_AMOUNT = 10;
const int HUNGER_DAMAGE_THRESHOLD = 8;
const int HUNGER_DAMAGE_RATE = ONE_MINUTE;
const int HUNGER_DAMAGE_AMOUNT = 10;

typedef enum
{
    NULL_AGENT = 0xFF,
} AgentID;

inline bool isValid(AgentID agent_id)
{
    return (agent_id >= 0 && agent_id < MAX_AGENTS);
}

const size_t MAX_AGENT_MODIFIERS = 0xFF;

/* Status */
const int RESPAWN_TICKS = 30 * 4; // 4 seconds
const int AGENT_MAX_TURRETS = 10;    // (256/64)*2 == 8
const int AGENT_MAX_SPAWNERS = 3;

// DONT CHANGE THESE. WILL BREAK STUFF AND YOU WONT KNOW UNTIL ITS TOO LATE
const unsigned int PLAYER_NAME_MIN_LENGTH = 3;
const unsigned int PLAYER_NAME_MAX_LENGTH = 15;

const Color AGENT_DEFAULT_COLOR = Color(64,192,64);

/* Player */
#if DC_CLIENT
const int AGENT_STATE_HISTORY_SIZE = 64;
const float AGENT_INTERPOLATION_DECAY  = 0.8f;

namespace AgentHudName
{
// setup const config data for coloring
const Color green = Color(0, 206, 0);
const Color yellow = Color(228, 222, 20);
const Color orange = Color(244, 161, 35);
const Color orange_red = Color(244, 88, 35);
const Color red = Color(219, 29, 20);
const Color black = Color(0,0,0);
const unsigned int COLOR_COUNT = 5;
const Color health_colors[COLOR_COUNT] = { green, yellow, orange, orange_red, red };
const float health_color_points[COLOR_COUNT] = { 1.0f, 0.6f, 0.4f, 0.3f, 0.0f };    // anchor health_color_points

const Color HEALTH_TEXT_DEAD_COLOR = black;
const float SIZE = 0.7f;

void verify_configuration()
{
    // validate config state
    GS_ASSERT(COLOR_COUNT > 0);
    for (unsigned int i=1; i<COLOR_COUNT; i++)
        GS_ASSERT(health_color_points[i] < health_color_points[i-1]); // must be descending order
}

}   // AgentHudName
#endif
