#include "_interface.hpp"

#include <serializer/map.hpp>

namespace serializer
{

void init()
{
    // make sure paths exist
    create_path("./world");
    create_path("./world/map");
    create_path("./world/players");
    create_path("./world/items");
    create_path("./world/containers");
    
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
