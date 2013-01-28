#pragma once

namespace Toolbelt
{

/* Trigger event function pointers */

#if DC_SERVER
typedef void (*tickItem)    (AgentID, ItemID, ItemType);
typedef void (*triggerItem) (AgentID, ItemID, ItemType);
#endif

#if DC_CLIENT
// others
typedef void (*tickItem)    (AgentID, ItemType);
typedef void (*triggerItem) (AgentID, ItemType);
typedef void (*beginItem)   (AgentID, ItemType);
typedef void (*endItem)     (AgentID, ItemType);

// locals
typedef void (*tickLocalItem)     (ItemID, ItemType);
typedef void (*triggerLocalItem) (ItemID, ItemType);
typedef void (*beginLocalItem)   (ItemType);
typedef void (*endLocalItem)     (ItemType);
#endif

}    // Toolbelt
