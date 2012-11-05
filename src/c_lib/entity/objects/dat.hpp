#pragma once

namespace Objects
{

#define ENTITY_NAME_FILE_ACTIVE   "entity_names.active"
#define ENTITY_NAME_FILE_INACTIVE "entity_names.inactive"

// Attribute class

class EntityAttributes
{
    public:
        bool loaded;
        
        ObjectType type;
        char name[DAT_NAME_MAX_LENGTH+1];


    EntityAttributes() : loaded(false),
        type(OBJECT_NONE)
    {
        memset(this->name, 0, sizeof(this->name));
    }
};

extern class EntityAttributes* attributes;
extern class DatNameMap* entity_name_map;

void init_entity_dat();
void teardown_entity_dat();

// Validate
inline bool is_valid_entity_name(const char* name)
{
    return is_valid_name(name);
}

void verify_entity_dat();
void end_entity_dat();

// Use this to remove or rename a entity
void change_entity_name(const char* original, const char* replacement);

void entity_def(const char* name, ObjectType type);
void finish_def();

// Load
void load_entity_dat()
{
    //entity_def("");
    //e->;

    //finish_def();
}

void apply_entity_dat_changes()
{
    //change_entity("old_name", "new_name");
    entity_name_map->condense();  // finalize
}


ObjectType get_entity_type(const char* name)
{   // TODO -- use hash map
    for (int i=0; i<MAX_OBJECT_TYPES; i++)
        if (attributes[i].loaded && strcmp(name, attributes[i].name))
            return (ObjectType)i;
    return OBJECT_NONE;
}

}   // Objects
