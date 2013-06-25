#pragma once

#include <common/common.hpp>

template <typename Type>
class Property
{
    public:
        Type type;
        char name[DAT_NAME_MAX_LENGTH+1];
        char pretty_name[PRETTY_NAME_MAX_LENGTH+1];
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
        memset(this->pretty_name, 0, sizeof(this->pretty_name));
    }
};


template <class Property, typename Type>
class Properties
{
    private:
        size_t current;
    public:
        size_t index;
        size_t max;
        Property* properties;
        Property* current_property;

    /* Reading API */

    Property* get(const char* name)
    {
        IF_ASSERT(name == NULL) return NULL;
        unsigned int hash = strhash(name);
        for (size_t i=0; i<this->max; i++)
            if (this->properties[i].loaded &&
                this->properties[i].hash == hash)
                    return &this->properties[i];
        GS_ASSERT(false);
        return NULL;
    }

    Property* get(Type type)
    {
        Property* p = this->_get_any(type);
        IF_ASSERT(p == NULL) return NULL;
        if (!p->loaded) return NULL;
        return p;
    }

    /* Loading API */

    Property* get_next()
    {
        if (this->current_property != NULL)
            this->done_loading();
        IF_ASSERT(this->current >= this->max)
            return NULL;
        do
        {
            this->current_property = &this->properties[this->current++];
        } while (this->current_property->loaded &&
                 this->current <= this->max);
        this->current_property->set_type(Type(this->current-1));
        if (this->current > this->index)
            this->index = this->current;
        return this->current_property;
    }

    Property* get_next(Type type)
    {
        if (this->current_property != NULL) this->done_loading();
        IF_ASSERT(!isValid(type)) return NULL;
        IF_ASSERT(this->properties[type].loaded) return NULL;
        this->current_property = &this->properties[type];
        this->current_property->set_type(type);
        if ((size_t)type > this->index)
            this->index = type;
        return this->current_property;
    }

    Property* _get_any(Type type)
    {
        IF_ASSERT(!isValid(type)) return NULL;
        return &this->properties[type];
    }

    void done_loading()
    {
        if (this->current_property == NULL) return;
        this->current_property->done_loading();
        this->current_property = NULL;
    }

    Properties(size_t max) :
        current(0), index(0), max(max), current_property(NULL)
    {
        this->properties = new Property[max];
    }

    ~Properties()
    {
        delete[] this->properties;
    }

    /* Additional helpers that will only work on specialized Propertys */

    void set_pretty_name(Type type, const char* pretty_name)
    {
        Property* p = this->_get_any(type);
        IF_ASSERT(p == NULL) return;
        GS_ASSERT(strlen(pretty_name) <= PRETTY_NAME_MAX_LENGTH);
        strncpy(p->pretty_name, pretty_name, PRETTY_NAME_MAX_LENGTH+1);
        p->pretty_name[PRETTY_NAME_MAX_LENGTH] = '\0';
    }

    void set_pretty_names()
    {   // automatically set "pretty" names for items
        for (size_t i=0; i<this->max; i++)
        {
            Property* a = &this->properties[i];
            if (!a->loaded) continue;
            if (a->pretty_name[0] == '\0')
                make_pretty_name(a->name, a->pretty_name);
        }
    }

};
