#pragma once

#include <chat/globals.hpp>

#if DC_CLIENT
# include <chat/client.hpp>
#endif

#if DC_SERVER
# include <chat/server.hpp>
#endif

namespace Chat
{

void init_packets();

#if DC_CLIENT    
extern class ChatMessageList* chat_message_list;
extern class ChatClient* chat_client;
extern class ChatSystemMessage* system_message;

void init_chat_client();
void teardown_chat_client();

void send_system_message(const char* msg);
void send_system_messagef(const char* fmt, ...);
void subscribe_channels();
void use_global_channel();
#endif

#if DC_SERVER
extern class ChatServer* chat_server;

void init_chat_server();
void teardown_chat_server();

void add_player_to_chat(ClientID client_id);
void remove_player_from_chat(ClientID client_id);
#endif

}   // Chat
