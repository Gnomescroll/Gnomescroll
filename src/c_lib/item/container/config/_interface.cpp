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

static void add_container()
{
    GS_ASSERT_ABORT(c != NULL);
    if (c == NULL) return;
    c->loaded = true;
}

static void container_def(ItemContainerType type, const char* name)
{   // Don't call this directly
    ASSERT_VALID_CONTAINER_TYPE(type);
    IF_INVALID_CONTAINER_TYPE(type)
    {
        GS_ASSERT_ABORT(false);
        return;
    }
    if (c != NULL) add_container();
    c = &container_attributes[type];
    c->type = type;
    c->set_name(name);
}

// TODO -- container names should be inherited from the block names
// Also, we shouldn't need enums
// The container_block_types map (cube->container)
// will be constructed after this point

// just make sure auto-mapping still works without supervision for override item defs
// of blocks

void null_container_def(const char* name)
{
    static int i=0;
    GS_ASSERT_ABORT(!(i++));
    container_def(CONTAINER_TYPE_NONE, name); 
}

void agent_container_def(ItemContainerType type, const char* name)
{
    container_def(type, name);
    c->attached_to_agent = true;
}

void block_container_def(ItemContainerType type)
{
    const char* name = t_map::get_cube_name_for_container(type);
    GS_ASSERT_ABORT(name != NULL);
    if (name == NULL) return;
    container_def(type, name);  // name will be set by block later
    c->attached_to_agent = false;
}
        
static void register_settings()
{
    GS_ASSERT_ABORT(c == NULL);

    // special NULL container
    null_container_def("none");
    c->xdim = 0;
    c->ydim = 0;
    c->attached_to_agent = false;
    c->alpha_action = &no_container_alpha_action_decision_tree;
    c->beta_action = &no_container_beta_action_decision_tree;
    c->alpha_packet = &send_no_container_alpha_action;
    c->beta_packet = &send_no_container_beta_action;

    agent_container_def(AGENT_HAND, "hand");
    c->xdim = 1;
    c->ydim = 1;
    // there are no alpha/beta actions for hand; it has a special role in the manipulation of inventory

    agent_container_def(AGENT_INVENTORY, "inventory");
    c->xdim = 6;
    c->ydim = 3;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;

    agent_container_def(AGENT_TOOLBELT, "toolbelt");
    c->xdim = 9;
    c->ydim = 1;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;

    agent_container_def(AGENT_SYNTHESIZER, "synthesizer");
    c->xdim = 1;
    c->ydim = 1;
    c->alt_xdim = 5;
    c->alt_ydim = 3;
    c->alpha_action = &synthesizer_alpha_action_decision_tree;
    c->beta_action = &synthesizer_beta_action_decision_tree;
    c->alpha_packet = &send_synthesizer_alpha_action;
    c->beta_packet = &send_synthesizer_beta_action;
    c->alpha_action_alt = &synthesizer_shopping_alpha_action_decision_tree;
    c->beta_action_alt = &synthesizer_shopping_beta_action_decision_tree;
    c->alpha_packet_alt = &send_purchase_item_action;
    c->beta_packet_alt = &send_purchase_item_action;

    agent_container_def(AGENT_ENERGY_TANKS, "energy_tanks");
    c->xdim = 4;
    c->ydim = 1;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    
    block_container_def(CONTAINER_TYPE_STORAGE_BLOCK_SMALL);
    c->xdim = 3;
    c->ydim = 3;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;

    block_container_def(CONTAINER_TYPE_CRAFTING_BENCH_UTILITY);
    c->xdim = 4;
    c->ydim = 1;
    c->alt_xdim = 1;
    c->alt_ydim = 3;
    c->alpha_action = &craft_input_alpha_action_decision_tree;
    c->beta_action = &craft_input_beta_action_decision_tree;
    c->alpha_packet = &send_craft_alpha_action;
    c->beta_packet = &send_craft_beta_action;
    c->alpha_action_alt = &craft_output_alpha_action_decision_tree;
    c->beta_action_alt = &craft_output_beta_action_decision_tree;
    c->alpha_packet_alt = &send_craft_item_action;
    c->beta_packet_alt = &send_craft_item_action;

    block_container_def(CONTAINER_TYPE_CRYOFREEZER_SMALL);
    c->xdim = 2;
    c->ydim = 2;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;

    block_container_def(CONTAINER_TYPE_SMELTER_ONE);
    c->xdim = 1;
    c->ydim = 1;
    c->alt_xdim = 1;
    c->alt_ydim = 1;
    c->alpha_action = &smelter_alpha_action_decision_tree;
    c->beta_action = &smelter_beta_action_decision_tree;
    c->alpha_packet = &send_smelter_alpha_action;
    c->beta_packet = &send_smelter_beta_action;

    block_container_def(CONTAINER_TYPE_CRUSHER);
    c->xdim = 1;
    c->ydim = 1;
    c->alpha_action = &crusher_alpha_action_decision_tree;
    c->beta_action = &crusher_beta_action_decision_tree;
    c->alpha_packet = &send_crusher_alpha_action;
    c->beta_packet = &send_crusher_beta_action;
    c->alpha_action_alt = &crusher_crush_alpha_action_decision_tree;
    c->alpha_packet_alt = &send_crusher_crush_action;
  
    add_container();   // finalize
}

