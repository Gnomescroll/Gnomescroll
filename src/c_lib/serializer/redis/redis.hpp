#pragma once

#include <hiredis/hiredis.h>
#include <hiredis/async.h>


namespace serializer
{
namespace redis
{

extern redisAsyncContext* ctx;
extern bool connected; 

void init_redis();
void teardown_redis();
void update_redis();

}   // redis
}   // serializer
