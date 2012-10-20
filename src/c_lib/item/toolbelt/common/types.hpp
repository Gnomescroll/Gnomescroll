#pragma once

namespace Toolbelt
{

/* Trigger event function pointers */

#if DC_SERVER
typedef void (*tickItem)    (AgentID, ItemID, int); // agent_id, ItemID, item_type
typedef void (*triggerItem) (AgentID, ItemID, int); // agent_id, ItemID, item_type
#endif

#if DC_CLIENT
// others
typedef void (*tickItem)    (AgentID, int);    // agent_id, item_type
typedef void (*triggerItem) (AgentID, int);    // agent_id, item_type
typedef void (*beginItem)   (AgentID, int);         // agent_id, item_type
typedef void (*endItem)     (AgentID, int);           // agent_id, item_type

// locals
typedef void (*tickLocalItem)     (ItemID, int); // ItemID, item_type
typedef void (*triggerLocalItem) (ItemID, int); // ItemID, item_type
typedef void (*beginLocalItem)   (int);         // item_type
typedef void (*endLocalItem)     (int);         // item_type
#endif

}    // Toolbelt
