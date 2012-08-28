#include "packets.hpp"

#include <chat/interface.hpp>

#ifdef DC_CLIENT
inline void ChatMessage_StoC::handle()
{
    if (sender != CHAT_SENDER_SYSTEM)
    {
        Agent_state *a = ClientState::agent_list->get(sender);
        if (a == NULL) return;
    }

    for (int i=0; i<CHAT_MESSAGE_SIZE_MAX; i++)
    {   // convert tabs to a space
        if (this->msg[i] == '\t')
            this->msg[i] = ' ';
    }

    msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
    chat_client->received_message(channel, sender, msg);
}

inline void ChatMessage_CtoS::handle(){}
#endif

#ifdef DC_SERVER

#include <chat/server.hpp>

inline void ChatMessage_CtoS::handle()
{
    Agent_state* a = NetServer::agents[client_id];
    if (a == NULL) return;

    msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
    chat_server->receive_message(channel, client_id, msg);
}

inline void ChatMessage_StoC::handle(){}
#endif
