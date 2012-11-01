#include "client.hpp"

#include <limits.h>
#include <ctype.h>

#include <common/logger.hpp>
#include <auth/constants.hpp>
#include <net_lib/client.hpp>

/* ChatMessage */

void ChatMessage::set_name()
{
    if (sender == CHAT_SENDER_SYSTEM)
        strcpy(name, (char*)"System");
    else
    {
        AgentID agent_id = NetClient::get_agent_id_for_client(this->sender); 
        Agent* a = Agents::get_agent(agent_id);
        if (a==NULL || a->status.name[0] == '\0')
            strcpy(name, Auth::UNDEFINED_NAME);
        else
            strcpy(name, a->status.name);
    }
}

void ChatMessage::set_color()
{
    if (sender == CHAT_SENDER_SYSTEM) // system msg
        this->color = color_init(Options::system_message_r, Options::system_message_g, Options::system_message_b);
    else if (
        ClientState::playerAgent_state.agent_id >= 0
     && ClientState::playerAgent_state.agent_id + CHANNEL_ID_AGENT_OFFSET == channel
    )    // pm
        this->color = CHAT_PM_COLOR;
    else // global
    {
        AgentID agent_id = NetClient::get_agent_id_for_client(this->sender); 
        Agent *a = Agents::get_agent(agent_id);
        struct Color color = AGENT_DEFAULT_COLOR;
        if (a != NULL) color = a->status.color;
        this->color = color;
    }
}

ChatMessage::ChatMessage(int id) :
    id(id), timestamp(0), color(CHAT_UNKNOWN_COLOR)
{
    this->payload = (char*)calloc(CHAT_MESSAGE_SIZE_MAX+1, sizeof(char));
    this->name = (char*)calloc(PLAYER_NAME_MAX_LENGTH+1, sizeof(char));
}

/* ChatMessageHistoryObject */

ChatMessageHistoryObject::ChatMessageHistoryObject(ChatMessage* m) :
    m(m), next(NULL), prev(NULL)
{}

ChatMessageHistoryObject::~ChatMessageHistoryObject()
{
    chat_message_list->destroy(m->id);
}

/* ChatClientChannel */