void change_container(const char* original, const char* replacement)
{
    GS_ASSERT_ABORT(is_valid_container_name(original));
    GS_ASSERT_ABORT(is_valid_container_name(replacement));        
    bool mapped = container_name_map->add_definition(original, replacement);
    GS_ASSERT_ABORT(mapped);
}

static void apply_container_dat_changes()
{
    // NEVER REUSE A NAME!!
    //change_container("old_name", "new_name");
}
    
static void validate_settings()
{
    // if the dimensions change, we need to know as it affects the serializer
    // It is safe to increase the max dimensions of a container, but not to shrink it
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
        class ContainerAttributes* c = &container_attributes[i];
        if (!c->loaded) continue;
        if (c->type == CONTAINER_TYPE_NONE)
        {
            n_none++;
            continue;
        }
        GS_ASSERT_ABORT(is_valid_container_name(c->name));
        GS_ASSERT_ABORT(c->xdim > 0);
        GS_ASSERT_ABORT(c->ydim > 0);
        GS_ASSERT_ABORT(c->max_dim() <= MAX_CONTAINER_SIZE);
        if (c->type != AGENT_HAND)
        {
            GS_ASSERT_ABORT(c->alpha_action != NULL);
            GS_ASSERT_ABORT(c->beta_action != NULL);
            #if DC_CLIENT
            GS_ASSERT_ABORT(c->alpha_packet != NULL);
            GS_ASSERT_ABORT(c->beta_packet != NULL);
            #endif
        }

        // containers should share a block name if attached to agent
        // containers should only share a block name if not attached to agent
        bool block_match = false;
        for (int i=0; i<MAX_CUBES; i++)
        {
            class t_map::CubeProperties* p = t_map::get_cube_properties((CubeID)i);
            if (p != NULL && strcmp(p->name, c->name) == 0)
            {
                GS_ASSERT_ABORT(p->container_type == c->type);
                GS_ASSERT_ABORT(!c->attached_to_agent);
                block_match = true;
                break;
            }
        }
        if (!block_match)
        {
            GS_ASSERT_ABORT(c->attached_to_agent);
        }
    }
    GS_ASSERT_ABORT(n_none == 1);

    // make sure no names collide
    for (int i=0; i<MAX_CONTAINER_TYPES-1; i++)
    for (int j=i+1; j<MAX_CONTAINER_TYPES; j++)
    {
        class ContainerAttributes* c = &container_attributes[i];
        class ContainerAttributes* d = &container_attributes[j];
        if (!c->loaded || !d->loaded) continue;
        GS_ASSERT_ABORT(strcmp(c->name, d->name) != 0);
    }

    // all ItemContainer blocks must have a container defined
    for (int i=0; i<MAX_CUBES; i++)
    {
        if (!t_map::isValidCube((CubeID)i)) continue;
        if (t_map::get_cube_type((CubeID)i) == ItemContainerCube)
        {
            GS_ASSERT_ABORT(get_type(t_map::get_cube_name((CubeID)i)) != CONTAINER_TYPE_NONE);
        }
    }
}

void init_config()
{
    GS_ASSERT(container_attributes == NULL);
    container_attributes = new class ContainerAttributes[MAX_CONTAINER_TYPES];

    GS_ASSERT(container_name_map == NULL);
    container_name_map = new class DatNameMap(32, DAT_NAME_MAX_LENGTH);
    
    #if DC_SERVER
    init_crusher_dat();
    #endif
}

void teardown_config()
{
    if (container_attributes != NULL) delete[] container_attributes;
    if (container_name_map != NULL) delete container_name_map;
    
    #if DC_SERVER
    teardown_crusher_dat();
    #endif
}

void load_config()
{
    register_settings();
}

void end_config()
{
    apply_container_dat_changes();
    validate_settings();
}

/* Public Attribute Accessors */

ItemContainerType get_type(const char* name)
{
    for (int i=0; i<MAX_CONTAINER_TYPES; i++)
    {
        if (container_attributes[i].loaded && strcmp(container_attributes[i].name, name) == 0)
            return container_attributes[i].type;
    }
    return CONTAINER_TYPE_NONE;
}

ItemContainerType get_compatible_type(const char* name)
{
    const char* mapname = container_name_map->get_mapped_name(name);
    if (mapname != NULL)
        return get_type(mapname);
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
    if (!container_attributes[type].loaded) return NULL;
    return &container_attributes[type];
}

unsigned int get_container_max_slots(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->max_dim();
}

int get_container_xdim(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->xdim;
}

int get_container_ydim(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->ydim;
}

int get_container_alt_max_slots(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->max_alt_dim();
}

int get_container_alt_xdim(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->alt_xdim;
}

int get_container_alt_ydim(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return 0;
    return attr->alt_ydim;
}

bool container_type_is_attached_to_agent(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return false;
    return attr->attached_to_agent;
}

bool container_type_is_block(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return false;
    return !attr->attached_to_agent;
}

const char* get_container_name(ItemContainerType type)
{
    class ContainerAttributes* attr = get_attr(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL;
    return attr->name;
}

bool is_valid_container_name(const char* name)
{
    size_t len = strlen(name);
    if (len <= 0 || len > DAT_NAME_MAX_LENGTH) return false;
    for (size_t i=0; i<len; i++)
        if (!is_valid_name_char(name[i]))
            return false;
    return true;
}

}   // ItemContainer
