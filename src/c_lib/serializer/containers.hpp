/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <serializer/parse.hpp>

namespace serializer
{

/* Parse data structs */

class ParsedContainerFileData
{
    public:

        int version;
        int container_count;
        bool valid;

    void reset()
    {
        this->valid = false;
        this->version = 0;
        this->container_count = 0;
    }

    ParsedContainerFileData()
    {
        this->reset();
    }
};

class ParsedContainerData
{
    public:

        ItemContainerID container_id;
        char name[DAT_NAME_MAX_LENGTH+1];
        int item_count;
        struct Vec3i position;
        bool valid;

    void reset()
    {
        this->valid = false;
        this->container_id = NULL_CONTAINER;
        this->name[0] = '\0';
        this->item_count = 0;
        this->position.x = -1;
        this->position.y = -1;
        this->position.z = -1;
    }

    ParsedContainerData()
    {
        this->reset();
    }
};


bool load_containers();
bool save_containers();

}   // serializer
