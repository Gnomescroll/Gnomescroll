/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <hiredis/hiredis.h>
#include <hiredis/async.h>

namespace serializer
{

extern redisAsyncContext* ctx;
extern bool redis_connected;

void redis_get_cb(redisAsyncContext* ctx, void* _reply, void* note);

void init_redis();
void teardown_redis();
void update_redis();

void wait_for_redis_replies();
void received_redis_reply();

int send_redis_command(redisAsyncContext *ac, redisCallbackFn *fn, void *privdata, const char *format, ...);

}   // serializer
