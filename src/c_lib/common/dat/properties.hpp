#pragma once

template <typename Type>
class Property
{
    public:
        Type type;
        char name[DAT_NAME_MAX_LENGTH+1];
        unsigned int hash;
        bool loaded;

    void set_type(Type type)
    {
        this->type = type;
    }

    void set_name(const char* name)
    {
        GS_ASSERT(is_valid_name(name));
        strncpy(this->name, name, DAT_NAME_MAX_LENGTH+1);
        this->name[DAT_NAME_MAX_LENGTH] = '\0';
        this->hash = strhash(this->name);
    }

    void done_loading()
    {
        GS_ASSERT(!this->loaded);
        GS_ASSERT(isValid(this->type));
        GS_ASSERT(this->name[0] != '\0');
        GS_ASSERT(this->hash != 0);
        this->loaded = true;
    }

    Property(Type null_type) :
        type(null_type), hash(0), loaded(false)
    {
        memset(this->name, 0, sizeof(this->name));
    }
};


template <class Property, typename Type>
class Properties
{
    public:
        size_t current;
        size_t max;
        Property* properties;
        Property* current_property;

    /* Reading API */

    Property* get(const char* name)
    {
        unsigned int hash = strhash(name);
        for (size_t i=0; i<this->max; i++)
            if (this->properties[i].loaded &&
                this->properties[i].hash == hash)
                {
                    return &this->properties[i];
                }
        GS_ASSERT(false);
        printf("No property found for name: %s\n", name);
        return NULL;
    }

    Property* get(Type type)
    {
        IF_ASSERT(!isValid(type)) return NULL;
        Property* p = &this->properties[type];
        if (!p->loaded) return NULL;
        return p;
    }

    /* Loading API */

    Property* get_next()
    {
        if (this->current_property != NULL) this->done_loading();
        IF_ASSERT(this->current >= this->max) return NULL;
        do
        {
            this->current_property = &this->properties[this->current++];
        } while (this->current_property->loaded &&
                 this->current <= this->max);
        this->current_property->set_type((Type)(this->current-1));
        return this->current_property;
    }

    Property* get_next(Type type)
    {
        if (this->current_property != NULL) this->done_loading();
        IF_ASSERT(!isValid(type)) return NULL;
        IF_ASSERT(this->properties[type].loaded) return NULL;
        this->current_property = &this->properties[type];
        this->current_property->set_type(type);
        return this->current_property;
    }

    void done_loading()
    {
        if (this->current_property == NULL) return;
        this->current_property->done_loading();
        this->current_property = NULL;
    }

    Properties(size_t max) :
        current(0), max(max), current_property(NULL)
    {
        this->properties = new Property[max];
    }

    ~Properties()
    {
        delete[] this->properties;
    }
};
