#include "server.hpp"

#include <chat/packets.hpp>

namespace Chat
{

void broadcast_server_message(const char* str)
{
    ChatMessage_StoC msg;
    strncpy(msg.msg, str, CHAT_MESSAGE_SIZE_MAX+1);
    msg.msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
    msg.sender = CHAT_SENDER_SYSTEM;
    msg.channel = CHAT_CHANNEL_SYSTEM;
    msg.broadcast();
}

/* ChatServerChannel */

ChatServerChannel::ChatServerChannel(int max) :
    n(0), max(max)
{
    this->listeners = (ClientID*)malloc(sizeof(ClientID) * max);
    for (int i=0; i<max; this->listeners[i++] = NULL_CLIENT);
}

ChatServerChannel::~ChatServerChannel()
{
    free(this->listeners);
}

void ChatServerChannel::broadcast(ClientID sender, char* payload)
{
    ChatMessage_StoC msg;
    strncpy(msg.msg, payload, CHAT_MESSAGE_SIZE_MAX+1);
    msg.msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
    msg.channel = this->id;
    msg.sender = sender;
    
    for (int i=0; i<max; i++)
        if (listeners[i] != NULL_CLIENT)
            msg.sendToClient(listeners[i]);
}

bool ChatServerChannel::add_listener(ClientID id)
{
    if (n == max) return false;
    
    for (int i=0; i<max; i++)
        if (listeners[i] == id) return false;
    
    for (int i=0; i<max; i++)
        if (listeners[i] == NULL_CLIENT)
        {
            listeners[i] = id;
            n++;
            break;
        }
    return true;
}

bool ChatServerChannel::remove_listener(ClientID id)
{
    if (n == 0) return false;
    for (int i=0; i<max; i++)
    {
        if (listeners[i] != id) continue;
        listeners[i] = NULL_CLIENT;
        n--;
        break;
    }
    return true;
}

/* ChatServer */

void ChatServer::player_join(ClientID id)
{
    system->add_listener(id);
    global->add_listener(id);
    pm[id]->add_listener(id);
}

void ChatServer::player_quit(ClientID id)
{
    pm[id]->remove_listener(id);
    system->remove_listener(id);
    global->remove_listener(id);
}

void ChatServer::receive_message(int channel, ClientID sender, char* payload)
{
    GS_ASSERT(payload != NULL);
    if (payload == NULL) return;
    
    GS_ASSERT(channel >= 0 && channel < CHAT_SERVER_CHANNELS_MAX);
    if (channel < 0 || channel >= CHAT_SERVER_CHANNELS_MAX) return;

    if (!is_valid_chat_message(payload)) return;

    if (channel == 0)
    {
        if (sender == CHAT_SENDER_SYSTEM)
            system->broadcast(sender, payload);
        else GS_ASSERT(false);
    }
    else if (channel == 1)
        global->broadcast(sender, payload);
    else if (channel >= CHANNEL_ID_AGENT_OFFSET && channel < CHANNEL_ID_AGENT_OFFSET + PLAYERS_MAX)
        pm[channel - CHANNEL_ID_AGENT_OFFSET]->broadcast(sender, payload);
    
    if (Options::log_chat)    
        this->log_message(channel, sender, payload);    // logging
}

void ChatServer::log_message(int channel, ClientID sender, char* payload)
{
    GS_ASSERT(Options::log_chat);
    if (!Options::log_chat) return;
    
    GS_ASSERT(this->logfile != NULL);
    if (logfile == NULL) return;

    GS_ASSERT(this->log_msg_buffer != NULL);
    if (this->log_msg_buffer == NULL) return;

    GS_ASSERT(this->log_msg_buffer_len > 0);
    if (this->log_msg_buffer_len <= 0) return;

    GS_ASSERT(payload != NULL);
    if (payload == NULL) return;
    
    GS_ASSERT(channel >= 0 && channel < CHAT_SERVER_CHANNELS_MAX);
    if (channel < 0 || channel >= CHAT_SERVER_CHANNELS_MAX) return;

    if (!is_valid_chat_message(payload)) return;

    AgentID agent_id = NetServer::get_agent_id_for_client(sender);
    Agents::Agent* a = Agents::get_agent(agent_id);
    GS_ASSERT(a != NULL); 
    if (a == NULL) return;
    char* sender_name = a->status.name;
    GS_ASSERT(sender_name != NULL);
    if (sender_name == NULL) return;
    GS_ASSERT(sender_name[0] != '\0');

    // logfile timestamp
    char* time_str = get_time_str();
    if (time_str != NULL)
        fwrite(time_str, sizeof(char), strlen(time_str), this->logfile);

    // logfile msg
    GS_ASSERT(strlen(CHAT_LOG_MSG_FORMAT) + strlen(payload) + strlen(sender_name) + count_digits(sender) < this->log_msg_buffer_len);
    int msg_len = snprintf(this->log_msg_buffer, this->log_msg_buffer_len,
                    CHAT_LOG_MSG_FORMAT, sender, sender_name, payload);
    GS_ASSERT(msg_len < (int)this->log_msg_buffer_len);
    fwrite(this->log_msg_buffer, sizeof(char), msg_len, this->logfile);
}

ChatServer::ChatServer()
: logfile(NULL), log_msg_buffer_len(0), log_msg_buffer(NULL)
{
    int channel_index = 0;
    system = new ChatServerChannel(PLAYERS_MAX);
    system->id = 0;
    channels[channel_index++] = 0;

    global = new ChatServerChannel(PLAYERS_MAX);
    global->id = 1;
    channels[channel_index++] = 1;

    pm = (ChatServerChannel**)malloc(sizeof(ChatServerChannel*)*PLAYERS_MAX);
    for (int i=0; i<PLAYERS_MAX; i++)
    {
        pm[i] = new ChatServerChannel(1);
        pm[i]->id = CHANNEL_ID_AGENT_OFFSET + i;
        channels[channel_index++] = pm[i]->id;
    }
    
    if (Options::log_chat)
    {
        this->logfile = fopen("./log/chat.log", "a");
        GS_ASSERT(this->logfile != NULL);
        if (this->logfile != NULL)
            setvbuf(this->logfile, NULL, _IOLBF, 256);

        this->log_msg_buffer_len = count_digits(HARD_MAX_CONNECTIONS)
        + strlen(CHAT_LOG_MSG_FORMAT) + CHAT_MESSAGE_SIZE_MAX + PLAYER_NAME_MAX_LENGTH + 1;
        this->log_msg_buffer = (char*)malloc(this->log_msg_buffer_len * sizeof(char));
    }
}

ChatServer::~ChatServer()
{
    delete system;
    delete global;

    for (int i=0; i<PLAYERS_MAX; delete pm[i++]);
    free(this->pm);

    if (this->log_msg_buffer != NULL) free(this->log_msg_buffer);
    if (this->logfile != NULL) fclose(this->logfile);
}

}   // Chat
