#include "_state.hpp"

namespace Toolbelt
{

//Common

//Client
#if DC_CLIENT
int selected_slot = 0;
int toolbelt_id = NULL_CONTAINER;
#endif

//Server
#if DC_SERVER
int* agent_selected_slot = NULL;
#endif

}
