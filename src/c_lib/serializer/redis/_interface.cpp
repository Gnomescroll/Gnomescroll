#include "_interface.hpp"

#include <ev.h>
#include <hiredis/adapters/libev.h>

namespace serializer
{
namespace redis
{

// TODO -- enum label in the *note field to indicate request type, handle more specifically from that
// TODO -- wrap higher level transactions
// TODO -- pipelining commands? MULTI/EXEC? scripting?

static redisAsyncContext* ctx = NULL;
static bool waiting_to_connect = false; 
static bool waiting_to_disconnect = false; 
bool connected = false;

static void handle_reply(redisReply* reply)
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
                handle_reply(reply->element[i]);
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
    handle_reply(reply);
    if (note != NULL) printf("Note: %s\n", (char*)note);
}

void connectCallback(const redisAsyncContext *ctx, int status)
{
    waiting_to_connect = false;
    if (status == REDIS_OK)
    {
        printf("Redis connected...\n");
        connected = true;
    }
    else
    {
        printf("Redis connect error: %s\n", ctx->errstr);
        connected = false;
    }
}

void disconnectCallback(const redisAsyncContext *ctx, int status)
{
    printf("Redis disconnected...\n");
    if (status != REDIS_OK)
        printf("Redis disconnect error: %s\n", ctx->errstr);

    GS_ASSERT(connected);
    connected = false;

    GS_ASSERT(ctx != NULL);

    // the context will be freed by redis, we can clear ours
    ctx = NULL;

    waiting_to_disconnect = false;
}

void connect()
{
    GS_ASSERT(!connected);
    if (connected) return;
    
    // CONNECT NEW
    waiting_to_connect = true;
    ctx = redisAsyncConnect("127.0.0.1", 6379);
    GS_ASSERT(ctx != NULL);
    if (ctx == NULL)
    {
        waiting_to_connect = false;
        return;
    }
    GS_ASSERT(!ctx->err);
    if (ctx->err)
    {
        waiting_to_connect = false;
        redisAsyncFree(ctx);
        printf("Redis connect error: %s\n", ctx->errstr);
        return;
    }

    int ret = REDIS_OK;

    // BIND TO LIBEV
    ret = redisLibevAttach(EV_DEFAULT_ ctx);
    GS_ASSERT(ret == REDIS_OK);

    // SET UP CALLBACKS
    ret = redisAsyncSetConnectCallback(ctx, &connectCallback);
    GS_ASSERT(ret == REDIS_OK);
    ret = redisAsyncSetDisconnectCallback(ctx, &disconnectCallback);
    GS_ASSERT(ret == REDIS_OK);

    // SELECT DATABASE
    ret = redisAsyncCommand(ctx, NULL, NULL, "SELECT %d", Options::redis_database);
    GS_ASSERT(ret == REDIS_OK);

    // TEST COMMANDS
    //ret = redisAsyncCommand(ctx, NULL, NULL, "SET key %b", "crack", strlen("crack"));
    //GS_ASSERT(ret == REDIS_OK);
    //ret = redisAsyncCommand(ctx, &getCallback, NULL, "GET key");
    //GS_ASSERT(ret == REDIS_OK);
}

void disconnect()
{
    if (ctx == NULL) return;
    GS_ASSERT(connected);
    redisAsyncDisconnect(ctx);
}

void init()
{
    GS_ASSERT(ctx == NULL);
    if (ctx != NULL) return;
    
    signal(SIGPIPE, SIG_IGN);

    #if EV_MULTIPLICITY
    struct ev_loop* evl = ev_default_loop(EVFLAG_AUTO);
    GS_ASSERT(evl != NULL);
    #else
    int ret = ev_default_loop(EVFLAG_AUTO);
    GS_ASSERT(ret);
    #endif
    
    connect();
}

void teardown()
{
    if (!connected)
    {
        if (ctx != NULL)
            redisAsyncFree(ctx);
        return;
    }

    GS_ASSERT(!waiting_to_disconnect);
    waiting_to_disconnect = true;
    disconnect();

    // wait for disconnect response before shutting down
    while (waiting_to_disconnect)
    {
        gs_millisleep(10);
        ev_loop(EV_DEFAULT_ EVLOOP_NONBLOCK);
    }
}

void update()
{
    ev_loop(EV_DEFAULT_ EVLOOP_NONBLOCK);
    if (!connected && !waiting_to_connect)
        connect();
}

}   // redis
}   // serializer
