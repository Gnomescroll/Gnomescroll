#include "callbacks.hpp"

#include <item/toolbelt/common/types.hpp>

//#if DC_SERVER
//typedef void (*tickItem)    (int, ItemID, int); // agent_id, ItemID, item_type
//typedef void (*triggerItem) (int, ItemID, int); // agent_id, ItemID, item_type
//#endif

//#if DC_CLIENT
//typedef void (*tickItem)         (int, int); // agent_id, item_type
//typedef void (*tickLocalItem)    (int);      // item_type
//typedef void (*triggerItem)      (int, int); // agent_id, item_type
//typedef void (*triggerLocalItem) (int);      // item_type
//typedef void (*beginLocalItem)   (int);      // item_type
//typedef void (*endLocalItem)     (int);      // item_type
//#endif


#if DC_SERVER
#define CONLY(body)
#endif

#if DC_CLIENT
#define CONLY(body) body
#endif

namespace Toolbelt
{

// no need to validate arguments for callbacks
// all args are prevalidated and processed


/* Mining laser */

void begin_mining_laser(int item_type)
{CONLY(
    printf("Begin mining laser\n");
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::playerAgent_state.action.begin_mining_laser();
)}

void end_mining_laser(int item_type)
{CONLY(
    GS_ASSERT(Item::get_item_group_for_type(item_type) == IG_MINING_LASER);
    ClientState::playerAgent_state.action.end_mining_laser();
)}

#undef CONLY

}    // Toolbelt
