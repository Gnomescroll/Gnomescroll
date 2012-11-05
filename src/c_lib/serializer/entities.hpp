#pragma once

namespace serializer
{

class ParsedAgentSpawnerData
{
    private:
        size_t user_index;
    
    public:

        struct {
            int x,y,z;
        } position;
        size_t user_count;
        UserID* users;
        bool valid;

    void reset()
    {
        if (this->users != NULL)
            free(this->users);
        this->users = NULL;
        this->user_count = 0;
        this->user_index = 0;
        this->valid = false;
        this->position.x = -1;
        this->position.y = -1;
        this->position.z = -1;
    }

    bool add_user(UserID user_id)
    {
        ASSERT_VALID_USER_ID(user_id);
        IF_INVALID_USER_ID(user_id) return false;
        GS_ASSERT(this->users != NULL);
        if (this->users == NULL) return false;
        if (this->user_index >= this->user_count) return false;
        for (size_t i=0; i<this->user_index; i++)
        {
            GS_ASSERT(this->users[i] != user_id);
            if (this->users[i] == user_id)
                return false;
        }
        this->users[this->user_index++] = user_id;
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

    ParsedAgentSpawnerData()
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

        struct {
            int x,y,z;
        } position;

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
