#include "_interface.hpp"

#include <common/files.hpp>
#include <options/server_options.hpp>
#include <serializer/mechs.hpp>

#if GS_SERIALIZER
# include <serializer/containers.hpp>
# include <serializer/map.hpp>
# include <serializer/redis.hpp>
# include <serializer/items.hpp>
#endif

namespace serializer
{

void init()
{
    verify_config();

    // make sure paths exist
    create_path(DATA_PATH);    
    create_path(MAP_DATA_PATH);
    create_path(MECH_DATA_PATH);
    create_path(PLAYER_DATA_PATH);
    create_path(CONTAINER_DATA_PATH);
    
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

bool load_data()
{
    if (!load_default_map()) return false;

    // Actually, save it as palette.new
    // After the first time the map is saved, rename it to normal

    // we can save the new map palette now that we're done with the old one
    bool saved = save_map_palette_file();
    GS_ASSERT_ABORT(saved);

    bool successful_load = load_mechs();
    GS_ASSERT_ABORT(successful_load);

    if (!Options::serializer) return true;

    #if GS_SERIALIZER
    successful_load = load_containers();
    GS_ASSERT_ABORT(successful_load);

    //load_players();
    #endif
    
    return true;
}

void update()
{
    check_map_save_state();

    if (!Options::serializer) return;

    #if GS_SERIALIZER
    update_redis();
    #endif
}

}   // serializer
