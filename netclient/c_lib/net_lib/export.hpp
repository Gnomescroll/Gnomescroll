#pragma once


typedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id);

void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt);

extern PY_MESSAGE_CALLBACK PY_MESSAGE_CALLBACK_GLOBAL;

/*
typedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id);
PY_MESSAGE_CALLBACK PY_MESSAGE_CALLBACK_GLOBAL = NULL;

void set_python_net_callback_function(void* pt)
{  
    PY_MESSAGE_CALLBACK_GLOBAL = (PY_MESSAGE_CALLBACK) pt; //may need *
}
*/