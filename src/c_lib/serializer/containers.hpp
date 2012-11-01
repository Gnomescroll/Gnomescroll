#pragma once

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

        int container_id;
        char name[DAT_NAME_MAX_LENGTH+1];
        int item_count;
        struct
        {
            int x,y,z;
        } position;
        bool valid;

    void reset()
    {
        this->valid = false;
        this->container_id = -1;
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