void ChatClientChannel::add_message(ChatMessage* m)
{
    ChatMessageHistoryObject* in = new ChatMessageHistoryObject(m);
        
    if (history_size == CHAT_CLIENT_MESSAGE_HISTORY_MAX)
    {
        // remove back
        if (this->tail == NULL)
            printlog(Log::CHAT, Log::Warning, "tail is NULL but history full\n");
            
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

void ChatClientChannel::clear_history()
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
        printlog(Log::CHAT, Log::Warning, "freed %d but history_size=%d\n", freed, history_size);
    this->history = NULL;
}

ChatClientChannel::ChatClientChannel() :
    id(-1), history_size(0), history(NULL), tail(NULL)
{}

ChatClientChannel::~ChatClientChannel()
{
    this->clear_history();
}


/* ChatInput */

ChatInputHistoryObject::ChatInputHistoryObject(const char* m) :
    next(NULL), prev(NULL)
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

void ChatInput::add_to_history(const char *s)
{
    ChatInputHistoryObject* in = new ChatInputHistoryObject(s);

    if (history_size >= CHAT_CLIENT_INPUT_HISTORY_MAX)
    {   // remove tail
        if (this->history_tail == NULL)
            printlog(Log::CHAT, Log::Warning, "tail is NULL but history full\n");

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

    bool was_cmd = route_command();
    bool valid = is_valid_chat_message(this->buffer);

    if (!was_cmd && valid)
    {   // create, send packet
        ChatMessage_CtoS msg;
        strncpy(msg.msg, buffer, CHAT_MESSAGE_SIZE_MAX+1);
        msg.msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
        msg.channel = channel;
        msg.send();
    }
    
    // add to history
    if (was_cmd || valid)
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
    int n_chars = 1;
    if (x == '\t')
    {
        n_chars = 4;
        x = ' ';
    }

    for (int i=0; i<n_chars; i++)
    {
        if (cursor >= CHAT_BUFFER_SIZE) break;
        if (buffer_len >= CHAT_BUFFER_SIZE) break;

       for (int j=buffer_len; j>=cursor; j--)
            buffer[j+1] = buffer[j];

        buffer[cursor++] = x;
        buffer[++buffer_len] = '\0';
    }
    buffer[buffer_len] = '\0';
}

void ChatInput::remove()
{
    if (!buffer_len || !cursor) return;

    int i=cursor;
    char c;
    while ((c = buffer[i++]) != '\0')
        buffer[i-2] = c;
        
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
    if (history < 0 || history_index < 0) return;

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
    buffer_len = (int)strlen(buffer);
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
    buffer_len = (int)strlen(buffer);
    cursor = buffer_len;
}

bool ChatInput::route_command()
{
    if (!buffer_len) return false;
    if (this->buffer[0] != '/' || this->buffer[1] == '\0' || isspace(this->buffer[1]))
        return false;

    char cmd[CHAT_BUFFER_SIZE] = {'\0'};
    char c;
    int i=1;
    while((c = this->buffer[i++]) != '\0' && !isspace(c))   // advance cursor past command
        cmd[i-2] = c;

    if (i == 2) return false;

    while((c = buffer[i++]) != '\0' && isspace(c)); // advance cursor to first non-space char
    i -= 1;
    
    //if (!strcmp(cmd, (char*)"name") || !strcmp(cmd, (char*)"nick"))
    //{
        //if (buffer_len <= (int)(strlen((char*)"/name "))) return false;
        //char name[PLAYER_NAME_MAX_LENGTH+1] = {'\0'};
        //int j = 0;
        //while ((c = buffer[i++]) != '\0' && !isspace(c))
        //{
            //name[j++] = c;
            //if (j == (int)PLAYER_NAME_MAX_LENGTH) break;
        //}
        //if (j) ClientState::send_identify_packet(name);
        //return true;
    //}
    //else
    if (!strcmp(cmd, (char*)"kill") || !strcmp(cmd, (char*)"die"))
    {
        killme_CtoS msg;
        msg.send();
    }
    else
    if (!strcmp(cmd, (char*)"color") || !strcmp(cmd, (char*)"colour"))
    {
        struct Color color = color_init(0,0,0);
        bool valid = true;

        int start = i;  // save start cursor

        // split lines up
        int n = 1;
        const char delim = ' ';
        while ((c = buffer[i++]) != '\0')
        {
            if (!isdigit(c) && c != delim)
                break;
            if (c == delim)
            {
                buffer[i-1] = '\0';
                n++;
            }
        }
        
        if (c != '\0') valid = false;    // encountered invalid char
        if (n != 3) valid = false;       // invalid arg count
        
        if (!valid)
        {
            chat_client->send_system_message("Usage: /color R G B (R G B must be between 0 and 255)");
            return false;
        }
        
        char buf[3+1] = {'\0'};

        i = start;  // reset cursor
        
        int j = 0;  // copy red
        while ((c = buffer[i++]) != '\0' && j < 3)
            buf[j++] = c;
        buf[j] = '\0';
        if (buffer[i-1] != '\0') valid = false;
        else
        {
            int r = atoi(buf);
            if (r > 0xff) valid = false;
            else color.r = r;
        }
        
        j = 0;      // copy green
        while ((c = buffer[i++]) != '\0' && j < 3)
            buf[j++] = c;
        buf[j] = '\0';
        if (buffer[i-1] != '\0') valid = false;
        else
        {
            int g = atoi(buf);
            if (g > 0xff) valid = false;
            else color.g = g;
        }
                
        j = 0;      // copy blue
        while ((c = buffer[i++]) != '\0' && j < 3)
            buf[j++] = c;
        buf[j] = '\0';
        if (buffer[i-1] != '\0') valid = false;
        else
        {
            int b = atoi(buf);
            if (b > 0xff) valid = false;
            else color.b = b;
        }
        
        if (!valid)
        {
            chat_client->send_system_message("Usage: /color R G B (R G B must be between 0 and 255)");
            return false;
        }

        if (ClientState::playerAgent_state.you != NULL
         && colors_equal(ClientState::playerAgent_state.you->status.color, color))
        {
            static const char msgfmt[] = "Your color is already %d %d %d";
            static const size_t msg_len = sizeof(msgfmt) + 3*3 - 3*2;
            static char msg[msg_len+1] = {'\0'};

            snprintf(msg, msg_len+1, msgfmt, color.r, color.g, color.b);
            chat_client->send_system_message(msg);
        }
        
        colorme_CtoS msg;
        msg.color = color;
        msg.send();
    }
    else
    if (!strcmp(cmd, "url"))
    {
        Awesomium::open_url(&buffer[i]);
        return true;
    }

    return false;
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
    this->buffer = (char*)calloc(CHAT_BUFFER_SIZE+1, sizeof(char));
    this->buffer[CHAT_BUFFER_SIZE] = '\0';
    this->buffer[0] = '\0';
}

ChatInput::~ChatInput()
{
    this->clear_history();
    free(this->buffer);
}


/* ChatClient */

void ChatClient::received_message(int channel, ClientID sender, const char* payload)
{
    ChatClientChannel* chan = NULL;

    // get channel
    for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; i++)
    {
        chan = this->channels[i];
        if (chan == NULL)
            continue;
        if (chan->id == channel)
            break;
        chan = NULL;
    }

    GS_ASSERT(chan != NULL);
    if (chan == NULL)
    {
        printlog(Log::CHAT, Log::Always, "unknown message channel %d\n", channel);
        return;
    }

    // create message
    ChatMessage* m = chat_message_list->create();
    m->sender = sender;
    m->channel = channel;

    // copy payload
    strncpy(m->payload, payload, CHAT_MESSAGE_SIZE_MAX);

    // set properties
    m->set_color();
    m->set_name();
    m->timestamp = _GET_MS_TIME();
    
    chan->add_message(m);
}

void ChatClient::send_system_message(const char* msg)
{
    this->received_message(CHAT_CHANNEL_SYSTEM, CHAT_SENDER_SYSTEM, msg);
}

void ChatClient::subscribe_system_channel()
{
    if (this->channels[CHANNEL_SYSTEM] == NULL)
        this->channels[CHANNEL_SYSTEM] = new ChatClientChannel;

    this->channels[CHANNEL_SYSTEM]->type = CHANNEL_SYSTEM;
    this->channels[CHANNEL_SYSTEM]->id = 0;
}

void ChatClient::subscribe_channels()
{
    this->subscribe_system_channel();

    // call after playerAgent_state has been assigned by server
    if (ClientState::playerAgent_state.you == NULL)
        return;

    ChatClientChannel* chan;
    for (int i=1; i<CHAT_CLIENT_CHANNELS_MAX; i++)
    {
        if (this->channels[i] == NULL)
            this->channels[i] = new ChatClientChannel;

        chan = this->channels[i];
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
            default:
                GS_ASSERT(false);
                break;
        }
    }
}

