#include "_interface.hpp"

#include <item/common/constants.hpp>
#include <item/common/enum.hpp>
#include <item/container/config/_state.hpp>

#if DC_CLIENT
#include <item/container/client.hpp>
#endif

#if DC_SERVER
#include <item/container/config/crusher_dat.hpp>
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
void send_crusher_alpha_action(ContainerActionType action, int container_id, int slot){}
void send_crusher_beta_action(ContainerActionType action, int container_id, int slot){}
void send_crusher_crush_action(ContainerActionType action, int container_id, int slot){}
#endif

namespace ItemContainer
{

/* Configuration Loader */ 

static class ContainerAttributes* c = NULL;

void add_container(class ContainerAttributes* c)
{
    GS_ASSERT(container_attributes != NULL);
    if (container_attributes == NULL) return;
    c->loaded = true;
    container_attributes[c->type] = c;
}

void container_def(ItemContainerType type, const char* name)
{
    if (c != NULL) add_container(c);
    c = new ContainerAttributes;
    c->type = type;
    c->set_name(name);
}
        
static void register_settings()
{
    GS_ASSERT(c == NULL);
    
    container_def(CONTAINER_TYPE_NONE, "none");
    c->xdim = 0;
    c->ydim = 0;
    c->attached_to_agent = false;
    c->alpha_action = &no_container_alpha_action_decision_tree;
    c->beta_action = &no_container_beta_action_decision_tree;
    c->alpha_packet = &send_no_container_alpha_action;
    c->beta_packet = &send_no_container_beta_action;

    container_def(AGENT_HAND, "hand");
    c->xdim = 1;
    c->ydim = 1;
    c->attached_to_agent = true;
    // there are no alpha/beta actions for hand; it has a special role in the manipulation of inventory

    container_def(AGENT_INVENTORY, "inventory");
    c->xdim = 6;
    c->ydim = 3;
    c->attached_to_agent = true;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;

    container_def(AGENT_TOOLBELT, "toolbelt");
    c->xdim = 9;
    c->ydim = 1;
    c->attached_to_agent = true;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;

    container_def(AGENT_SYNTHESIZER, "synthesizer");
    c->xdim = 1;
    c->ydim = 1;
    c->alt_xdim = 5;
    c->alt_ydim = 3;
    c->attached_to_agent = true;
    c->alpha_action = &synthesizer_alpha_action_decision_tree;
    c->beta_action = &synthesizer_beta_action_decision_tree;
    c->alpha_packet = &send_synthesizer_alpha_action;
    c->beta_packet = &send_synthesizer_beta_action;
    c->alpha_action_alt = &synthesizer_shopping_alpha_action_decision_tree;
    c->beta_action_alt = &synthesizer_shopping_beta_action_decision_tree;
    c->alpha_packet_alt = &send_purchase_item_action;
    c->beta_packet_alt = &send_purchase_item_action;

    container_def(AGENT_ENERGY_TANKS, "energy_tanks");
    c->xdim = 4;
    c->ydim = 1;
    c->attached_to_agent = true;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    
    container_def(CONTAINER_TYPE_STORAGE_BLOCK_SMALL, "storage_block_small");
    c->xdim = 3;
    c->ydim = 3;
    c->attached_to_agent = false;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;

    container_def(CONTAINER_TYPE_CRAFTING_BENCH_UTILITY, "crafting_bench_basic");
    c->xdim = 4;
    c->ydim = 1;
    c->alt_xdim = 1;
    c->alt_ydim = 3;
    c->attached_to_agent = false;
    c->alpha_action = &craft_input_alpha_action_decision_tree;
    c->beta_action = &craft_input_beta_action_decision_tree;
    c->alpha_packet = &send_craft_alpha_action;
    c->beta_packet = &send_craft_beta_action;
    c->alpha_action_alt = &craft_output_alpha_action_decision_tree;
    c->beta_action_alt = &craft_output_beta_action_decision_tree;
    c->alpha_packet_alt = &send_craft_item_action;
    c->beta_packet_alt = &send_craft_item_action;

    container_def(CONTAINER_TYPE_CRYOFREEZER_SMALL, "cryofreezer_small");
    c->xdim = 2;
    c->ydim = 2;
    c->attached_to_agent = false;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;

    container_def(CONTAINER_TYPE_SMELTER_ONE, "smelter_basic");
    c->xdim = 1;
    c->ydim = 1;
    c->alt_xdim = 1;
    c->alt_ydim = 1;
    c->attached_to_agent = false;
    c->alpha_action = &smelter_alpha_action_decision_tree;
    c->beta_action = &smelter_beta_action_decision_tree;
    c->alpha_packet = &send_smelter_alpha_action;
    c->beta_packet = &send_smelter_beta_action;

    container_def(CONTAINER_TYPE_CRUSHER, "crusher");
    c->xdim = 1;
    c->ydim = 1;
    c->attached_to_agent = false;
    c->alpha_action = &crusher_alpha_action_decision_tree;
    c->beta_action = &crusher_beta_action_decision_tree;
    c->alpha_packet = &send_crusher_alpha_action;
    c->beta_packet = &send_crusher_beta_action;
    c->alpha_action_alt = &crusher_crush_alpha_action_decision_tree;
    c->alpha_packet_alt = &send_crusher_crush_action;
  
    add_container(c);   // finalize
}
    
static void validate_settings()
{
    GS_ASSERT_ABORT(get_attr("none") != NULL && get_attr("none")->max_dim() == 0);
    GS_ASSERT_ABORT(get_attr("hand") != NULL && get_attr("hand")->max_dim() == 1);
    GS_ASSERT_ABORT(get_attr("inventory") != NULL && get_attr("inventory")->max_dim() == 18);
    GS_ASSERT_ABORT(get_attr("toolbelt") != NULL && get_attr("toolbelt")->max_dim() == 9);
    GS_ASSERT_ABORT(get_attr("synthesizer") != NULL && get_attr("synthesizer")->max_dim() == 1);
    GS_ASSERT_ABORT(get_attr("energy_tanks") != NULL && get_attr("energy_tanks")->max_dim() == 4);
    GS_ASSERT_ABORT(get_attr("storage_block_small") != NULL && get_attr("storage_block_small")->max_dim() == 9);
    GS_ASSERT_ABORT(get_attr("crafting_bench_basic") != NULL && get_attr("crafting_bench_basic")->max_dim() == 4);
    GS_ASSERT_ABORT(get_attr("cryofreezer_small") != NULL && get_attr("cryofreezer_small")->max_dim() == 4);
    GS_ASSERT_ABORT(get_attr("smelter_basic") != NULL && get_attr("smelter_basic")->max_dim() == 1);
    GS_ASSERT_ABORT(get_attr("crusher") != NULL && get_attr("crusher")->max_dim() == 1);
    
    int n_none = 0;
    for (int i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        class ContainerAttributes* c = container_attributes[i];
        if (c == NULL || !c->loaded) continue;
        if (c->type == CONTAINER_TYPE_NONE)
        {
            n_none++;
            continue;
        }
        GS_ASSERT(is_valid_container_name(c->name));
        GS_ASSERT(c->xdim > 0);
        GS_ASSERT(c->ydim > 0);
        GS_ASSERT(c->max_dim() <= MAX_CONTAINER_SIZE);
        if (c->type != AGENT_HAND)
        {
            GS_ASSERT(c->alpha_action != NULL);
            GS_ASSERT(c->beta_action != NULL);
            #if DC_CLIENT
            GS_ASSERT(c->alpha_packet != NULL);
            GS_ASSERT(c->beta_packet != NULL);
            #endif
        }
    }
    GS_ASSERT(n_none == 1);

    // make sure no names collide
    for (int i=0; i<MAX_CONTAINER_TYPES-1; i++)
    for (int j=i+1; j<MAX_CONTAINER_TYPES; j++)
    {
        class ContainerAttributes* c = container_attributes[i];
        class ContainerAttributes* d = container_attributes[j];
        if (c == NULL || d == NULL) continue;
        if (!c->loaded || !d->loaded) continue;
        GS_ASSERT(strcmp(c->name, d->name) != 0);
    }
}

void init_config()
{
    GS_ASSERT(container_attributes == NULL);
    container_attributes = (ContainerAttributes**)calloc(MAX_CONTAINER_TYPES, sizeof(ContainerAttributes*));
    register_settings();
    validate_settings();

    #if DC_SERVER
    init_crusher_dat();
    #endif
}

void teardown_config()
{
    if (container_attributes != NULL)
    {
        for (int i=0; i<MAX_CONTAINER_TYPES; i++)
            if (container_attributes[i] != NULL)
                delete container_attributes[i];
        free(container_attributes);
    }

    #if DC_SERVER
    teardown_crusher_dat();
    #endif
}

/* Public Attribute Accessors */

ItemContainerType get_type(const char* name)
{
    for (int i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        if (container_attributes[i] != NULL && container_attributes[i]->loaded
         && strcmp(container_attributes[i]->name, name) == 0)
            return container_attributes[i]->type;
    }
    return CONTAINER_TYPE_NONE;
}

ItemContainerType get_compatible_type(const char* name)
{
    // TODO -- renaming scheme
    return get_type(name);
}

class ContainerAttributes* get_attr(const char* name)
{
    ItemContainerType type = get_type(name);
    return get_attr(type);
}

class ContainerAttributes* get_attr(ItemContainerType type)
{
    ASSERT_VALID_CONTAINER_TYPE(type);
    IF_INVALID_CONTAINER_TYPE(type) return NULL;
    return container_attributes[type];
}

unsigned int get_container_max_slots(ItemContainerType type)
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
    return attr->attached_to_agent;
}

bool container_type_is_block(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return false;
    return !attr->attached_to_agent;
}

const char* get_container_name(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    if (attr == NULL) return NULL;
    return attr->name;
}

bool is_valid_container_name(const char* name)
{
    size_t len = strlen(name);
    if (len <= 0 || len > CONTAINER_NAME_MAX_LENGTH) return false;
    for (size_t i=0; i<len; i++)
        if (!is_valid_name_char(name[i]))
            return false;
    return true;
}
    
}   // ItemContainer
