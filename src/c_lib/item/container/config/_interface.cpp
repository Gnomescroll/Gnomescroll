#include "_interface.hpp"

#include <item/common/constant.hpp>
#include <item/common/enum.hpp>
#include <item/container/config/_state.hpp>

#if DC_CLIENT
#include <item/container/client.hpp>
#endif

#if DC_SERVER
#include <item/container/server.hpp>

// packet send stubs
// client has these defined and we want them in config
// but dont want to have to split the config up for client/server
void send_container_alpha_action(ContainerActionType action, int container_id, int slot){}
void send_container_beta_action(ContainerActionType action, int container_id, int slot){}
void send_synthesizer_alpha_action(ContainerActionType action, int container_id, int slot){}
void send_synthesizer_beta_action(ContainerActionType action, int container_id, int slot){}
void send_craft_alpha_action(ContainerActionType action, int container_id, int slot){}
void send_purchase_item_action(ContainerActionType action, int container_id, int slot){}
void send_craft_beta_action(ContainerActionType action, int container_id, int slot){}
void send_craft_item_action(ContainerActionType action, int container_id, int slot){}
void send_no_container_alpha_action(ContainerActionType action, int container_id, int slot){}
void send_no_container_beta_action(ContainerActionType action, int container_id, int slot){}
void send_smelter_alpha_action(ContainerActionType action, int container_id, int slot){}
void send_smelter_beta_action(ContainerActionType action, int container_id, int slot){}
void send_recycler_alpha_action(ContainerActionType action, int container_id, int slot){}
void send_recycler_beta_action(ContainerActionType action, int container_id, int slot){}
void send_recycler_crush_action(ContainerActionType action, int container_id){}

#endif

