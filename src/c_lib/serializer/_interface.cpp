#include "_interface.hpp"

#include <options/server_options.hpp>

#include <serializer/map.hpp>
#include <serializer/items.hpp>
#include <serializer/_interface.hpp>

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

    if (!Options::serializer) return;

    // ORDER DEPENDENT
    init_redis();
    init_state();  
    init_players();
}

void teardown()
{
    #if PTHREADS_ENABLED
    printf("Waiting for threads to finish...\n");
    serializer::wait_for_threads();
    #endif
        
    check_map_save_state();
    
    teardown_map_serializer();

    // TODO -- save all item data, wait for responses

    // ORDER DEPENDENT
    teardown_redis();
    teardown_players();
    teardown_state();
}

bool load_data()
{
    if (!load_default_map()) return false;
    bool successful_load = load_containers();
    GS_ASSERT_ABORT(successful_load);

    //load_mechs();
    //load_players();
    
    return true;
}

void update()
{
    check_map_save_state();

    if (!Options::serializer) return;

    update_redis();
}

}   // serializer
