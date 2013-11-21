/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace Chat
{

typedef void (*chatCommand)(const char*, size_t, char*, size_t);

struct ChatCommand
{
    char name[COMMAND_MAX_LENGTH+1];
    chatCommand action;
};

extern size_t n_commands;
extern struct ChatCommand commands[MAX_COMMANDS];

void register_chat_commands();

}   // Chat
