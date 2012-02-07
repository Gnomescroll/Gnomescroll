#include "client.hpp"

/* ChatMessageHistoryObject */

ChatMessageHistoryObject::ChatMessageHistoryObject(ChatMessage* m)
:
m(m),
next(NULL),
prev(NULL)
{}

/* ChatClientChannel */

void ChatClientChannel::add_message(ChatMessage* m)
{
    ChatMessageHistoryObject* in = new ChatMessageHistoryObject(m);
        
    if (history_size == CHAT_CLIENT_MESSAGE_HISTORY_MAX)
    {
        // remove back
        if (this->tail == NULL)
            printf("WARNING ChatInput::add_message -- tail is NULL but history full\n");
            
        ChatMessageHistoryObject* delete_me = this->tail;
        this->tail = delete_me->next;
        this->tail->prev = NULL;
        delete delete_me;
        history_size--;
    }

    // append to front
    if (this->history == NULL)
        this->history = in;
    else
    {
        in->prev = this->history;
        this->history->next = in;
        if (this->tail == NULL)
            this->tail = this->history;
        this->history = in;
    }
    history_size++;
}

ChatClientChannel::ChatClientChannel()
:
id(-1),
history_size(0),
history(NULL),
tail(NULL)
{}

ChatClientChannel::~ChatClientChannel()
{
    if (history == NULL) return;
    int freed = 0;

    while (history != NULL)
    {
        ChatMessageHistoryObject* delete_me = history;
        history = delete_me->prev;
        delete delete_me;
        freed++;
    }
    if (freed != history_size)
        printf("WARNING ~ChatClientChannel -- freed %d but history_size=%d\n", freed, history_size);
}


/* ChatInput */

ChatInputHistoryObject::ChatInputHistoryObject(char* m)
:
next(NULL),
prev(NULL)
{
    this->m = (char*)malloc(sizeof(char) * (strlen(m)+1));
    strcpy(this->m, m);
}

ChatInputHistoryObject::~ChatInputHistoryObject()
{
    free(this->m);
}


void ChatInput::clear_history()
{
    while (history != NULL)
    {
        ChatInputHistoryObject* delete_me = history;
        history = delete_me->prev;
        delete delete_me;
    }
    history = NULL;
    history_tail = NULL;
    history_size = 0;
}

void ChatInput::add_to_history(char *s)
{
    ChatInputHistoryObject* in = new ChatInputHistoryObject(s);

    if (history_size >= CHAT_CLIENT_INPUT_HISTORY_MAX)
    {   // remove tail
        if (this->history_tail == NULL)
            printf("WARNING ChatInput::add_to_history -- tail is NULL but history full\n");

        ChatInputHistoryObject* delete_me = this->history_tail;
        this->history_tail = delete_me->next;
        this->history_tail->prev = NULL;
        delete delete_me;
        history_size--;
    }

    // add to front
    if (this->history == NULL)
        this->history = in;
    else
    {
        in->prev = this->history;
        this->history->next = in;
        if (this->history_tail == NULL)
            this->history_tail = in;
        this->history = in;
    }
    history_size++;
}

void ChatInput::submit(int channel)
{
    if (!buffer_len) return;

    // create, send packet
    ChatMessage_CtoS msg;
    strcpy(msg.msg, buffer);
    msg.channel = channel;
    msg.send();
    
    // add to history
    this->add_to_history(buffer);

    // reset buffer
    this->clear_buffer();
}

void ChatInput::clear_buffer()
{
    buffer[0] = '\0';
    buffer_len = 0;
    cursor = 0;
    history_index = -1;
}

void ChatInput::add(char x)
{
    if (cursor == CHAT_BUFFER_SIZE) return;
    if (buffer_len == CHAT_BUFFER_SIZE) return;
    for (int i=cursor; i<CHAT_BUFFER_SIZE-1; i++)
        buffer[i+1] = buffer[i];
    buffer[cursor++] = x;
    buffer_len++;
}

