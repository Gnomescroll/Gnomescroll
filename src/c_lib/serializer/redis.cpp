#include "redis.hpp"

#include <hiredis/hiredis.h>
#include <hiredis/adapters/libev.h>

namespace serializer
{

void getCallback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = (redisReply*) r;
    if (reply == NULL) return;
    printf("argv[%s]: %s\n", (char*)privdata, reply->str);

    /* Disconnect after receiving the reply to GET */
    redisAsyncDisconnect(c);
}

void connectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK)
    {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK)
    {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
}

void init_redis()
{
    signal(SIGPIPE, SIG_IGN);

    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err)
    {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
    }

    redisLibevAttach(EV_DEFAULT_ c);
    redisAsyncSetConnectCallback(c,connectCallback);
    redisAsyncSetDisconnectCallback(c,disconnectCallback);
    redisAsyncCommand(c, NULL, NULL, "SET key %b", "crack", strlen("crack"));
    redisAsyncCommand(c, getCallback, (char*)"end-1", "GET key");
    ev_loop(EV_DEFAULT_ 0);
}



}   // serializer