void ChatClient::submit()
{
    this->input->submit(this->channel);
}

void ChatClient::teardown()
{
    this->input->clear_buffer();
    this->input->clear_history();
    for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; i++)
        if (this->channels[i] != NULL)
            this->channels[i]->clear_history();
}

void ChatClient::use_global_channel()
{
    this->channel = 1;
}

ChatClient::ChatClient()
:
channel(1)
{
    this->channels = (ChatClientChannel**)malloc(sizeof(ChatClientChannel*)*CHAT_CLIENT_CHANNELS_MAX);
    for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; channels[i++] = NULL);
    this->subscribe_system_channel();
    
    this->input = new ChatInput;
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

    if (this->input != NULL)
        delete this->input;
}

/* ChatMessageList */

void ChatMessageList::quicksort_timestamp_desc(int beg, int end)
{
    if (end > beg + 1)
    {
        int timestamp;
        if (this->filtered_objects[beg] == NULL)
            timestamp = INT_MIN;
        else
            timestamp = this->filtered_objects[beg]->timestamp;
        int l = beg + 1, r = end;
        while (l < r)
        {
            int t;
            t = (this->filtered_objects[l] == NULL) ? INT_MIN : this->filtered_objects[l]->timestamp;
            if (t >= timestamp)
                l++;
            else
                swap_object_state(&this->filtered_objects[l], &this->filtered_objects[--r]);
        }
        swap_object_state(&this->filtered_objects[--l], &this->filtered_objects[beg]);
        quicksort_timestamp_desc(beg, l);
        quicksort_timestamp_desc(r, end);
    }
}

void ChatMessageList::quicksort_timestamp_asc(int beg, int end)
{
    if (end > beg + 1)
    {
        int timestamp;
        if (this->filtered_objects[beg] == NULL)
            timestamp = INT_MAX;
        else
            timestamp = this->filtered_objects[beg]->timestamp;
        int l = beg + 1, r = end;
        while (l < r)
        {
            int t;
            t = (this->filtered_objects[l] == NULL) ? INT_MAX : this->filtered_objects[l]->timestamp;
            if (t <= timestamp)
                l++;
            else
                swap_object_state(&this->filtered_objects[l], &this->filtered_objects[--r]);
        }
        swap_object_state(&this->filtered_objects[--l], &this->filtered_objects[beg]);
        quicksort_timestamp_asc(beg, l);
        quicksort_timestamp_asc(r, end);
    }
}

void ChatMessageList::sort_by_most_recent()
{
    this->filter_none();
    this->quicksort_timestamp_desc(0, this->n_filtered);
}

void ChatMessageList::filter_none()
{   // moves all non null objects to the filtered list
    unsigned int c = 0;
    for (unsigned int i=0; i<this->max; i++)
        if (this->objects[i].id != this->null_id)
            this->filtered_objects[c++] = &this->objects[i];
    this->n_filtered = c;
}



/* ChatSystemMessage */
// collection of methods

namespace SystemMessage
{
    
char* spawner_created(Objects::Object* s)
{
    return NULL;
}

char* spawner_destroyed(Objects::Object* s)
{
    return NULL;
}

}   // SystemMessage

void ChatSystemMessage::object_destroyed(Objects::Object* object)
{
    //char* msg = NULL;
    //switch (object->state()->type)
    //{
        //case OBJECT_AGENT_SPAWNER:
            //msg = SystemMessage::spawner_destroyed((Spawner*)object);
            //break;
        //case OBJECT_TURRET:
            ////msg = SystemMessage::turret_destroyed((Turret*)object);
            //break;
        //default: return;
    //}
    //if (msg != NULL)
    //{
        //chat_client->send_system_message(msg);
        //free(msg);
    //}
}

void ChatSystemMessage::object_created(Objects::Object* object)
{
    //char* msg = NULL;
    //switch (object->state()->type)
    //{
        //case OBJECT_AGENT_SPAWNER:
            //msg = SystemMessage::spawner_created((Spawner*)object);
            //break;
        //case OBJECT_TURRET:
            ////msg = SystemMessage::turret_created((Turret*)object);
            //break;
        //default: return;
    //}
    //if (msg != NULL)
    //{
        //chat_client->send_system_message(msg);
        //free(msg);
    //}
}
