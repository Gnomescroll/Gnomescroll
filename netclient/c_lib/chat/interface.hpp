#pragma once


/*
	Server Interface
*/

#ifdef DC_SERVER

#include <c_lib/chat/server.hpp>

/* globals */
extern ChatServer* chat_server;

void init_chat_server();
void teardown_chat_server();

void add_player_to_chat(int client_id);
void remove_player_from_chat(int client_id);

#endif

/*
	Client Interface
*/

#ifdef DC_CLIENT

#include <c_lib/chat/client.hpp>
	
extern ChatMessageList* chat_message_list;
extern ChatClient* chat_client;
extern ChatSystemMessage* system_message;

void init_chat_client();
void teardown_chat_client();

#endif