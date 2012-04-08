#include "interface.hpp"


/*
	Server Interface
*/

#ifdef DC_SERVER

/* globals */
ChatServer* chat_server = NULL;

void init_chat_server()
{
    chat_server = new ChatServer;
}

void teardown_chat_server()
{
    if (chat_server != NULL)
        delete chat_server;
}

void add_player_to_chat(int client_id)
{
    chat_server->player_join(client_id);
}

void remove_player_from_chat(int client_id)
{
    Agent_state* a = agent_list->get(client_id);
    if (a==NULL) {
        printf("WARNING ServerState::remove_player_from_chat -- id %d does not have an agent\n", client_id);
        return;
    }
    chat_server->player_quit(client_id, a->status.team);
}

#endif


/*
	Client Interface
*/

#ifdef DC_CLIENT

/* globals */
ChatClient* chat_client = NULL;
ChatMessageList* chat_message_list = NULL;
ChatSystemMessage* system_message = NULL;

void init_chat_client()
{
    chat_client = new ChatClient;
    chat_message_list = new ChatMessageList;
    system_message = new ChatSystemMessage;
}

void teardown_chat_client()
{
    if (chat_client != NULL)
    {
        chat_client->teardown();
        delete chat_client;
    }
    if (chat_message_list != NULL)
        delete chat_message_list;
    if (system_message != NULL)
        delete system_message;
}

#endif