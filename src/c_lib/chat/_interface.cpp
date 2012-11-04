#include "_interface.hpp"

namespace Chat
{

void init_packets()
{
    ChatMessage_CtoS::register_server_packet();
    ChatMessage_StoC::register_client_packet();
}

#if DC_CLIENT
ChatClient* chat_client = NULL;
ChatMessageList* chat_message_list = NULL;
ChatSystemMessage* system_message = NULL;

void send_system_message(const char* msg)
{
    GS_ASSERT(chat_client != NULL);
    if (chat_client == NULL) return;
    chat_client->send_system_message(msg);
}

void send_system_messagef(const char* fmt, ...)
{
    GS_ASSERT(chat_client != NULL);
    if (chat_client == NULL) return;
    va_list args;
    va_start(args, fmt);
    chat_client->send_system_messagevf(fmt, args);
    va_end(args);
}

void subscribe_channels()
{
    GS_ASSERT(chat_client != NULL);
    if (chat_client == NULL) return;
    chat_client->subscribe_channels();
}

void use_global_channel()
{
    GS_ASSERT(chat_client != NULL);
    if (chat_client == NULL) return;
    chat_client->use_global_channel();
}

void init_chat_client()
{
    GS_ASSERT(chat_client == NULL);
    GS_ASSERT(chat_message_list == NULL);
    GS_ASSERT(system_message == NULL);
    chat_client = new ChatClient;
    chat_message_list = new ChatMessageList((CHAT_CLIENT_MESSAGE_HISTORY_MAX+1)*CHAT_CLIENT_CHANNELS_MAX);
    system_message = new ChatSystemMessage;
}

void teardown_chat_client()
{
    if (chat_client != NULL) delete chat_client;
    if (chat_message_list != NULL) delete chat_message_list;
    if (system_message != NULL) delete system_message;
}
#endif

#if DC_SERVER
ChatServer* chat_server = NULL;

void init_chat_server()
{
    GS_ASSERT(chat_server == NULL);
    chat_server = new ChatServer;
}

void teardown_chat_server()
{
    if (chat_server != NULL) delete chat_server;
}

void add_player_to_chat(ClientID client_id)
{
    chat_server->player_join(client_id);
}

void remove_player_from_chat(ClientID client_id)
{
    chat_server->player_quit(client_id);
}
#endif

}   // Chat
