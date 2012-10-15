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
    
    //create_path(PLAYER_DATA_PATH);
    //create_path(CONTAINER_DATA_PATH);
    //create_path(ITEM_DATA_PATH);
    
    //create_path(PLAYER_DATA_PATH    INVALID_DATA_SUBPATH);
    //create_path(CONTAINER_DATA_PATH INVALID_DATA_SUBPATH);
    //create_path(ITEM_DATA_PATH      INVALID_DATA_SUBPATH);
    
    init_map_serializer();

    if (!Options::serializer) return;
    
    init_state();  
    init_players();
    init_redis();
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

    teardown_redis();   // MUST COME FIRST -- all callbacks/data need to return results
    teardown_players();
    teardown_state();
}

void update()
{
    check_map_save_state();

    if (!Options::serializer) return;

    update_redis();
}

}   // serializer
