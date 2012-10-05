#pragma once

#include <hiredis/hiredis.h>
#include <hiredis/async.h>


namespace serializer
{

extern redisAsyncContext* ctx;
extern bool redis_connected; 

void init_redis();
void teardown_redis();
void update_redis();

}   // serializer
