#pragma once

#include <common/color.hpp>

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
    NULL_DEATH = 0,
    DEATH_NORMAL,
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

typedef enum
{
    AGENT_PART_TORSO = 0,
    AGENT_PART_HEAD,
    AGENT_PART_LARM,
    AGENT_PART_RARM,
    AGENT_PART_LLEG,
    AGENT_PART_RLEG,
    NULL_AGENT_PART,    // must come last
}  AgentBodyParts;

typedef enum
{
    NULL_AGENT = 0xFF,
} AgentID;

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

const float AGENT_BOX_RADIUS = 0.30f;

const int HUNGER_INCREASE_RATE = ONE_SECOND * 45;   //hunger bar is length 10
const int HUNGER_REGEN_THRESHOLD = 3;
const int HUNGER_REGEN_RATE = ONE_SECOND * 4;
const int HUNGER_REGEN_AMOUNT = 1;
const int HUNGER_DAMAGE_THRESHOLD = 8;
const int HUNGER_DAMAGE_RATE = ONE_SECOND*8;
const int HUNGER_DAMAGE_AMOUNT = 1;             //on fourth of a heart every 8 seconds

const int RAD_EXPOSURE_MAX = 1024;
const int RAD_LEACH_RATE = 1; //4x per second


const size_t MAX_AGENT_MODIFIERS = 0xFF;

const Color AGENT_DEFAULT_COLOR = Color(64,192,64);
const int RESPAWN_TICKS = ONE_SECOND * 4;

// DONT CHANGE THESE. WILL BREAK STUFF AND YOU WONT KNOW UNTIL ITS TOO LATE
const unsigned int PLAYER_NAME_MIN_LENGTH = 3;
const unsigned int PLAYER_NAME_MAX_LENGTH = 15;

// physics constants
const float tr = 1.0f / 10.0f;
const float tr2 = tr*tr;
const float AGENT_GRAVITY = -3.0f * tr2;
const float JETPACK_VELOCITY = -AGENT_GRAVITY + tr2;
const float JETPACK_MAX_VELOCITY = JETPACK_VELOCITY * 5.0f;
const float JETPACK_MAX_HEIGHT = 8.0f;

// no damage should be taken falling from this height:
const float FALL_SAFE_HEIGHT = JETPACK_MAX_HEIGHT + 1;
// a player with default max_health and no other defense (i.e. just spawned)
// should die if falling from this height:
const float FALL_DEATH_HEIGHT = 32.0f;

inline bool isValid(AgentID agent_id)
{
    return (agent_id >= 0 && agent_id < MAX_AGENTS);
}

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
