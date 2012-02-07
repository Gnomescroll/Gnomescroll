#pragma once

const int CHAT_MESSAGE_SIZE_MAX = 90;
const int CHAT_MESSAGE_RENDER_TIMEOUT = 1000*10; // 10 seconds

// first 16 are reserved
// next 16 are for teams
// next [32,PLAYERS_MAX] are for pm channels 
const int CHANNEL_ID_TEAM_OFFSET = 16;
const int CHANNEL_ID_AGENT_OFFSET = 32;

const int CHAT_SENDER_SYSTEM = 255;

const unsigned char CHAT_SYSTEM_COLOR_R = 255;
const unsigned char CHAT_SYSTEM_COLOR_G = 0;
const unsigned char CHAT_SYSTEM_COLOR_B = 0;

const unsigned char CHAT_PM_COLOR_R = 0;
const unsigned char CHAT_PM_COLOR_G = 0;
const unsigned char CHAT_PM_COLOR_B = 255;

const unsigned char CHAT_UNKNOWN_COLOR_R = 0;
const unsigned char CHAT_UNKNOWN_COLOR_G = 0;
const unsigned char CHAT_UNKNOWN_COLOR_B = 0;

const unsigned char CHAT_GLOBAL_COLOR_R = 20;
const unsigned char CHAT_GLOBAL_COLOR_G = 210;
const unsigned char CHAT_GLOBAL_COLOR_B = 20;
