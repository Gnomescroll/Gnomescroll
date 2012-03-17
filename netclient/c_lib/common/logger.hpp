#pragma once

namespace Log
{
    
typedef enum
{
    GENERIC = 0,
    AUDIO,
    AGENT,
    MAP,
    SHADER,
    SETUP,
    GAME,
    ITEM,
    VOXEL,
    MAP_GEN,
    HUD,
    CHAT,
    WEAPON,
    UNKNOWN // always leave this at the end
} LogType;

typedef enum
{
    ALWAYS = 0,
    WARNING,
    ERROR
} LogLevel;

int print(LogType type, LogLevel level, const char* fmt, ...);
int print(LogType type, const char* fmt, ...);
int print(const char* fmt, ...);

void init();
void teardown();

} // Log
