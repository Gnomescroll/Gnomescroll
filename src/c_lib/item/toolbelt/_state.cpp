#include "_state.hpp"

namespace Toolbelt
{

//Common
bool* agent_fire_on = NULL;
ItemType* agent_selected_type = NULL;
ItemType fist_item_type = NULL_ITEM_TYPE;
int* agent_fire_cooldown = NULL;

//Client
#if DC_CLIENT
bool holding = false;
int holding_tick = 0;
bool single_trigger = false;
int selected_slot = 0;
bool charging = false;
ItemContainerID toolbelt_id = NULL_CONTAINER;
#endif

//Server
#if DC_SERVER
int* agent_selected_slot = NULL;
ItemID* agent_selected_item = NULL;
#endif

void init_state()
{
    GS_ASSERT(fist_item_type == NULL_ITEM_TYPE);
    fist_item_type = Item::get_item_type("fist");
    GS_ASSERT(fist_item_type != NULL_ITEM_TYPE);

    GS_ASSERT(agent_fire_cooldown == NULL);
    GS_ASSERT(agent_selected_type == NULL);
    GS_ASSERT(agent_fire_on       == NULL);

    agent_selected_type = (ItemType*)malloc(MAX_AGENTS * sizeof(ItemType));
    for (int i=0; i<MAX_AGENTS; agent_selected_type[i++] = NULL_ITEM_TYPE);
    agent_fire_cooldown = (int*)calloc(MAX_AGENTS, sizeof(int));
    agent_fire_on = (bool*)calloc(MAX_AGENTS, sizeof(bool));

    #if DC_SERVER
    GS_ASSERT(agent_selected_slot == NULL);
    GS_ASSERT(agent_selected_item == NULL);
    agent_selected_slot = (int*)calloc(MAX_AGENTS, sizeof(int));
    agent_selected_item = (ItemID*)malloc(MAX_AGENTS * sizeof(ItemID));
    for (int i=0; i<MAX_AGENTS; agent_selected_item[i++] = NULL_ITEM);
    #endif
}

void teardown_state()
{
    free(agent_selected_type);
    free(agent_fire_on);
    free(agent_fire_cooldown);
    agent_selected_type = NULL;
    agent_fire_on = NULL;
    agent_fire_cooldown = NULL;

    #if DC_SERVER
    free(agent_selected_slot);
    free(agent_selected_item);
    agent_selected_slot = NULL;
    agent_selected_item = NULL;
    #endif
}

}    // Toolbelt
