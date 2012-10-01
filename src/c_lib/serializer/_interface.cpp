#include "_interface.hpp"

#include <serializer/map.hpp>
#include <serializer/items.hpp>

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

    if (!Options::serialize) return;
    
    init_item_serializer();
}

void teardown()
{
    teardown_map_serializer();
    teardown_item_serializer();
}

void check_save_state()
{
    check_map_save_state();
}

}   // serializer
