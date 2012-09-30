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
    
    init_map_serializer();

    SerializerError err = load_item_global_id();
    GS_ASSERT(err == SE_NONE);
}

void teardown()
{
    teardown_map_serializer();
    SerializerError err = write_item_global_id();
    GS_ASSERT(err == SE_NONE);
}

void check_save_state()
{
    check_map_save_state();
}

}   // serializer