void ChatInput::remove()
{
    if (!buffer_len || !cursor) return;
    int i;
    for (i=cursor; i<CHAT_BUFFER_SIZE; i++)
        buffer[i] = buffer[i+1];
    buffer[--buffer_len] = '\0';
    cursor--;
}

void ChatInput::cursor_left()
{
    cursor--;
    cursor = (cursor < 0) ? 0 : cursor;
}

void ChatInput::cursor_right()
{
    cursor++;
    cursor = (cursor > buffer_len) ? buffer_len : cursor;
}

void ChatInput::history_newer()
{
    if (history < 0) return;
    history_index--;
    history_index = (history_index < -1) ? -1 : history_index;

    if (history_index < 0)
    {
        clear_buffer();
        return;
    }

    ChatInputHistoryObject* o = history;
    for (int i=0; i<history_index; i++)
    {
        o = o->prev;
    }
    strcpy(buffer, o->m);
    buffer_len = strlen(buffer);
    cursor = buffer_len;
}

void ChatInput::history_older()
{
    if (history==NULL) return;
    if (!history_size) return;
    
    history_index++;
    history_index = (history_index > history_size-1) ? history_size-1 : history_index;

    ChatInputHistoryObject* o = history;
    for (int i=0; i<history_index; i++)
    {
        o = o->prev;
    }
    strcpy(buffer, o->m);
    buffer_len = strlen(buffer);
    cursor = buffer_len;
}

ChatInput::ChatInput()
:
history(NULL),
history_tail(NULL),
history_size(0),
history_index(-1),
buffer_len(0),
cursor(0)
{
    this->buffer = (char*)malloc(sizeof(char)*(CHAT_BUFFER_SIZE+1));
    this->buffer[CHAT_BUFFER_SIZE] = '\0';
    this->buffer[0] = '\0';
}

ChatInput::~ChatInput()
{
    this->clear_history();
    free(this->buffer);
}


/* ChatClient */

void ChatClient::add_message(ChatMessage* m)
{
    ChatClientChannel* chan = NULL;

    for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; i++)
    {
        if (this->channels[i]->id == m->channel)
        {
            chan  = this->channels[i];
            break;
        }
    }

    if (chan == NULL)
    {
        printf("ChatClient::add_message -- unknown message channel %d\n", m->channel);
    }

    chan->add_message(m);
}

void ChatClient::subscribe_channels()
{   // call after playerAgent_state has been assigned by server
    if (ClientState::playerAgent_state.you == NULL)
    {
        printf("ChatClient::init -- playerAgent_state.you is NULL, abort\n");
        return;
    }
    for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; i++)
    {
        ChatClientChannel* chan = new ChatClientChannel();
        chan->type = (ChannelTypes)i;
        switch (chan->type)
        {
            case CHANNEL_SYSTEM:
                chan->id = 0;
                break;
            case CHANNEL_GLOBAL:
                chan->id = 1;
                break;
            case CHANNEL_PRIVATE:
                chan->id = ClientState::playerAgent_state.agent_id + CHANNEL_ID_AGENT_OFFSET;
                break;
            case CHANNEL_TEAM:
                chan->id = ClientState::playerAgent_state.you->status.team + CHANNEL_ID_TEAM_OFFSET;
                break;
            default: break;
        }
        this->channels[i] = chan;
    }
}

void ChatClient::submit()
{
    this->input.submit(this->channel);
}

ChatClient::ChatClient()
:
channel(1)
{
    this->channels = (ChatClientChannel**)malloc(sizeof(ChatClientChannel*)*CHAT_CLIENT_CHANNELS_MAX);
    for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; channels[i++] = NULL);
}

ChatClient::~ChatClient()
{
    if (this->channels != NULL)
    {
        for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; i++)
        {
            if (this->channels[i] == NULL) continue;
            delete this->channels[i];
        }
        free(this->channels);
    }
}

ChatClient chat_client;

ChatMessageList chat_message_list;
