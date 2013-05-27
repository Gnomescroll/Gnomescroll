#pragma once

#include <t_mech/common/common.hpp>
#include <t_mech/config/_interface.hpp>
#include <t_mech/properties.hpp>

namespace t_mech
{
    //this is whitespace/alpha dimensions for ray casting box
    int mech_sprite_width[MAX_MECHS];  //centered
    int mech_sprite_height[MAX_MECHS]; //from bottom
    float mech_sprite_width_f[MAX_MECHS];  //centered
    float mech_sprite_height_f[MAX_MECHS]; //from bottom
}

namespace t_mech
{

#define MECH_NAME_FILE_ACTIVE   "mech_names.active"
#define MECH_NAME_FILE_INACTIVE "mech_names.inactive"

int _current_mech_index = 0;
class MechAttribute* s = NULL;

void load_mech()
{
    IF_ASSERT(s == NULL) return;
    s->loaded = true;
    _current_mech_index++;
}

void mech_def(MechClassType class_type, const char* name, MechRenderType mech_render_type, MechBehaviorType behavior_type)
{
    if (s != NULL) load_mech();

    // check names
    IF_ASSERT(!is_valid_mech_name(name)) return;
    for (int i=0; i<_current_mech_index; i++)
        if (strcmp(name, mech_attributes[i].name) == 0)
        {
            GS_ASSERT(false);
            return;
        }

    MechType type = (MechType)_current_mech_index;

    IF_ASSERT(!isValid(type)) return;
    IF_ASSERT(mech_attributes[type].loaded) return;

    s = &mech_attributes[type];

    s->type = type;
    s->class_type = class_type;
    strncpy(s->name, name, DAT_NAME_MAX_LENGTH);
    s->name[DAT_NAME_MAX_LENGTH] = '\0';

    s->render_type = mech_render_type;
    s->behavior_type = behavior_type;

    if (class_type == MECH_WALL_OBJECT)
        s->size = 0.5f;
}

void set_sprite_index(int sprite)
{
    IF_ASSERT(s == NULL) return;
    GS_ASSERT(s->sprite == NULL_MECH_SPRITE);
    s->sprite = (MechSpriteIndex)sprite;
}

// Use this to remove or rename a mech
void change_mech(const char* original, const char* replacement)
{
    GS_ASSERT_ABORT(is_valid_mech_name(original));
    GS_ASSERT_ABORT(is_valid_mech_name(replacement));
    bool mapped = mech_name_map->add_definition(original, replacement);
    GS_ASSERT_ABORT(mapped);
}

void end_mech_dat()
{
    if (s != NULL) load_mech();
}

void verify_mech_dat()
{
    for (int i=0; i<MAX_MECHS; i++)
    {
        class MechAttribute* a = &mech_attributes[i];
        if (!a->loaded) continue;
        GS_ASSERT_ABORT(a->sprite != NULL_MECH_SPRITE);
        GS_ASSERT_ABORT(a->render_type != MECH_RENDER_TYPE_NONE);
        GS_ASSERT_ABORT(a->type != NULL_MECH_TYPE);
        GS_ASSERT_ABORT(a->class_type != NULL_MECH_CLASS);
        GS_ASSERT_ABORT(is_valid_mech_name(a->name));
        GS_ASSERT_ABORT(i == a->type);
    }

    for (int i=0; i<MAX_MECHS-1; i++)
    for (int j=i+1; j<MAX_MECHS; j++)
    {
        class MechAttribute* a = &mech_attributes[i];
        class MechAttribute* b = &mech_attributes[j];
        if (!a->loaded || !b->loaded) continue;
        GS_ASSERT_ABORT(strcmp(a->name, b->name) != 0);
        GS_ASSERT_ABORT(a->type != b->type);
    }

    GS_ASSERT_ABORT(mech_name_map->condensed);

    // check inactive names against active
    for (int i=0; i<MAX_MECHS; i++)
        if (mech_attributes[i].loaded)
        {
            GS_ASSERT_ABORT(mech_name_map->get_mapped_name(mech_attributes[i].name) == NULL);
        }

    // check inactive name destinations against active
    for (size_t i=0; i<mech_name_map->size; i++)
    {
        GS_ASSERT_ABORT(get_mech_type(mech_name_map->get_replacement(i)) != NULL_MECH_TYPE);
    }


    #if DC_SERVER || !PRODUCTION
    // either both files must be missing or both must exist
    bool active_dat = file_exists(DATA_PATH MECH_NAME_FILE_ACTIVE);
    bool inactive_dat = file_exists(DATA_PATH MECH_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT((active_dat && inactive_dat) || (!active_dat && !inactive_dat));

    if (active_dat && inactive_dat)
    {   // check that all names declared a valid with respect to past name definitions
        // but only if the files are present
        GS_ASSERT_ABORT(name_changes_valid(DATA_PATH MECH_NAME_FILE_ACTIVE, DATA_PATH MECH_NAME_FILE_INACTIVE,
            DAT_NAME_MAX_LENGTH, mech_attributes, MAX_MECHS, mech_name_map));
    }
    #endif


    /*
        Growth timer just in case
    */
    for (int i=0; i<MAX_MECHS; i++)
    {
        class MechAttribute* a = &mech_attributes[i];
        if (!a->loaded) continue;

        if (a->behavior_type == MECH_BEHAVIOR_TYPE_DEFAULT)
            GS_ASSERT_ABORT(a->growth_ttl == -1);
    }
}

void save_mech_names()
{
    #if DC_SERVER || !PRODUCTION
    bool saved = save_active_names(mech_attributes, MAX_MECHS, DAT_NAME_MAX_LENGTH, DATA_PATH MECH_NAME_FILE_ACTIVE);
    GS_ASSERT_ABORT(saved);
    saved = mech_name_map->save(DATA_PATH MECH_NAME_FILE_INACTIVE);
    GS_ASSERT_ABORT(saved);
    #endif
}


}   // t_mech
