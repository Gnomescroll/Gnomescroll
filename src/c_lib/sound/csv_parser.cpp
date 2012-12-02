#include "csv_parser.hpp"

#include <sound/triggers.hpp>

namespace Sound
{

// the csv columns in left to right order
typedef enum
{
    EVENT_NAME = 0,
    FILE_NAME,
    IN_USE,         // not code
    INTEGRATED,     // not code
    PITCH,
    GAIN,
    MAX_DISTANCE,
    REFERENCE_DISTANCE,
    MINIMUM_GAIN,
    MAXIMUM_GAIN,
    ROLLOFF_FACTOR,
    LOOP,
    MAX_PLAYABLE_DISTANCE,
    NOTES           // not code
} ColumnName;

//defaults
const float GS_DEFAULT_PITCH = 1.0f;
const float GS_DEFAULT_GAIN = 1.0f;
const float GS_DEFAULT_MAX_DISTANCE = 128.0f;
const float GS_DEFAULT_REFERENCE_DISTANCE = 16.0f;
const float GS_DEFAULT_MINIMUM_GAIN = 0.0f;
const float GS_DEFAULT_MAXIMUM_GAIN = 1.0f;
const float GS_DEFAULT_ROLLOFF_FACTOR = 1.0f;
const bool GS_DEFAULT_LOOP = false;
const float GS_DEFAULT_MAX_PLAYABLE_DISTANCE = 128.0f;

void parse_sound_triggers(const char* csv_filename)
{
    size_t size = 0;
    char *buff = read_file_to_buffer(csv_filename, &size);
    GS_ASSERT(buff != NULL && size != 0);
    if (buff == NULL)
    {
        printf("Error opening sound conf: %s\n", csv_filename);
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
    soundfiles = (class Soundfile*)malloc(sizeof(class Soundfile) * n_lines);

    i = 0;
    int n = 0;
    int column = 0;

    const int MAX_EVENT_NAME_LEN = 100;
    char* filename = (char*)calloc(MAX_EVENT_NAME_LEN+1, sizeof(char));
    char* event_name = (char*)calloc(MAX_EVENT_NAME_LEN+1, sizeof(char));
    int filename_index = 0;
    int event_name_index = 0;

    const int MAX_FLOAT_BUFFER_INDEX = 10;
    const int MAX_BOOL_BUFFER_INDEX = 1;
    char pitch[MAX_FLOAT_BUFFER_INDEX+1];
    char gain[MAX_FLOAT_BUFFER_INDEX+1];
    char max_distance[MAX_FLOAT_BUFFER_INDEX+1];
    char reference_distance[MAX_FLOAT_BUFFER_INDEX+1];
    char minimum_gain[MAX_FLOAT_BUFFER_INDEX+1];
    char maximum_gain[MAX_FLOAT_BUFFER_INDEX+1];
    char rolloff_factor[MAX_FLOAT_BUFFER_INDEX+1];
    char loop[MAX_BOOL_BUFFER_INDEX+1];
    char max_playable_distance[MAX_FLOAT_BUFFER_INDEX+1];
    
    int pitch_index = 0;
    int gain_index = 0;
    int max_distance_index = 0;
    int reference_distance_index = 0;
    int minimum_gain_index = 0;
    int maximum_gain_index = 0;
    int rolloff_factor_index = 0;
    int loop_index = 0;
    int max_playable_distance_index = 0;
    
    float pitch_value;
    float gain_value;
    float max_distance_value;
    float reference_distance_value;
    float minimum_gain_value;
    float maximum_gain_value;
    float rolloff_factor_value;
    bool loop_value;
    float max_playable_distance_value;

    // event name is stored in the 1st column
    // filename is stored in the 2nd column,
    // commas deliminate columns
    // newlines deliminate entries
    // any character that cant be a filename or event name character should throw warning,
    // but only ignoring spaces as is
    // the csv exported by gnumeric contains no spaces, and looks like: col1,col2,...,colN,\n

    while ((c = buff[i++]) != '\0')
    {
        if (c == ' ')
            continue;

        if (c == ',')
        {
            column++;
            continue;
        }
            
        if (n > 0)  // first line of csv  is metadata, skip
        {
            switch (column)
            {
                case FILE_NAME:
                    if (filename_index >= MAX_EVENT_NAME_LEN)
                    {
                        printf("Error parsing sounds conf: filename %s exceeding max length (%d)\n", filename, MAX_EVENT_NAME_LEN);
                        return;
                    }
                    if (isspace(c))
                        break;
                    filename[filename_index++] = c;
                    break;
                    
                case EVENT_NAME: 
                    if (event_name_index >= MAX_EVENT_NAME_LEN)
                    {
                        printf("Error parsing sounds conf: event name %s exceeding max length (%d)\n", event_name, MAX_EVENT_NAME_LEN);
                        return;
                    }
                    if (isspace(c))
                        break;
                    event_name[event_name_index++] = c;
                    break;

                case PITCH:
                    if (pitch_index >= MAX_FLOAT_BUFFER_INDEX || !(isdigit(c) || c == '.' || c == '-'))
                        break;
                    pitch[pitch_index++] = c;
                    break;
                    
                case GAIN:
                    if (gain_index >= MAX_FLOAT_BUFFER_INDEX || !(isdigit(c) || c == '.' || c == '-'))
                        break;
                    gain[gain_index++] = c;
                    break;

                case MAX_DISTANCE:
                    if (max_distance_index >= MAX_FLOAT_BUFFER_INDEX || !(isdigit(c) || c == '.' || c == '-'))
                        break;
                    max_distance[max_distance_index++] = c;
                    break;

                case REFERENCE_DISTANCE:
                    if (reference_distance_index >= MAX_FLOAT_BUFFER_INDEX || !(isdigit(c) || c == '.' || c == '-'))
                        break;
                    reference_distance[reference_distance_index++] = c;
                    break;

                case MINIMUM_GAIN:
                    if (minimum_gain_index >= MAX_FLOAT_BUFFER_INDEX || !(isdigit(c) || c == '.' || c == '-'))
                        break;
                    minimum_gain[minimum_gain_index++] = c;
                    break;

                case MAXIMUM_GAIN:
                    if (maximum_gain_index >= MAX_FLOAT_BUFFER_INDEX || !(isdigit(c) || c == '.' || c == '-'))
                        break;
                    maximum_gain[maximum_gain_index++] = c;
                    break;

                case ROLLOFF_FACTOR:
                    if (rolloff_factor_index >= MAX_FLOAT_BUFFER_INDEX || !(isdigit(c) || c == '.' || c == '-'))
                        break;
                    rolloff_factor[rolloff_factor_index++] = c;
                    break;

                case LOOP:
                    if (loop_index >= MAX_BOOL_BUFFER_INDEX || !(isdigit(c)))
                        break;
                    loop[loop_index++] = c;
                    break;

                case MAX_PLAYABLE_DISTANCE:
                    if (max_playable_distance_index >= MAX_FLOAT_BUFFER_INDEX || !(isdigit(c) || c == '.' || c == '-'))
                        break;
                    max_playable_distance[max_playable_distance_index++] = c;
                    break;
                
                // unused by code 
                case IN_USE:
                case INTEGRATED:
                case NOTES:
                    break;
            }
        }
            
        if (c == '\n')
        {   // end of row; insert data
            n++;
            if (filename_index && event_name_index)
            {
                // end the string
                filename[filename_index] = '\0';
                event_name[event_name_index] = '\0';
                pitch[pitch_index] = '\0';
                gain[gain_index] = '\0';
                max_distance[max_distance_index] = '\0';
                reference_distance[reference_distance_index] = '\0';
                minimum_gain[minimum_gain_index] = '\0';
                maximum_gain[maximum_gain_index] = '\0';
                rolloff_factor[rolloff_factor_index] = '\0';
                loop[loop_index] = '\0';
                max_playable_distance[max_playable_distance_index] = '\0';

                // set strings
                int snd_id = n_sounds;
                if (set_soundfile(snd_id, event_name, filename))
                {   // set properties
                    n_sounds++;
                    pitch_value = (pitch_index) ? (float)atof(pitch) : GS_DEFAULT_PITCH;
                    gain_value = (gain_index) ? (float)atof(gain) : GS_DEFAULT_GAIN;
                    max_distance_value = (max_distance_index) ? (float)atof(max_distance) : GS_DEFAULT_MAX_DISTANCE;
                    reference_distance_value = (reference_distance_index) ? (float)atof(reference_distance) : GS_DEFAULT_REFERENCE_DISTANCE;
                    minimum_gain_value = (minimum_gain_index) ? (float)atof(minimum_gain) : GS_DEFAULT_MINIMUM_GAIN;
                    maximum_gain_value = (maximum_gain_index) ? (float)atof(maximum_gain) : GS_DEFAULT_MAXIMUM_GAIN;
                    rolloff_factor_value = (rolloff_factor_index) ? (float)atof(rolloff_factor) : GS_DEFAULT_ROLLOFF_FACTOR;
                    loop_value = (loop_index) ? (bool)atoi(loop) : GS_DEFAULT_LOOP;
                    max_playable_distance_value = (max_playable_distance_index) ? (float)atof(max_playable_distance) : GS_DEFAULT_MAX_PLAYABLE_DISTANCE;
                    set_soundfile_properties(snd_id, pitch_value, gain_value, max_distance_value, reference_distance_value, minimum_gain_value, maximum_gain_value, rolloff_factor_value, loop_value, max_playable_distance_value);
                }

                if (n_sounds > n_lines)
                {
                    printf("OPENAL: double line allocation\n");
                    n_lines *= 2;
                    class Soundfile* new_soundfiles = (class Soundfile*)realloc(soundfiles, sizeof(class Soundfile) * n_lines);
                    GS_ASSERT(new_soundfiles != NULL);
                    if (new_soundfiles != NULL)
                        soundfiles = new_soundfiles;
                    else
                    {
                        n_lines /= 2;
                        break;
                    }
                }
            }

            filename_index = 0;
            event_name_index = 0;
            pitch_index = 0;
            gain_index = 0;
            max_distance_index = 0;
            reference_distance_index = 0;
            minimum_gain_index = 0;
            maximum_gain_index = 0;
            rolloff_factor_index = 0;
            loop_index = 0;
            max_playable_distance_index = 0;
            column = 0;
        }
    }

    // finalize
    if (n_sounds == 0 && soundfiles != NULL)
    {
        free(soundfiles);
        soundfiles = NULL;
    }

    // cleanup
    free(buff);
    free(filename);
    free(event_name);
}

}   // Sound
