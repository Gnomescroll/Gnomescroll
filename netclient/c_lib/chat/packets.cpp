#include "packets.hpp"

#ifdef DC_CLIENT
inline void ChatMessage_StoC::handle()
{
    //printf("Client received: %s\n", msg);
    //printf("from: %d, channel=%d\n", sender, channel);

    if (sender != CHAT_SENDER_SYSTEM)
    {
        Agent_state *a = ClientState::agent_list->get(sender);
        if (a==NULL) return;
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
    //printf("Server received: %s\n", msg);
    //printf("from: %d, channel=%d\n", client_id, channel);

    Agent_state* a = NetServer::agents[client_id];
    if (a==NULL) return;

    // filter team channel here
    if (channel >= CHANNEL_ID_TEAM_OFFSET && channel < (int)(CHANNEL_ID_TEAM_OFFSET + N_TEAMS)
      && a->status.team-1 != channel-CHANNEL_ID_TEAM_OFFSET)
        return;

    msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
    chat_server->receive_message(channel, client_id, msg);
}

inline void ChatMessage_StoC::handle(){}
#endif
