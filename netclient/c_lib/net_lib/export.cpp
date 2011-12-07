#include "export.hpp"

//typedef void (*PY_MESSAGE_CALLBACK)(char* buff, int n, int client_id);
PY_MESSAGE_CALLBACK PY_MESSAGE_CALLBACK_GLOBAL = NULL;

void set_python_net_callback_function(PY_MESSAGE_CALLBACK pt)
{  
    PY_MESSAGE_CALLBACK_GLOBAL = pt;
}