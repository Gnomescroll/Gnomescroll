#include "_interface.hpp"

#include <common/dirent.h>

#include <common/files.hpp>
#include <options/server_options.hpp>
#include <serializer/mechs.hpp>

#if GS_SERIALIZER
# include <serializer/constants.hpp>
# include <serializer/containers.hpp>
# include <serializer/map.hpp>
# include <serializer/redis.hpp>
# include <serializer/items.hpp>
#endif

namespace serializer
{

static char save_folder[SAVE_FOLDER_LEN+1] = {'\0'};

static bool set_save_folder(int version, time_t timestamp)
{
    int wrote = snprintf(save_folder, SAVE_FOLDER_LEN+1, save_folder_fmt, GS_VERSION, timestamp);
    GS_ASSERT_ABORT(wrote > 0 && (size_t)wrote <= SAVE_FOLDER_LEN);
    if (wrote <= 0 || (size_t)wrote > SAVE_FOLDER_LEN) return false;
    save_folder[SAVE_FOLDER_LEN] = '\0';
    return true;    
}

static void create_data_paths(const char* save_folder)
{
    GS_ASSERT(save_folder[0] != '\0');
    if (save_folder[0] == '\0') return;
    
    static const size_t alongest = GS_MAX(sizeof(MAP_DATA_PATH), sizeof(MECH_DATA_PATH));
    static const size_t blongest = GS_MAX(sizeof(PLAYER_DATA_PATH), sizeof(CONTAINER_DATA_PATH));
    static const size_t longest  = GS_MAX(alongest, blongest);
    
    char* path = (char*)malloc((sizeof(WORLD_DATA_PATH) + strlen(save_folder) + longest) * sizeof(char));

    sprintf(path, "%s%s%s", WORLD_DATA_PATH, save_folder, MAP_DATA_PATH);
    create_path(path);
    sprintf(path, "%s%s%s", WORLD_DATA_PATH, save_folder, MECH_DATA_PATH);
    create_path(path);
    sprintf(path, "%s%s%s", WORLD_DATA_PATH, save_folder, PLAYER_DATA_PATH);
    create_path(path);
    sprintf(path, "%s%s%s", WORLD_DATA_PATH, save_folder, CONTAINER_DATA_PATH);
    create_path(path);

    free(path);
}

bool begin_new_world_version()
{
    bool set = set_save_folder(GS_VERSION, utc_now());
    GS_ASSERT(set);
    if (!set) return false;
    bool exists = file_exists(save_folder);
    GS_ASSERT(!exists);
    if (exists) return false;
    create_path(save_folder);
    create_data_paths(save_folder);
    return true;
}

bool load_data()
{
    GS_ASSERT(save_folder[0] != '\0');
    if (save_folder[0] == '\0')
    {
        printf("There is no data to load -- we have started a new world version\n");
        return false;
    }
    
    // Load all the data
    //      Each piece loads it palette and raw data and applies any transformations needed
    // Then resave the palettes and data
    
    if (!load_default_map()) return false;

    // Actually, save it as palette.new
    // After the first time the map is saved, rename it to normal

    bool successful_load = load_mechs();
    GS_ASSERT_ABORT(successful_load);

    // we can save the new map palette now that we're done with the old one
    bool saved = save_map_palette_file();
    GS_ASSERT_ABORT(saved);

    if (!Options::serializer) return true;

    #if GS_SERIALIZER
    successful_load = load_containers();
    GS_ASSERT_ABORT(successful_load);

    //load_players();
    #endif
    
    return true;
}

void save_data()
{
    // We need to know where to save it
    // If creation_timestamp is set then we're working with that version,
    // otherwise we need to detect our version name

    save_map();
    save_mechs();

    if (Options::serializer)
    {
        save_map_palette_file();
        save_containers();
    }
}

void update()
{
    check_map_save_state();

    if (!Options::serializer) return;

    #if GS_SERIALIZER
    update_redis();
    #endif
}

static bool parse_world_dirname(const char* dir, int& version, time_t& timestamp)
{
    version = 0;
    timestamp = 0;

    // copy name over
    char _buf[NAME_MAX+1];
    strncpy(_buf, dir, NAME_MAX+1);
    _buf[NAME_MAX] = '\0';

    char* buf = &_buf[0];

    // replace the delimiter with \0, and check the component count
    size_t pts = 1;
    for (size_t i=0; buf[i] != '\0'; i++)
        if (buf[i] == WORLD_DATA_FOLDER_DELIMITER[0])
        {
            pts++;
            buf[i] = '\0';
        }
    if (pts != WORLD_DATA_FOLDER_PARTS) return false;

    // check the prefix
    if (strcmp(buf, WORLD_DATA_PREFIX) != 0) return false;
    buf = &buf[sizeof(WORLD_DATA_PREFIX)-2];

    // extract and check version
    bool err = false;
    long long int _version = parse_int(buf, err);
    if (err) return false;
    if (_version <= 0 || _version > GS_VERSION) return false;
    version = (int)version;

    buf = &buf[strlen(buf)];

    // extract and check timestamp
    long long int _timestamp = parse_int(buf, err);
    if (err) return false;
    if (_timestamp <= 0 || _timestamp > utc_now()) return false;
    timestamp = (time_t)_timestamp;

    return true;
}

static bool find_existing_world(char filename[NAME_MAX], int& version, time_t& timestamp)
{
    DIR* dp;
    struct dirent* ep;

    dp = opendir(WORLD_DATA_PATH);
    if (dp == NULL) return false;

    time_t most_recent = 0;
    int highest_version = 0;
    
    while ((ep = readdir(dp)) != NULL)
    {
        bool is_world = parse_world_dirname(ep->d_name, version, timestamp);
        if (!is_world) continue;
        if (timestamp > most_recent)
        {
            GS_ASSERT(version >= highest_version);
            highest_version = version;
            most_recent = timestamp;
            strcpy(filename, ep->d_name);
        }
    }
    int ret = closedir(dp);
    GS_ASSERT(ret == 0);

    return (most_recent != 0 && highest_version != 0);
}

void init()
{
    verify_config();

    create_path(WORLD_DATA_PATH);

    // find the most up to date world data, if present
    // TODO

    int version = 0;
    time_t timestamp = 0;
    char filename[NAME_MAX] = {'\0'};
    bool found = find_existing_world(filename, version, timestamp);
    if (found)
    {   // save the folder name
        GS_ASSERT_ABORT(strlen(filename) <= SAVE_FOLDER_LEN);
        
        strncpy(save_folder, filename, SAVE_FOLDER_LEN);
        save_folder[SAVE_FOLDER_LEN] = '\0';

        // create the folder
        char* path = (char*)malloc((sizeof(WORLD_DATA_PATH)+strlen(save_folder)) * sizeof(char));
        sprintf(path, "%s%s", WORLD_DATA_PATH, save_folder);
        create_path(path);
        free(path);
        create_data_paths(save_folder);
    }
    else
        begin_new_world_version();
    
    init_map_serializer();
    init_state();
    init_mechs();

    if (!Options::serializer) return;

    #if GS_SERIALIZER
    // ORDER DEPENDENT
    init_redis();
    init_items();
    init_players();
    #endif
}

void teardown()
{
    #if PTHREADS_ENABLED
    printf("Waiting for threads to finish...\n");
    serializer::wait_for_threads();
    #endif
        
    check_map_save_state();
    
    teardown_map_serializer();
    teardown_mechs();

    #if GS_SERIALIZER    
    // TODO -- save all item data, wait for responses

    // ORDER DEPENDENT
    teardown_redis();
    teardown_players();
    teardown_items();
    teardown_state();
    #endif
}

}   // serializer
