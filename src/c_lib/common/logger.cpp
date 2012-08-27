#include "logger.hpp"

#include <stdarg.h>

#if DC_CLIENT
#include <options/client_options.hpp>
#endif

#if DC_SERVER
#include <options/server_options.hpp>
#endif

namespace Log
{
    
static const int N_LOG_FILES = 15;  // update this if adding/removing a LogType

static const char GENERIC_FN[] = "generic";
static const char AUDIO_FN[] = "audio";
static const char AGENT_FN[] = "agent";
static const char MAP_FN[] = "map";
static const char SHADER_FN[] = "shader";
static const char SETUP_FN[] = "setup";
static const char GAME_FN[] = "game";
static const char ITEM_FN[] = "item";
static const char VOXEL_FN[] = "voxel";
static const char MAP_GEN_FN[] = "map_gen";
static const char HUD_FN[] = "hud";
static const char CHAT_FN[] = "chat";
static const char WEAPON_FN[] = "weapon";
static const char ANALYTICS_FN[] = "analytics";
static const char UNKNOWN_FN[] = "unknown";

static const char FILENAME_FMT[] = "%s%s-%d-%s.log";
static const char LOG_DIR[] = "./log/";

static const char LOG_MSG_FMT[] = "%s:%d %s -- %s";
static const int LOG_MSG_MAX_LEN = 2048;
static char* log_buffer = NULL;

static const LogType DEFAULT_TYPE = GENERIC;
static const LogLevel DEFAULT_LEVEL = Always;

static FILE* log_files[N_LOG_FILES] = {NULL};
static char* log_filenames[N_LOG_FILES] = {NULL};

FILE* open_file(LogType type)
{
    if (type < 0 || type > UNKNOWN) return NULL;
    FILE* f = fopen(log_filenames[type], "w");
    if (f == NULL) printf("Couldnt create log file %s. Does the folder %s exist?\n", log_filenames[type], LOG_DIR);
    if (f != NULL && feof(f))
    {
        fclose(f);
        f = NULL;
    }
    return f;
}

FILE* get_file(LogType type)
{
    if (type < 0 || type > UNKNOWN) type = UNKNOWN;
    if (log_files[type] != NULL) log_files[type] = open_file(type);
    return log_files[type];
}

void shutdown_file(FILE* f)
{
    if (f == NULL) return;
    fclose(f);
}

void shutdown_file(LogType type)
{
    shutdown_file(log_files[type]);
    log_files[type] = NULL;
}

bool file_ok(FILE* f)
{
    if (f == NULL || feof(f) || ferror(f))
        return false;
    return true;
}

FILE* get_file_descriptor(LogType type, LogLevel level)
{
    if (Options::logger)
    {
        FILE *f = get_file(type);
        if (f == NULL || !file_ok(f))
        {
            shutdown_file(type);
            return NULL;
        }
        return f;
    }
    else
    {
        if (level == Error)
            return stderr;
        return stdout;
    }
}

// fprintf wrapper
int print(LogType type, LogLevel level, const char* file, int line, const char* function, char* fmt, ...)
{
    if (!Options::logger) return -1;
    
    GS_ASSERT(log_buffer != NULL);
    
    if (log_buffer == NULL) return -1;
    FILE* f = get_file_descriptor(type, level);
    if (f == NULL) return -1;

    int len = (int) (strlen(file) + 10 + strlen(function) + strlen(LOG_MSG_FMT) - 8);
    int remain = LOG_MSG_MAX_LEN - len - 1;
    len = (int)strlen(fmt);
    if (len > remain)
        fmt[remain] = '\0';
    sprintf(log_buffer, LOG_MSG_FMT, file, line, function, fmt);

    va_list args;
    va_start(args, fmt);
    int res = vfprintf(f, log_buffer, args);
    va_end(args);
    GS_ASSERT(res >= 0);
    if (res < 0)
        shutdown_file(type);
    return res;
}

int print(LogType type, const char* file, int line, const char* function, char* fmt, ...)
{
    if (!Options::logger) return -1;
    va_list args;
    va_start(args, fmt);
    int res = print(type, DEFAULT_LEVEL, file, line, function, fmt, args);
    va_end(args);
    return res;
}

int print(const char* file, int line, const char* function, char* fmt, ...)
{
    if (!Options::logger) return -1;
    va_list args;
    va_start(args, fmt);
    int res = print(DEFAULT_TYPE, DEFAULT_LEVEL, file, line, function, fmt, args);
    va_end(args);
    return res;
}

int print(LogType type, LogLevel level, char* fmt, ...)
{
    GS_ASSERT(fmt != NULL);
    if (fmt == NULL) return -1;
    
    if (!Options::logger) return -1;
    GS_ASSERT(log_buffer != NULL);
    if (log_buffer == NULL) return -1;

    FILE* f = get_file_descriptor(type, level);
    if (f == NULL) return -1;
    
    va_list args;
    va_start(args, fmt);
    int res = vfprintf(f, fmt, args);
    va_end(args);
    return res;
}

#define GEN_FN_CASE(TYPENAME) \
    case TYPENAME: \
        fn = (char*)malloc(sizeof(char) * \
            ((sizeof(TYPENAME##_FN)-1) \
          + (sizeof(FILENAME_FMT)-1) \
          + (sizeof(LOG_DIR)-1) \
          + (sizeof(time_str)-1) \
          + 30 - 1)); \
        sprintf(fn, FILENAME_FMT, LOG_DIR, TYPENAME##_FN, GS_VERSION, time_str); \
        log_filenames[i] = fn; \
        break;

void generate_filenames()
{
    char* time_str = get_time_str();
    char* fn;
    for (int i=0; i<N_LOG_FILES; i++)
    {
        switch (i)
        {
            GEN_FN_CASE(GENERIC)
            GEN_FN_CASE(AUDIO)
            GEN_FN_CASE(AGENT)
            GEN_FN_CASE(MAP)
            GEN_FN_CASE(SHADER)
            GEN_FN_CASE(SETUP)
            GEN_FN_CASE(GAME)
            GEN_FN_CASE(ITEM)
            GEN_FN_CASE(VOXEL)
            GEN_FN_CASE(MAP_GEN)
            GEN_FN_CASE(HUD)
            GEN_FN_CASE(CHAT)
            GEN_FN_CASE(WEAPON)
            GEN_FN_CASE(ANALYTICS)
            GEN_FN_CASE(UNKNOWN)
            default:
                fprintf(stdout, "Warning -- Log::generate_filenames() -- N_LOG_FILES %d is out of sync with LogTypes\n", N_LOG_FILES);
                return;
        }
    }
}
#undef GEN_FN_CASE

void free_filenames()
{
    for (int i=0; i<N_LOG_FILES; i++)
        if (log_filenames[i] != NULL)
            free(log_filenames[i]);
}

void close_files()
{
    for (int i=0; i<N_LOG_FILES; i++)
        if (log_files[i] != NULL)
        {
            fclose(log_files[i]);
            log_files[i] = NULL;
        }
}

void init()
{
    create_path(LOG_DIR);
    if (!Options::logger) return;
    generate_filenames();
    log_buffer = (char*)malloc(sizeof(char) * LOG_MSG_MAX_LEN);
    GS_ASSERT(log_buffer != NULL);
}

void teardown()
{
    if (!Options::logger) return;
    close_files();
    free_filenames();
    if (log_buffer != NULL)
        free(log_buffer);
}

} // Log
