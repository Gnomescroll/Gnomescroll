#pragma once


typedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id);

void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt);
extern PY_MESSAGE_CALLBACK PY_MESSAGE_CALLBACK_GLOBAL;

void send_python_net_message(char* message, int size, int client_id);
int _get_client_id();