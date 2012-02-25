#include "triggers.hpp"

#include <sound/sound.hpp>

namespace Sound
{

#define SOUND_TRIGGER(NAME)\
void NAME()\
{\
    static bool looked = false;\
    static char* soundfile = NULL;\
    if (!looked && soundfile == NULL)\
    {\
        looked = true;\
        soundfile = get_soundfile((char*)#NAME);\
    }\
    if (soundfile != NULL)\
        play_2d_sound(soundfile);\
}\
void NAME(float x, float y, float z, float vx, float vy, float vz)\
{\
    static bool looked = false;\
    static char* soundfile = NULL;\
    if (!looked && soundfile == NULL)\
    {\
        looked = true;\
        soundfile = get_soundfile((char*)#NAME);\
    }\
    if (soundfile != NULL)\
        play_3d_sound(soundfile, x,y,z,vx,vy,vz);\
}

/* triggers */

SOUND_TRIGGER(fire_laser)
SOUND_TRIGGER(out_of_ammo)
SOUND_TRIGGER(reload)
SOUND_TRIGGER(laser_hit_block)
SOUND_TRIGGER(laser_hit_agent)
SOUND_TRIGGER(pick_hit_block)
SOUND_TRIGGER(pick_hit_agent)
SOUND_TRIGGER(pick_swung)
SOUND_TRIGGER(block_destroyed)
SOUND_TRIGGER(block_took_damage)
SOUND_TRIGGER(flag_picked_up)
SOUND_TRIGGER(flag_scored)
SOUND_TRIGGER(spawner_placed)
SOUND_TRIGGER(respawned)
SOUND_TRIGGER(died)
SOUND_TRIGGER(walking)
SOUND_TRIGGER(running)
SOUND_TRIGGER(jetpack_on)
SOUND_TRIGGER(soft_landing)
SOUND_TRIGGER(medium_landing)
SOUND_TRIGGER(hard_landing)
SOUND_TRIGGER(victory)
SOUND_TRIGGER(restore_health)
SOUND_TRIGGER(restore_ammo)
SOUND_TRIGGER(block_set)
SOUND_TRIGGER(grenade_explode)
SOUND_TRIGGER(grenade_bounce)
SOUND_TRIGGER(agent_took_damage)


/* function -> filename mapping */

struct Sound_file_function_map* sound_file_functions = NULL;
int n_sounds = 0;

char* get_soundfile(char *fn)
{
    if (sound_file_functions == NULL)
        return NULL;
    for (int i=0; i<n_sounds; i++)
        if (!strcmp(fn, sound_file_functions[i].fn))
            return sound_file_functions[i].file;
    return NULL;
}

bool set_soundfile(int i, char* fn, char* file)
{
    // check if function mapped already
    if (get_soundfile(fn) != NULL)
        return false;
    
    int fn_len = strlen(fn);
    int file_len = strlen(file);
    sound_file_functions[i].fn = (char*)malloc(sizeof(char) * (fn_len + 1));
    strcpy(sound_file_functions[i].fn, fn);
    sound_file_functions[i].file = (char*)malloc(sizeof(char) * (file_len + 1));
    strcpy(sound_file_functions[i].file, file);

    load_sound(file);

    printf("Set sound trigger: %s -> %s\n", fn, file);

    return true;
}

void parse_sound_triggers(char *fn)
{
    int size = 0;
    char *buff = read_file_to_buffer(fn, &size);
    if (buff == NULL)
    {
        printf("Error opening sound conf: %s\n", fn);
        return;
    }
    if (size == 0)
    {
        printf("Sounds conf is empty.\n");
        return;
    }

    char c;
    int i = 0;
    int n_lines = 0;
    while ((c = buff[i++]) != '\0')
    {   // count number of lines, should correspond with number of sounds
        if (c == '\n')
            n_lines++;
    }

    if (n_lines == 0)
    {
        printf("Error parsing sounds conf: no lines found\n");
        return;
    }
    n_lines -= 1;   // first line of csv is metadata
    sound_file_functions = (struct Sound_file_function_map*)malloc(sizeof(struct Sound_file_function_map) * n_lines);

    i = 0;
    int n = 0;
    int comma = 0;

    const int MAX_FN_LEN = 100;
    char* filename = (char*)calloc(MAX_FN_LEN+1, sizeof(char));
    char* function_name = (char*)calloc(MAX_FN_LEN+1, sizeof(char));
    int filename_index = 0;
    int function_name_index = 0;

    // filename is stored in the 2nd column (1-indexed),
    // function name is stored in the 3rd column
    // commas deliminate columns
    // newlines deliminate entries
    // any character that cant be a filename or function name character should throw warning,
    // but only ignoring spaces as is
    // the csv exported by gnumeric contains no spaces, and looks like: col1,col2,...,colN,\n

    while ((c = buff[i++]) != '\0')
    {
        if (c == ' ')
            continue;

        if (c == ',')
        {
            comma++;
            continue;
        }
            
        if (n > 0)  // first line of csv  is metadata, skip
        {
            if (comma == 1)
            {
                if (filename_index == MAX_FN_LEN)
                {
                    printf("Error parsing sounds conf: filename %s exceeding max length (%d)\n", filename, MAX_FN_LEN);
                    return;
                }
                filename[filename_index++] = c;
            }
            else if (comma == 2)
            {
                if (function_name_index == MAX_FN_LEN)
                {
                    printf("Error parsing sounds conf: function name %s exceeding max length (%d)\n", function_name, MAX_FN_LEN);
                    return;
                }
                function_name[function_name_index++] = c;
            }
        }
            
        if (c == '\n')
        {
            n++;
            if (comma >= 2 && filename_index && function_name_index)
            {
                filename[filename_index] = '\0';
                function_name[function_name_index] = '\0';
                if (set_soundfile(n_sounds, function_name, filename))
                    n_sounds++;

                if (n_sounds > n_lines)
                {
                    n_lines *= 2;
                    sound_file_functions = (struct Sound_file_function_map*)realloc(sound_file_functions, sizeof(struct Sound_file_function_map) * n_lines);
                }
            }
            
            filename_index = 0;
            function_name_index = 0;
            comma = 0;
        }
    }

    if (n_sounds == 0)
        sound_file_functions = NULL;
    else if (n_sounds != n_lines)
        sound_file_functions = (struct Sound_file_function_map*)realloc(sound_file_functions, sizeof(struct Sound_file_function_map) *  n_sounds);

    free(buff);
    free(filename);
    free(function_name);
}

void teardown_triggers()
{
    if (sound_file_functions == NULL)
        return;
        
    for (int i=0; i<n_sounds; i++)
    {
        free(sound_file_functions[i].fn);
        free(sound_file_functions[i].file);
    }
    free(sound_file_functions);
}

}
