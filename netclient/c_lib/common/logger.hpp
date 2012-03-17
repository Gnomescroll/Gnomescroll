#pragma once

#define printlog(TYPE, LEVEL, FMT, ...) do { Log::print(TYPE, LEVEL, __FILE__, __LINE__, __FUNCTION__, (char*)FMT , ##__VA_ARGS__); } while(0)

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

int print(LogType type, LogLevel level, const char* file, int line, const char* function, char* fmt, ...);
int print(LogType type, const char* file, int line, const char* function, char* fmt, ...);
int print(const char* file, int line, const char* function, char* fmt, ...);

void init();
void teardown();

} // Log
