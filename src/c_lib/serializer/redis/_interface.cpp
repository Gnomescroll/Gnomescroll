#include "_interface.hpp"

namespace serializer
{
namespace redis
{

void init()
{
    init_items();
    init_redis();
}

void teardown()
{
    teardown_redis();   // MUST COME FIRST -- all callbacks/data need to return results
    teardown_items();
}

void update()
{
    update_redis();
}

}   // redis
}   // serializer

