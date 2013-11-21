/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "_interface.hpp"

#include <item/common/constants.hpp>
#include <item/common/enum.hpp>
#include <item/container/config/_state.hpp>

#if DC_CLIENT
# include <item/container/client.hpp>
#endif

#if DC_SERVER
# include <item/container/config/crusher_dat.hpp>
# include <item/container/server.hpp>

// packet send stubs
// client has these defined and we want them in config
// but dont want to have to split the config up for client/server
void send_container_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_container_beta_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_synthesizer_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_synthesizer_beta_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_craft_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_purchase_item_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_craft_beta_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_craft_item_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_no_container_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_no_container_beta_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_smelter_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_smelter_beta_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_crusher_alpha_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_crusher_beta_action(ContainerActionType action, ItemContainerID container_id, int slot){}
void send_crusher_crush_action(ContainerActionType action, ItemContainerID container_id, int slot){}
#endif

namespace ItemContainer
{

#define CONTAINER_NAME_FILE_ACTIVE   "container_names.active"
#define CONTAINER_NAME_FILE_INACTIVE "container_names.inactive"

/* Configuration Loader */

static class ContainerAttribute* c = NULL;


static void container_def(const char* name)
{   // Do not call this directly
    c = container_attributes->get_next();
    GS_ASSERT_ABORT(c != NULL);
    IF_ASSERT(c == NULL) return;
    c->set_name(name);
}

// TODO -- container names should be inherited from the block names
// Also, we shouldn't need enums
// The container_block_types map (cube->container)
// will be constructed after this point

// just make sure auto-mapping still works without supervision for override item defs
// of blocks

static void null_container_def(const char* name)
{
    static int i = 0;
    GS_ASSERT_ABORT(!(i++));
    container_def(name);
}

static void agent_container_def(const char* name)
{
    container_def(name);
    GS_ASSERT_ABORT(c != NULL);
    IF_ASSERT(c == NULL) return;
    c->attached_to_agent = true;
}

static void cube_container_def(const char* name)
{
    container_def(name);
    GS_ASSERT_ABORT(c != NULL);
    IF_ASSERT(c == NULL) return;
    c->attached_to_agent = false;

    CubeType cube_type = t_map::get_cube_type(name);
    t_map::CubeProperty* attr = t_map::get_cube_properties(cube_type);
    GS_ASSERT_ABORT(attr != NULL);
    IF_ASSERT(attr == NULL) return;
    attr->container_type = c->type;
}

static void set_pretty_name(const char* pretty_name)
{
    GS_ASSERT_ABORT(c != NULL);
    IF_ASSERT(c == NULL) return;
    container_attributes->set_pretty_name(c->type, pretty_name);
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

    agent_container_def("hand");
    c->xdim = 1;
    c->ydim = 1;
    c->create_function = &new_hand;
    // there are no alpha/beta actions for hand; it has a special role in the manipulation of inventory

    agent_container_def("inventory");
    c->xdim = 6;
    c->ydim = 4;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    c->create_function = &new_container;

    agent_container_def("toolbelt");
    c->xdim = 9;
    c->ydim = 1;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    c->create_function = &new_container;

    agent_container_def("synthesizer");
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
    c->create_function = &new_synthesizer;

    agent_container_def("energy_tanks");
    set_pretty_name("Energy Shields");
    c->xdim = 4;
    c->ydim = 1;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    c->create_function = &new_energy_tanks;

    agent_container_def("premium_cache");
    set_pretty_name("Cache");
    c->xdim = 4;
    c->ydim = 4;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    c->create_function = &new_container;

    agent_container_def("equipment");
    c->xdim = 7;
    c->ydim = 1;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    c->create_function = &new_equipment;

    cube_container_def("storage_block_small");
    set_pretty_name("Storage Block");
    c->xdim = 3;
    c->ydim = 3;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    c->create_function = &new_container;

    cube_container_def("crafting_bench_basic");
    set_pretty_name("Crafting Bench");
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
    c->create_function = &new_crafting_bench;

    cube_container_def("cryofreezer_small");
    set_pretty_name("Cryofreezer");
    c->xdim = 2;
    c->ydim = 2;
    c->alpha_action = &alpha_action_decision_tree;
    c->beta_action = &beta_action_decision_tree;
    c->alpha_packet = &send_container_alpha_action;
    c->beta_packet = &send_container_beta_action;
    c->create_function = &new_cryofreezer;

    cube_container_def("smelter_basic");
    set_pretty_name("Smelter");
    c->xdim = 1;
    c->ydim = 1;
    c->alt_xdim = 1;
    c->alt_ydim = 1;
    c->alpha_action = &smelter_alpha_action_decision_tree;
    c->beta_action = &smelter_beta_action_decision_tree;
    c->alpha_packet = &send_smelter_alpha_action;
    c->beta_packet = &send_smelter_beta_action;
    c->create_function = &new_smelter;

    cube_container_def("crusher");
    c->xdim = 1;
    c->ydim = 1;
    c->alpha_action = &crusher_alpha_action_decision_tree;
    c->beta_action = &crusher_beta_action_decision_tree;
    c->alpha_packet = &send_crusher_alpha_action;
    c->beta_packet = &send_crusher_beta_action;
    c->alpha_action_alt = &crusher_crush_alpha_action_decision_tree;
    c->alpha_packet_alt = &send_crusher_crush_action;
    c->create_function = &new_crusher;

    container_attributes->done_loading();
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

    container_name_map->condense();  // finalize
}

static void validate_settings()
{
    // if the dimensions change, we need to know as it affects the serializer
    // It is safe to increase the max dimensions of a container, but not to shrink it
    // NEVER LOWER MAX DIMENSIONS OF A CONTAINER
    GS_ASSERT_ABORT(get_attr("none") != NULL && get_attr("none")->max_dim() == 0);
    GS_ASSERT_ABORT(get_attr("hand") != NULL && get_attr("hand")->max_dim() == 1);
    GS_ASSERT_ABORT(get_attr("inventory") != NULL && get_attr("inventory")->max_dim() == 24);
    GS_ASSERT_ABORT(get_attr("premium_cache") != NULL && get_attr("premium_cache")->max_dim() == 16);
    GS_ASSERT_ABORT(get_attr("toolbelt") != NULL && get_attr("toolbelt")->max_dim() == 9);
    GS_ASSERT_ABORT(get_attr("equipment") != NULL && get_attr("equipment")->max_dim() == 7);
    GS_ASSERT_ABORT(get_attr("synthesizer") != NULL && get_attr("synthesizer")->max_dim() == 1);
    GS_ASSERT_ABORT(get_attr("energy_tanks") != NULL && get_attr("energy_tanks")->max_dim() == 4);
    GS_ASSERT_ABORT(get_attr("storage_block_small") != NULL && get_attr("storage_block_small")->max_dim() == 9);
    GS_ASSERT_ABORT(get_attr("crafting_bench_basic") != NULL && get_attr("crafting_bench_basic")->max_dim() == 4);
    GS_ASSERT_ABORT(get_attr("cryofreezer_small") != NULL && get_attr("cryofreezer_small")->max_dim() == 4);
    GS_ASSERT_ABORT(get_attr("smelter_basic") != NULL && get_attr("smelter_basic")->max_dim() == 1);
    GS_ASSERT_ABORT(get_attr("crusher") != NULL && get_attr("crusher")->max_dim() == 1);

    int n_none = 0;
    for (size_t i=0; i<container_attributes->max; i++)
    {
        class ContainerAttribute* c = &container_attributes->properties[i];
        if (!c->loaded) continue;
        if (strcmp(c->name, "none") == 0)
        {
            n_none++;
            continue;
        }
        GS_ASSERT_ABORT(is_valid_container_name(c->name));
        GS_ASSERT_ABORT(c->pretty_name[0] != '\0');
        GS_ASSERT_ABORT(c->xdim > 0);
        GS_ASSERT_ABORT(c->ydim > 0);
        GS_ASSERT_ABORT(c->max_dim() <= MAX_CONTAINER_SIZE);
        GS_ASSERT_ABORT(c->create_function != NULL);
        if (strcmp(c->name, "hand") != 0)
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
            class t_map::CubeProperty* p = t_map::get_cube_properties((CubeType)i);
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
    for (size_t i=0; i<container_attributes->max-1; i++)
    for (size_t j=i+1; j<container_attributes->max; j++)
    {
        class ContainerAttribute* c = &container_attributes->properties[i];
        class ContainerAttribute* d = &container_attributes->properties[j];
        if (!c->loaded || !d->loaded) continue;
        GS_ASSERT_ABORT(strcmp(c->name, d->name) != 0);
    }

    // all ItemContainer blocks must have a container defined
    for (int i=0; i<MAX_CUBES; i++)
    {
        if (!t_map::isValidCube((CubeType)i)) continue;
        if (t_map::get_cube_group((CubeType)i) == ItemContainerCube)
        {
            GS_ASSERT_ABORT(get_type(t_map::get_cube_name((CubeType)i)) != NULL_CONTAINER_TYPE);
        }
    }

    GS_ASSERT_ABORT(container_name_map->condensed);

    // check inactive names against active
    for (size_t i=0; i<container_attributes->max; i++)
        if (container_attributes->properties[i].loaded)
        {
            GS_ASSERT_ABORT(container_name_map->get_mapped_name(container_attributes->properties[i].name) == NULL);
        }

    // check inactive name destinations against active
    for (size_t i=0; i<container_name_map->size; i++)
    {
        GS_ASSERT_ABORT(get_type(container_name_map->get_replacement(i)) != NULL_CONTAINER_TYPE);
    }

    #if DC_SERVER || !PRODUCTION
    // either both files must be missing or both must exist
    bool active_dat = file_exists(DATA_PATH CONTAINER_NAME_FILE_ACTIVE);
    bool inactive_dat = file_exists(DATA_PATH CONTAINER_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT((active_dat && inactive_dat) || (!active_dat && !inactive_dat));

    if (active_dat && inactive_dat)
    {   // check that all names declared a valid with respect to past name definitions
        // but only if the files are present
        GS_ASSERT_ABORT(name_changes_valid(
            DATA_PATH CONTAINER_NAME_FILE_ACTIVE, DATA_PATH CONTAINER_NAME_FILE_INACTIVE,
            DAT_NAME_MAX_LENGTH,
            container_attributes->properties, MAX_CONTAINER_TYPES, container_name_map));
    }
    #endif
}

void save_container_names()
{
    #if DC_SERVER || !PRODUCTION
    bool saved = save_active_names(container_attributes->properties,
                                   MAX_CONTAINER_TYPES,
                                   DAT_NAME_MAX_LENGTH,
                                   DATA_PATH CONTAINER_NAME_FILE_ACTIVE);
    GS_ASSERT_ABORT(saved);
    saved = container_name_map->save(DATA_PATH CONTAINER_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT(saved);
    #endif
}

void init_config()
{
    GS_ASSERT(container_attributes == NULL);
    container_attributes = new class ContainerAttributes;

    GS_ASSERT(container_name_map == NULL);
    container_name_map = new class DatNameMap(32, DAT_NAME_MAX_LENGTH);

    #if DC_SERVER
    init_crusher_dat();
    #endif
}

void teardown_config()
{
    delete container_attributes;
    delete container_name_map;
    #if DC_SERVER
    teardown_crusher_dat();
    #endif
}

void load_config()
{
    register_settings();
    name::unpack();
}

void end_config()
{
    container_attributes->done_loading();
    container_attributes->set_pretty_names();
    apply_container_dat_changes();
    validate_settings();
    save_container_names();
}

/* Public Attribute Accessors */

class ContainerAttribute* get_attr(const char* name)
{
    return container_attributes->get(name);
}

class ContainerAttribute* get_attr(ItemContainerType type)
{
    return container_attributes->get(type);
}

ItemContainerType get_type(const char* name)
{
    ContainerAttribute* attr = get_attr(name);
    IF_ASSERT(attr == NULL) return NULL_CONTAINER_TYPE;
    return attr->type;
}

const char* get_compatible_name(const char* name)
{
    const char* mapname = container_name_map->get_mapped_name(name);
    if (mapname != NULL) return mapname;
    if (get_type(name) != NULL_CONTAINER_TYPE) return name;
    return NULL;
}

unsigned int get_container_max_slots(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return 0;
    return attr->max_dim();
}

int get_container_xdim(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return 0;
    return attr->xdim;
}

int get_container_ydim(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return 0;
    return attr->ydim;
}

int get_container_alt_max_slots(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return 0;
    return attr->max_alt_dim();
}

int get_container_alt_xdim(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return 0;
    return attr->alt_xdim;
}

int get_container_alt_ydim(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return 0;
    return attr->alt_ydim;
}

bool container_type_is_attached_to_agent(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return false;
    return attr->attached_to_agent;
}

bool container_type_is_block(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return false;
    return !attr->attached_to_agent;
}

const char* get_container_name(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return NULL;
    return attr->name;
}

const char* get_container_pretty_name(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL)return NULL;
    return attr->pretty_name;
}

containerCreate get_container_create_function(ItemContainerType type)
{
    class ContainerAttribute* attr = get_attr(type);
    IF_ASSERT(attr == NULL) return NULL;
    return attr->create_function;
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