namespace ItemContainer
{

/* Configuration Loader */ 
    
static int _started = 0;
static class ContainerAttributes c;

void add_container(class ContainerAttributes c)
{
    GS_ASSERT(container_attributes != NULL);
    if (container_attributes == NULL) return;
    c.loaded = true;
    container_attributes[c.type] = c;
    c.init();
}

void container_def(ItemContainerType type)
{
    if (_started++) add_container(c);
    c.type = type;
}
        
static void register_settings()
{
    container_def(CONTAINER_TYPE_NONE);
    c.xdim = 0;
    c.ydim = 0;
    c.attached_to_agent = false;
    c.alpha_action = &no_container_alpha_action_decision_tree;
    c.beta_action = &no_container_beta_action_decision_tree;
    c.alpha_packet = &send_no_container_alpha_action;
    c.beta_packet = &send_no_container_beta_action;

    container_def(AGENT_CONTAINER);
    c.xdim = 6;
    c.ydim = 3;
    c.attached_to_agent = true;
    c.alpha_action = &alpha_action_decision_tree;
    c.beta_action = &beta_action_decision_tree;
    c.alpha_packet = &send_container_alpha_action;
    c.beta_packet = &send_container_beta_action;

    container_def(AGENT_TOOLBELT);
    c.xdim = 9;
    c.ydim = 1;
    c.attached_to_agent = true;
    c.alpha_action = &alpha_action_decision_tree;
    c.beta_action = &beta_action_decision_tree;
    c.alpha_packet = &send_container_alpha_action;
    c.beta_packet = &send_container_beta_action;

    container_def(AGENT_SYNTHESIZER);
    c.xdim = 1;
    c.ydim = 1;
    c.alt_xdim = 5;
    c.alt_ydim = 3;
    c.attached_to_agent = true;
    c.alpha_action = &synthesizer_alpha_action_decision_tree;
    c.beta_action = &synthesizer_beta_action_decision_tree;
    c.alpha_packet = &send_synthesizer_alpha_action;
    c.beta_packet = &send_synthesizer_beta_action;
    c.alpha_action_alt = &synthesizer_shopping_alpha_action_decision_tree;
    c.beta_action_alt = &synthesizer_shopping_beta_action_decision_tree;
    c.alpha_packet_alt = &send_purchase_item_action;
    c.beta_packet_alt = &send_purchase_item_action;

    container_def(AGENT_ENERGY_TANKS);
    c.xdim = 4;
    c.ydim = 1;
    c.attached_to_agent = true;
    c.alpha_action = &alpha_action_decision_tree;
    c.beta_action = &beta_action_decision_tree;
    c.alpha_packet = &send_container_alpha_action;
    c.beta_packet = &send_container_beta_action;
    
    container_def(CONTAINER_TYPE_STORAGE_BLOCK_SMALL);
    c.xdim = 3;
    c.ydim = 3;
    c.attached_to_agent = false;
    c.alpha_action = &alpha_action_decision_tree;
    c.beta_action = &beta_action_decision_tree;
    c.alpha_packet = &send_container_alpha_action;
    c.beta_packet = &send_container_beta_action;

    container_def(CONTAINER_TYPE_CRAFTING_BENCH_UTILITY);
    c.xdim = 4;
    c.ydim = 1;
    c.alt_xdim = 1;
    c.alt_ydim = 3;
    c.attached_to_agent = false;
    c.alpha_action = &craft_input_alpha_action_decision_tree;
    c.beta_action = &craft_input_beta_action_decision_tree;
    c.alpha_packet = &send_craft_alpha_action;
    c.beta_packet = &send_craft_beta_action;
    c.alpha_action_alt = &craft_output_alpha_action_decision_tree;
    c.beta_action_alt = &craft_output_beta_action_decision_tree;
    c.alpha_packet_alt = &send_craft_item_action;
    c.beta_packet_alt = &send_craft_item_action;

    container_def(CONTAINER_TYPE_CRYOFREEZER_SMALL);
    c.xdim = 2;
    c.ydim = 2;
    c.attached_to_agent = false;
    c.alpha_action = &alpha_action_decision_tree;
    c.beta_action = &beta_action_decision_tree;
    c.alpha_packet = &send_container_alpha_action;
    c.beta_packet = &send_container_beta_action;

    container_def(CONTAINER_TYPE_SMELTER_ONE);
    c.xdim = 1;
    c.ydim = 1;
    c.alt_xdim = 1;
    c.alt_ydim = 1;
    c.attached_to_agent = false;
    c.alpha_action = &smelter_alpha_action_decision_tree;
    c.beta_action = &smelter_beta_action_decision_tree;
    c.alpha_packet = &send_smelter_alpha_action;
    c.beta_packet = &send_smelter_beta_action;

    container_def(CONTAINER_TYPE_RECYCLER);
    c.xdim = 1;
    c.ydim = 1;
    c.alt_xdim = 1;
    c.alt_ydim = 1;
    c.attached_to_agent = false;
    c.alpha_action = &recycler_alpha_action_decision_tree;
    c.beta_action = &recycler_beta_action_decision_tree;
    c.alpha_packet = &send_recycler_alpha_action;
    c.beta_packet = &send_recycler_beta_action;
  
    add_container(c);   // finalize
}
    
static void validate_settings()
{
    int n_none = 0;
    for (int i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        class ContainerAttributes* c = &container_attributes[i];
        if (!c->loaded) continue;
        if (c->type == CONTAINER_TYPE_NONE)
        {
            n_none++;
            continue;
        }
        GS_ASSERT(c->xdim > 0);
        GS_ASSERT(c->ydim > 0);
        GS_ASSERT(c->alpha_action != NULL);
        GS_ASSERT(c->beta_action != NULL);
        #if DC_CLIENT
        GS_ASSERT(c->alpha_packet != NULL);
        GS_ASSERT(c->beta_packet != NULL);
        #endif
    }
    GS_ASSERT(n_none <= 1);
}

void init_config()
{
    GS_ASSERT(container_attributes == NULL);
    container_attributes = new ContainerAttributes[MAX_CONTAINER_TYPES];
    register_settings();
    validate_settings();
}

void teardown_config()
{
    if (container_attributes != NULL) delete[] container_attributes;
}

/* Public Attribute Accessors */

class ContainerAttributes* get_attr(ItemContainerType type)
{
    GS_ASSERT(type >= 0 && type < MAX_CONTAINER_TYPES);
    if (type < 0 || type >= MAX_CONTAINER_TYPES) return NULL;
    return &container_attributes[type];
}

int get_container_max_slots(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return 0;
    return attr->max_dim();
}

int get_container_xdim(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return 0;
    return attr->xdim;
}

int get_container_ydim(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return 0;
    return attr->ydim;
}

int get_container_alt_max_slots(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return 0;
    return attr->max_alt_dim();
}

int get_container_alt_xdim(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return 0;
    return attr->alt_xdim;
}

int get_container_alt_ydim(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return 0;
    return attr->alt_ydim;
}

bool container_type_is_attached_to_agent(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return false;
    return !attr->attached_to_agent;
}

bool container_type_is_block(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return false;
    return !attr->attached_to_agent;
}
    
}   // ItemContainer
