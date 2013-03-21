//#include "client.hpp"
//
//#include <limits.h>
//#include <ctype.h>
//
//#include <common/logger.hpp>
//#include <auth/constants.hpp>
//#include <net_lib/client.hpp>
//
//namespace Chat
//{
//
///* TerminalInput */
//
//void TerminalInput::submit(int channel)
//{
//    if (!buffer_len) return;
//
//    bool was_cmd = route_command();
//    bool valid = is_valid_chat_message(this->buffer);
//
//    if (!was_cmd && valid)
//    {   // create, send packet
//        ChatMessage_CtoS msg;
//        strncpy(msg.msg, buffer, CHAT_MESSAGE_SIZE_MAX+1);
//        msg.msg[CHAT_MESSAGE_SIZE_MAX] = '\0';
//        msg.channel = channel;
//        msg.send();
//    }
//
//    // add to history
//    if (was_cmd || valid)
//        this->add_to_history(buffer);
//
//    // reset buffer
//    this->clear_buffer();
//}
//
//void TerminalInput::clear_buffer()
//{
//    buffer[0] = '\0';
//    buffer_len = 0;
//    cursor_x = 0;
//    history_index = -1;
//}
//
//void TerminalInput::add(char x)
//{
//    int n_chars = 1;
//    if (x == '\t')
//    {
//        n_chars = 4;
//        x = ' ';
//    }
//
//    for (int i=0; i<n_chars; i++)
//    {
//        if (cursor_x >= CHAT_BUFFER_SIZE) break;
//        if (buffer_len >= CHAT_BUFFER_SIZE) break;
//
//       for (int j=buffer_len; j>=cursor_x; j--)
//            buffer[j+1] = buffer[j];
//
//        buffer[cursor_x++] = x;
//        buffer[++buffer_len] = '\0';
//    }
//    buffer[buffer_len] = '\0';
//}
//
//void TerminalInput::remove()
//{
//    if (!buffer_len || !cursor_x) return;
//
//    int i=cursor_x;
//    char c;
//    while ((c = buffer[i++]) != '\0')
//        buffer[i-2] = c;
//
//    buffer[--buffer_len] = '\0';
//    cursor_x--;
//}
//
//void TerminalInput::cursor_x_left()
//{
//    cursor_x--;
//    cursor_x = (cursor_x < 0) ? 0 : cursor_x;
//}
//
//void TerminalInput::cursor_x_right()
//{
//    cursor_x++;
//    cursor_x = (cursor_x > buffer_len) ? buffer_len : cursor_x;
//}
//
////bool TerminalInput::route_command()
////{   // return false if the buffer doesn't start with command syntax; else return true (whether the command is valid/recognized or not)
////    if (this->buffer_len <= 1) return false;
////    if (this->buffer[0] != '/') return false;
////    if (isspace(this->buffer[1])) return false;
////
////    char cmd[CHAT_BUFFER_SIZE] = {'\0'};
////    size_t cmdlen = 0;
////    char c = '\0';
////    int i = 1;
////   // copy command to buffer and advance cursor_x
////    while ((c = this->buffer[i]) != '\0' && !isspace(c))
////    {
////        i++;
////        cmd[cmdlen++] = c;
////    }
////    cmd[cmdlen] = '\0';
////
////    // advance cursor_x to first non-space char (start of any arguments)
////    while ((c = this->buffer[i]) != '\0' && isspace(c))
////        i++;
////
////    char args[CHAT_BUFFER_SIZE] = {'\0'};
////    IF_ASSERT(this->buffer_len < i) return true;
////
////    size_t cmd_id = 0;
////    for (; cmd_id<n_commands; cmd_id++)
////        if (!strcmp(cmd, commands[cmd_id].name))
////        {
////            strcpy(args, &buffer[i]);
////            commands[cmd_id].action(cmd, cmdlen, args, this->buffer_len-i);
////            break;
////        }
////    if (cmd_id == n_commands)
////        chat_client->send_system_messagef("/%s is not a recognized command", cmd);
////
////    return true;
////}
//
//TerminalInput::TerminalInput() :
//    buffer_len(0),
//    cursor_x(0)
//{
//    this->buffer = (char*)calloc(CHAT_BUFFER_SIZE+1, sizeof(char));
//    this->buffer[CHAT_BUFFER_SIZE] = '\0';
//    this->buffer[0] = '\0';
//}
//
//TerminalInput::~TerminalInput()
//{
//    free(this->buffer);
//}
//
//}   // Chat
