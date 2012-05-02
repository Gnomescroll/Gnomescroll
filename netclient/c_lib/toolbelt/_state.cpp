#include "_state.hpp"

namespace Toolbelt
{

//Common
bool* agent_fire_on = NULL;
int* agent_fire_tick = NULL;
int* agent_selected_type = NULL;

//Client
#if DC_CLIENT
int selected_slot = 0;
int toolbelt_id = NULL_CONTAINER;
#endif

//Server
#if DC_SERVER
int* agent_selected_slot = NULL;
ItemID* agent_selected_item = NULL;
#endif

}
