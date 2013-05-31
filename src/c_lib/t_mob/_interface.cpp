#include "_interface.hpp"

namespace t_mob
{

void init()
{
    init_sprites();
    init_models();
}

void teardown()
{
    teardown_sprites();
    teardown_models();
}

}   // t_mob
