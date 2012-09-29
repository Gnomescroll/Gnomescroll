#include "_interface.hpp"

#include <serializer/map.hpp>

namespace serializer
{

void init()
{
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
