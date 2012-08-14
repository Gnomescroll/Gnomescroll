#pragma once

namespace Toolbelt
{

/* Trigger event function pointers */

#if DC_SERVER
typedef void (*tickItem)    (int, ItemID, int); // agent_id, ItemID, item_type
typedef void (*triggerItem) (int, ItemID, int); // agent_id, ItemID, item_type
#endif

#if DC_CLIENT
// others
typedef void (*tickItem)         (int, int);    // agent_id, item_type
typedef void (*triggerItem)      (int, int);    // agent_id, item_type
typedef void (*beginItem)   (int, int);         // agent_id, item_type
typedef void (*endItem)   (int, int);           // agent_id, item_type

// locals
typedef void (*tickLocalItem)    (ItemID, int); // ItemID, item_type
typedef void (*triggerLocalItem) (ItemID, int); // ItemID, item_type
typedef void (*beginLocalItem)   (int);         // item_type
typedef void (*endLocalItem)     (int);         // item_type
#endif

}    // Toolbelt
