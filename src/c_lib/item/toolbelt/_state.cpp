#include "_state.hpp"

namespace Toolbelt
{

//Common
bool* agent_fire_on = NULL;
int* agent_fire_tick = NULL;
int* agent_selected_type = NULL;
int fist_item_type = NULL_ITEM_TYPE;

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

void init_state()
{
    GS_ASSERT(fist_item_type == NULL_ITEM_TYPE);
    fist_item_type = Item::get_item_type("fist");
    GS_ASSERT(fist_item_type != NULL_ITEM_TYPE);
    
    GS_ASSERT(agent_selected_type == NULL);
    GS_ASSERT(agent_fire_tick     == NULL);
    GS_ASSERT(agent_fire_on       == NULL);

    agent_selected_type = (int*)malloc(MAX_AGENTS * sizeof(int));
    for (int i=0; i<MAX_AGENTS; agent_selected_type[i++] = NULL_ITEM_TYPE);
    agent_fire_tick = (int*)calloc(MAX_AGENTS, sizeof(int));
    agent_fire_on   = (bool*)calloc(MAX_AGENTS, sizeof(bool));
    
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
    if (agent_selected_type != NULL) free(agent_selected_type);
    if (agent_fire_tick != NULL)     free(agent_fire_tick);
    if (agent_fire_on   != NULL)     free(agent_fire_on);
    agent_selected_type = NULL;
    agent_fire_tick = NULL;
    agent_fire_on = NULL;

    #if DC_SERVER
    if (agent_selected_slot != NULL) free(agent_selected_slot);
    if (agent_selected_item != NULL) free(agent_selected_item);
    agent_selected_slot = NULL;
    agent_selected_item = NULL;
    #endif
}

}    // Toolbelt
