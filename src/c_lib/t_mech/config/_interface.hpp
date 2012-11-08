#pragma once

#include <t_mech/common/common.hpp>
#include <t_mech/config/_interface.hpp>
#include <t_mech/properties.hpp>

namespace t_mech
{
    //this is whitespace/alpha dimensions for ray casting box
    int mech_sprite_width[256];  //centered
    int mech_sprite_height[256]; //from bottom
    float mech_sprite_width_f[256];  //centered
    float mech_sprite_height_f[256]; //from bottom
}

namespace t_mech
{

#define MECH_NAME_FILE_ACTIVE   "mech_names.active"
#define MECH_NAME_FILE_INACTIVE "mech_names.inactive"

int _current_mech_index = 0;
class MechAttribute* s = NULL;

void load_mech()
{
    GS_ASSERT(s != NULL);
    if (s == NULL) return;
    s->loaded = true;
    _current_mech_index++;
}

void mech_def(MechClass mech_type_class, const char* name)
{    
    if (s != NULL) load_mech();

    // check names
    GS_ASSERT(is_valid_mech_name(name));
    if (!is_valid_mech_name(name)) return;
    for (int i=0; i<_current_mech_index; i++)
        if (strcmp(name, mech_attributes[i].name) == 0)
        {
            GS_ASSERT(false);
            return;
        }

    MechType mech_type = (MechType)_current_mech_index;

    ASSERT_VALID_MECH_TYPE(mech_type);
    IF_INVALID_MECH_TYPE(mech_type) return;

    GS_ASSERT(!mech_attributes[mech_type].loaded);
    if(mech_attributes[mech_type].loaded) return;

    s = &mech_attributes[mech_type];
    
    s->mech_type = mech_type;
    s->mech_type_class = mech_type_class;
    strncpy(s->name, name, DAT_NAME_MAX_LENGTH);
    s->name[DAT_NAME_MAX_LENGTH] = '\0';
}

void set_sprite_index(int sprite_index)
{
    GS_ASSERT(s != NULL);
    if (s == NULL) return;
    GS_ASSERT(s->sprite_index == NULL_MECH_SPRITE);
    s->sprite_index = (MechSpriteIndex)sprite_index;
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
        GS_ASSERT_ABORT(a->sprite_index != NULL_MECH_SPRITE);
        GS_ASSERT_ABORT(a->render_type != MECH_RENDER_TYPE_NONE);
        GS_ASSERT_ABORT(a->mech_type != NULL_MECH_TYPE);
        GS_ASSERT_ABORT(a->mech_type_class != NULL_MECH_CLASS);
        GS_ASSERT_ABORT(is_valid_mech_name(a->name));
        GS_ASSERT_ABORT(i == a->mech_type);
    }

    for (int i=0; i<MAX_MECHS-1; i++)
    for (int j=i+1; j<MAX_MECHS; j++)
    {
        class MechAttribute* a = &mech_attributes[i];
        class MechAttribute* b = &mech_attributes[j];
        if (!a->loaded || !b->loaded) continue;
        GS_ASSERT_ABORT(strcmp(a->name, b->name) != 0);
        GS_ASSERT_ABORT(a->mech_type != b->mech_type);
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
