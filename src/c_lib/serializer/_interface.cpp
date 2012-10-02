#include "_interface.hpp"

#include <options/server_options.hpp>

#include <serializer/map.hpp>
#include <serializer/items.hpp>
#include <serializer/redis/_interface.hpp>

namespace serializer
{

void init()
{
    // make sure paths exist
    create_path(DATA_PATH);
    
    create_path(MAP_DATA_PATH);
    create_path(MECH_DATA_PATH);
    create_path(PLAYER_DATA_PATH);
    create_path(CONTAINER_DATA_PATH);
    create_path(ITEM_DATA_PATH);
    
    create_path(PLAYER_DATA_PATH    INVALID_DATA_SUBPATH);
    create_path(CONTAINER_DATA_PATH INVALID_DATA_SUBPATH);
    create_path(ITEM_DATA_PATH      INVALID_DATA_SUBPATH);
    
    init_map_serializer();

    if (!Options::serializer) return;
    
    init_item_serializer();
    redis::update();
}

void teardown()
{
    #if PTHREADS_ENABLED
    printf("Waiting for threads to finish...\n");
    serializer::wait_for_threads();
    #endif
        
    check_map_save_state();
    
    teardown_map_serializer();
    teardown_item_serializer();    

    redis::teardown();
}

void update()
{
    check_map_save_state();

    if (!Options::serializer) return;

    redis::update();
}

}   // serializer
