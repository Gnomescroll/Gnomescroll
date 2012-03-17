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
    WARN,
    ERROR
} LogLevel;

FILE* get_file_descriptor(LogType type, LogLevel level);

int print(LogType type, LogLevel level, const char* fmt, ...);
int print(LogType type, const char* fmt, ...);
int print(const char* fmt, ...);

void generate_filenames();
void free_filenames();

void open_files();
void close_files();
void flush_files();

void init();
void teardown();

} // Log
