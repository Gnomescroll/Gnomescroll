#pragma once

#define printlog(TYPE, LEVEL, FMT, ...) do { Log::print(TYPE, LEVEL, __FILE__, __LINE__, __FUNCTION__, (char*)FMT , ##__VA_ARGS__); } while(0)
#define log_simple(TYPE, LEVEL, FMT, ...) do { Log::print(TYPE, LEVEL, (char*)FMT , ##__VA_ARGS__); } while(0)

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
    ANALYTICS,
    UNKNOWN // always leave this at the end
} LogType;

typedef enum
{
    Always = 0,
    Warning,
    Error
} LogLevel;

int print(LogType type, LogLevel level, const char* file, int line, const char* function, char* fmt, ...);
int print(LogType type, const char* file, int line, const char* function, char* fmt, ...);
int print(const char* file, int line, const char* function, char* fmt, ...);
int print(LogType type, LogLevel level, char* fmt, ...);    // simple version no markup

void init();
void teardown();

} // Log
