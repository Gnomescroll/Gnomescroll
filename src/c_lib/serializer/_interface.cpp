#include "_interface.hpp"

#include <serializer/map.hpp>

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
    
    init_map_serializer();
}

void teardown()
{
    teardown_map_serializer();
}

void check_save_state()
{
    check_map_save_state();
}

}   // serializer
