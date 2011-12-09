#pragma once


typedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id);
void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt);
extern PY_MESSAGE_CALLBACK PY_MESSAGE_CALLBACK_GLOBAL;

typedef void (*PY_CLIENT_EVENT_CALLBACK)(int client_id, int event_type);
void set_python_net_event_callback_function(PY_CLIENT_EVENT_CALLBACK pt);
extern PY_CLIENT_EVENT_CALLBACK PY_CLIENT_EVENT_CALLBACK_GLOBAL;

void client_connect_event(int client_id);
void client_disconnect_event(int client_id);

void send_python_net_message(char* message, int size, int client_id);
int _get_client_id();
int _check_connection_status();
int _check_connection_status(int client_id);