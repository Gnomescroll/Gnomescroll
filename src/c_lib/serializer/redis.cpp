#include "redis.hpp"

#include <hiredis/adapters/libev.h>

namespace serializer
{

static redisAsyncContext* redis_ctx = NULL;
static bool waiting_to_connect = false; 
static bool waiting_to_disconnect = false; 
bool redis_connected = false;

static void handle_redis_reply(redisReply* reply)
{
    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            printf("Error reply: %s\n", reply->str);
            break;
            
        case REDIS_REPLY_STATUS:
            printf("Status reply: %s\n", reply->str);
            break;

        case REDIS_REPLY_INTEGER:
            printf("Reply (integer): %lld\n", reply->integer);
            break;

        case REDIS_REPLY_NIL:
            printf("Empty redis reply\n");
            break;

        case REDIS_REPLY_STRING:
            printf("Reply: %s\n", reply->str);
            break;

        case REDIS_REPLY_ARRAY:
            printf("Reply (multi-bulk): %d elements", reply->elements);
            for (unsigned int i=0; i<reply->elements; i++)
                handle_redis_reply(reply->element[i]);
            break;

        default:
            GS_ASSERT(false);
            break;
    }
}

void getCallback(redisAsyncContext* ctx, void* _reply, void* note)
{   // note is data send in the initial redisAsyncCommand
    redisReply* reply = (redisReply*) _reply;
    if (reply == NULL) return;
    handle_redis_reply(reply);
    if (note != NULL) printf("Note: %s\n", (char*)note);
}

void connectCallback(const redisAsyncContext *ctx, int status)
{
    waiting_to_connect = false;
    if (status == REDIS_OK)
    {
        printf("Redis connected...\n");
        redis_connected = true;
    }
    else
    {
        printf("Redis connect error: %s\n", ctx->errstr);
        redis_connected = false;
    }
}

void disconnectCallback(const redisAsyncContext *ctx, int status)
{
    printf("Redis disconnected...\n");
    if (status != REDIS_OK)
        printf("Redis disconnect error: %s\n", ctx->errstr);

    GS_ASSERT(redis_connected);
    redis_connected = false;

    GS_ASSERT(redis_ctx != NULL);

    // the context will be freed by redis, we can clear ours
    redis_ctx = NULL;

    waiting_to_disconnect = false;
}

void connect_redis()
{
    GS_ASSERT(!redis_connected);
    if (redis_connected) return;
    
    // CONNECT NEW
    waiting_to_connect = true;
    redis_ctx = redisAsyncConnect("127.0.0.1", 6379);
    GS_ASSERT(redis_ctx != NULL);
    if (redis_ctx == NULL)
    {
        waiting_to_connect = false;
        return;
    }
    GS_ASSERT(!redis_ctx->err);
    if (redis_ctx->err)
    {
        waiting_to_connect = false;
        redisAsyncFree(redis_ctx);
        printf("Redis connect error: %s\n", redis_ctx->errstr);
    }

    int ret = REDIS_OK;

    // BIND TO LIBEV
    ret = redisLibevAttach(EV_DEFAULT_ redis_ctx);
    GS_ASSERT(ret == REDIS_OK);

    // SET UP CALLBACKS
    ret = redisAsyncSetConnectCallback(redis_ctx, &connectCallback);
    GS_ASSERT(ret == REDIS_OK);
    ret = redisAsyncSetDisconnectCallback(redis_ctx, &disconnectCallback);
    GS_ASSERT(ret == REDIS_OK);

    // SELECT DATABASE
    ret = redisAsyncCommand(redis_ctx, NULL, NULL, "SELECT %d", Options::redis_database);
    GS_ASSERT(ret == REDIS_OK);

    // TEST COMMANDS
    //ret = redisAsyncCommand(redis_ctx, NULL, NULL, "SET key %b", "crack", strlen("crack"));
    //GS_ASSERT(ret == REDIS_OK);
    //ret = redisAsyncCommand(redis_ctx, &getCallback, NULL, "GET key");
    //GS_ASSERT(ret == REDIS_OK);
}

void disconnect_redis()
{
    if (redis_ctx == NULL) return;
    GS_ASSERT(redis_connected);
    redisAsyncDisconnect(redis_ctx);
}

void init_redis()
{
    GS_ASSERT(redis_ctx == NULL);
    if (redis_ctx != NULL) return;
    
    signal(SIGPIPE, SIG_IGN);

    struct ev_loop* evl = ev_default_loop(EVFLAG_AUTO);
    GS_ASSERT(evl != NULL);

    connect_redis();
}

void teardown_redis()
{
    if (!redis_connected)
    {
        if (redis_ctx != NULL)
            redisAsyncFree(redis_ctx);
        return;
    }
    GS_ASSERT(!waiting_to_disconnect);
    waiting_to_disconnect = true;
    disconnect_redis();

    while (waiting_to_disconnect)
    {
        gs_millisleep(10);
        ev_loop(EV_DEFAULT_ EVLOOP_NONBLOCK);
    }
}

void update_redis()
{
    ev_loop(EV_DEFAULT_ EVLOOP_NONBLOCK);
    if (!redis_connected && !waiting_to_connect)
        connect_redis();
}

}   // serializer
