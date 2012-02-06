#pragma once

const int CHAT_CLIENT_INPUT_HISTORY_MAX = 10;
const int CHAT_CLIENT_MESSAGE_HISTORY_MAX = 50;
const int CHAT_CLIENT_CHANNELS_MAX = 4; // pm, global, team, system
const int CHAT_MESSAGE_SIZE_MAX = 90;

class ChatMessage
{
    // payload
    // sender id, name
    // color
    // channel

    char payload[CHAT_MESSAGE_SIZE_MAX];
    int sender_id;
    int channel;
};

class ChatChannel
{
    // history
    ChatMessage** history;

    ChatChannel();
    ~ChatChannel();
};

class ChatInput
{
    char** history[CHAT_CLIENT_INPUT_HISTORY_MAX];
    char* buffer[CHAT_BUFFER_SIZE];
    int cursor;

    ChatInput();
    ~ChatInput();
};

class ChatClient
{

    // history
    // rendering

    // channels

    ChatChannel** channels;

    // input 
    //      history
    //      buffer
    //      cursor

    ChatInput input;

    ChatClient();
    ~ChatClient();

};

/* TODO

Use object list for chat messages
*
* Need fifo, circular queue, dbly linked list w/e for history
*/
