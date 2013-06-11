#include "dat.hpp"

namespace Entities
{

class EntityAttributes* attributes = NULL;
class DatNameMap* entity_name_map = NULL;

void init_entity_dat()
{
    GS_ASSERT(attributes == NULL);
    attributes = new class EntityAttributes[MAX_ENTITY_TYPES];
    GS_ASSERT(entity_name_map == NULL);
    entity_name_map = new DatNameMap(256, DAT_NAME_MAX_LENGTH);
}

void teardown_entity_dat()
{
    delete[] attributes;
    delete entity_name_map;
}


class EntityAttributes* e = NULL;

void finish_def()
{
    GS_ASSERT_ABORT(e != NULL);
    if (e == NULL) return;
    GS_ASSERT_ABORT(!e->loaded);
    if (e->loaded) return;
    e->loaded = true;
}

void entity_def(const char* name, EntityType type)
{
    if (e != NULL) finish_def();

    GS_ASSERT_ABORT(type >= 0 && type < MAX_ENTITY_TYPES);
    IF_INVALID_ENTITY_TYPE(type) return;

    e = &attributes[type];
    GS_ASSERT_ABORT(!e->loaded);

    e->type = type;
    strncpy(e->name, name, DAT_NAME_MAX_LENGTH+1);
    e->name[DAT_NAME_MAX_LENGTH] = '\0';
}

void verify_entity_dat()
{
    // check attributes
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
    {
        class EntityAttributes* e = &attributes[i];
        if (!e->loaded) continue;

        GS_ASSERT_ABORT(e->type != ENTITY_NONE);
        GS_ASSERT_ABORT(is_valid_entity_name(e->name));
    }

    // check no type used twice
    for (int i=0; i<MAX_ENTITY_TYPES-1; i++)
    for (int j=i+1; j<MAX_ENTITY_TYPES; j++)
    {
        class EntityAttributes* a = &attributes[i];
        class EntityAttributes* b = &attributes[j];
        if (!a->loaded || !b->loaded) continue;
        GS_ASSERT_ABORT(a->type != b->type);
    }

    // check inactive names against active
    for (int i=0; i<MAX_ENTITY_TYPES; i++)
        if (attributes[i].loaded)
        {
            GS_ASSERT_ABORT(entity_name_map->get_mapped_name(e->name) == NULL);
        }

    // check inactive name destinations against active names
    for (size_t i=0; i<entity_name_map->size; i++)
    {
        GS_ASSERT_ABORT(get_entity_type(entity_name_map->get_replacement(i)) != ENTITY_NONE);
    }

    #if DC_SERVER || !PRODUCTION
    bool active_dat = file_exists(DATA_PATH ENTITY_NAME_FILE_ACTIVE);
    bool inactive_dat = file_exists(DATA_PATH ENTITY_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT((active_dat && inactive_dat) || (!active_dat && !inactive_dat));
    if (active_dat && inactive_dat)
    {
        GS_ASSERT_ABORT(name_changes_valid(DATA_PATH ENTITY_NAME_FILE_ACTIVE, DATA_PATH ENTITY_NAME_FILE_INACTIVE,
            DAT_NAME_MAX_LENGTH, attributes, MAX_ENTITY_TYPES, entity_name_map));
    }
    #endif
}

static void save_entity_names()
{
    #if DC_SERVER || !PRODUCTION
    bool saved = save_active_names(attributes, MAX_ENTITY_TYPES, DAT_NAME_MAX_LENGTH, DATA_PATH ENTITY_NAME_FILE_ACTIVE);
    GS_ASSERT_ABORT(saved);
    saved = entity_name_map->save(DATA_PATH ENTITY_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT(saved);
    #endif
}

// Use this to remove or rename a entity
void change_entity_name(const char* original, const char* replacement)
{
    GS_ASSERT_ABORT(is_valid_entity_name(original));
    GS_ASSERT_ABORT(is_valid_entity_name(replacement));
    bool mapped = entity_name_map->add_definition(original, replacement);
    GS_ASSERT_ABORT(mapped);
}

void end_entity_dat()
{
    apply_entity_dat_changes();
    verify_entity_dat();
    save_entity_names();
}

}   // Entities

