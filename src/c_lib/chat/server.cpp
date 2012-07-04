#include "server.hpp"

#include <chat/packets.hpp>

/* ChatServerChannel */

ChatServerChannel::ChatServerChannel(int max)
:
n(0),
max(max)
{
    this->listeners = (int*)malloc(sizeof(int) * max);
    for (int i=0; i<max; this->listeners[i++] = -1);
}

ChatServerChannel::~ChatServerChannel()
{
    free(this->listeners);
}

void ChatServerChannel::broadcast(int sender, char* payload)
{
    if (strlen(payload) > (int)CHAT_MESSAGE_SIZE_MAX)
        payload[CHAT_MESSAGE_SIZE_MAX] = '\0';
    ChatMessage_StoC msg;
    strcpy(msg.msg, payload);
    msg.channel = this->id;
    msg.sender = sender;
    
    for (int i=0; i<max; i++)
    {
        if (listeners[i] == -1) continue;
        msg.sendToClient(listeners[i]);
    }
}

bool ChatServerChannel::add_listener(int id)
{
    if (n == max) return false;
    
    for (int i=0; i<max; i++)
        if (listeners[i] == id) return false;
    
    for (int i=0; i<max; i++)
    {
        if (listeners[i] != -1) continue;
        listeners[i] = id;
        n++;
        break;
    }
    return true;
}

bool ChatServerChannel::remove_listener(int id)
{
    if (n == 0) return false;
    for (int i=0; i<max; i++)
    {
        if (listeners[i] != id) continue;
        listeners[i] = -1;
        n--;
        break;
    }
    return true;
}

/* ChatServer */

void ChatServer::player_join(int id)
{
    system->add_listener(id);
    global->add_listener(id);
    pm[id]->add_listener(id);
}

void ChatServer::player_quit(int id)
{
    pm[id]->remove_listener(id);
    system->remove_listener(id);
    global->remove_listener(id);
}

void ChatServer::receive_message(int channel, int sender, char* payload)
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
		this->log_message(channel, sender, payload);	// logging
}

void ChatServer::log_message(int channel, int sender, char* payload)
{
	GS_ASSERT(Options::log_chat);
	if (!Options::log_chat) return;
	
	GS_ASSERT(this->log != NULL);
	if (log == NULL) return;

	GS_ASSERT(payload != NULL);
	if (payload == NULL) return;
	
	GS_ASSERT(channel >= 0 && channel < CHAT_SERVER_CHANNELS_MAX);
    if (channel < 0 || channel >= CHAT_SERVER_CHANNELS_MAX) return;

    if (!is_valid_chat_message(payload)) return;
		
	Agent_state* a = ServerState::agent_list->get(sender);
	GS_ASSERT(a != NULL); 
	if (a == NULL) return;
	char* sender_name = a->status.name;
	GS_ASSERT(sender_name != NULL);
	if (sender_name == NULL) return;
	GS_ASSERT(sender_name[0] != '\0');
	
	const char msg_fmt[] = "Channel: %d, Sender: %d %s; %s\n";
	int msg_len = strlen(msg_fmt) + strlen(payload)
		+ count_digits(channel) + count_digits(sender)
		+ strlen(sender_name) + 1 - 8;
	char* msg = (char*)malloc(msg_len * sizeof(char));
	
	msg_len = sprintf(msg, msg_fmt, channel, sender, sender_name, payload);
	fwrite(msg, sizeof(char), msg_len, this->log);
}

ChatServer::ChatServer()
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
		this->log = fopen("./log/chat.log", "a");
		GS_ASSERT(this->log != NULL);
        if (this->log != NULL)
            setvbuf(this->log, NULL, _IOLBF, 256);
	}
	else
		this->log = NULL;
}

ChatServer::~ChatServer()
{
    delete system;
    delete global;

    for (int i=0; i<PLAYERS_MAX; delete pm[i++]);
    free(this->pm);
}
