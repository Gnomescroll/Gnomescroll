#pragma once

#include <serializer/parse.hpp>

namespace serializer
{

class ParsedEntityFileData
{
    public:

        int version;
        int entity_count;
        bool valid;

    void reset()
    {
        this->valid = false;
        this->version = 0;
        this->entity_count = 0;
    }
    
    ParsedEntityFileData()
    {
        this->reset();
    }
};

class ParsedEntityData
{
    public:

        char name[DAT_NAME_MAX_LENGTH+1];
        int entity_id;
        bool valid;

    void reset()
    {
        memset(this->name, 0, sizeof(this->name));
        this->entity_id = -1;
        this->valid = false;
    }
    
    ParsedEntityData()
    {
        this->reset();
    }
};

class ParsedAgentSpawnerData
{    
    public:
    
        struct MapPosition position;
        size_t user_count;
        size_t added_users;
        UserID* users;
        bool valid;

    void reset()
    {
        if (this->users != NULL) free(this->users);
        this->users = NULL;
        this->user_count = 0;
        this->added_users = 0;
        this->valid = false;
        this->position.x = -1;
        this->position.y = -1;
        this->position.z = -1;
    }

    bool add_user(UserID user_id)
    {
        IF_ASSERT(!isValid(user_id)) return false;
        IF_ASSERT(this->users == NULL) return false;
        if (this->added_users >= this->user_count) return false;
        for (size_t i=0; i<this->added_users; i++)
        {
            IF_ASSERT(this->users[i] == user_id)
                return false;
        }
        this->users[this->added_users++] = user_id;
        return true;
    }

    void set_user_count(size_t user_count)
    {
        this->user_count = user_count;
        if (this->user_count)
        {
            this->users = (UserID*)malloc(this->user_count * sizeof(UserID));
            for (size_t i=0; i<this->user_count; this->users[i++] = NULL_USER_ID);
        }
    }

    ParsedAgentSpawnerData() : users(NULL)
    {
        this->reset();
    }

    ~ParsedAgentSpawnerData()
    {
        if (this->users != NULL) free(this->users);
    }
};

class ParsedEnergyCoreData
{
    public:

        struct MapPosition position;
        bool valid;

    void reset()
    {
        this->valid = false;
        this->position.x = -1;
        this->position.y = -1;
        this->position.z = -1;
    }

    ParsedEnergyCoreData()
    {
        this->reset();
    }
};

bool save_entities();
bool load_entities();

}   // serializer
