#include "entities.hpp"

namespace serializer
{

bool load_entities_file(const char* fn)
{
    return true;

    // Entity header

    // Entity data, based on type in header
}

bool save_entities()
{
    return true;

    // For entity in entities we care about
    //  // Write entity header
    //  // Write data for the entity

    // spawner, energy cores
    // position for both (integers)

    // spawners also have a list of user_ids that are attached to them
    // can be comma separated list
}

bool load_entities()
{
    if (file_exists(entity_path) && fsize(entity_path) > 0)
        return load_entities_file(entity_path);
    else
    if (file_exists(entity_path_bak) && fsize(entity_path_bak) > 0)
        return load_entities_file(entity_path_bak);
    else
        printf("WARNING: No entities file found\n");
    return true;
}

}   // serializer

