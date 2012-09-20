#pragma once

#if DC_CLIENT
#include <chat/client.hpp>
    
extern class ChatMessageList* chat_message_list;
extern class ChatClient* chat_client;
extern class ChatSystemMessage* system_message;

void init_chat_client();
void teardown_chat_client();
#endif

#if DC_SERVER
#include <chat/server.hpp>

extern class ChatServer* chat_server;

void init_chat_server();
void teardown_chat_server();

void add_player_to_chat(int client_id);
void remove_player_from_chat(int client_id);
#endif
