#include "chat.hpp"

/* ChatChannel */

ChatChannel::ChatChannel()
{
    this->history = (ChatMessage*)malloc(sizeof(ChatMessage*)*CHAT_CLIENT_MESSAGE_HISTORY_MAX);
    for (int i=0; i<CHAT_CLIENT_MESSAGE_HISTORY_MAX; this->history[i++] = new ChatMessage());
}

ChatChannel::~ChatChannel()
{
    for (int i=0; i<CHAT_CLIENT_MESSAGE_HISTORY_MAX; delete this->history[i++]);
    free(this->history);
}


/* ChatInput */

ChatInput::ChatInput()
:
cursor(0)
{
    this->history = (char**)malloc(sizeof(char*) * CHAT_CLIENT_INPUT_HISTORY_MAX);
    for (int i=0; i<CHAT_CLIENT_INPUT_HISTORY_MAX; i++)
        this->history[i] = (char*)malloc(sizeof(char) * CHAT_BUFFER_SIZE);

    this->buffer = (char*)malloc(sizeof(char) * CHAT_BUFFER_SIZE);
}

ChatInput::~ChatInput()
{
    if (this->history != NULL)
    {
        for (int i=0; i<CHAT_CLIENT_INPUT_HISTORY_MAX; i++)
            free(this->history[i]);
        free(this->history);
    }
    if (this->buffer != NULL)
    {
        free(this->buffer);
    }
}



/* ChatClient */

ChatClient::ChatClient()
{
    this->channels = (ChatChannel**)malloc(sizeof(ChatChannel*)*CHAT_CLIENT_CHANNELS_MAX);
    for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; channels[i++] = new ChatChannel());
}

ChatClient::~ChatClient()
{
    if (this->channels != NULL)
    {
        for (int i=0; i<CHAT_CLIENT_CHANNELS_MAX; delete channels[i++]);
        free(this->channels);
    }
}

