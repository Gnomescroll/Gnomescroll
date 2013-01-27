#pragma once

#include <new>

template <class ObjectState, typename IDType=int>
class ObjectList
{
    private:
        unsigned int start;         // indexing offset for quicker lookups

    virtual const char* name() = 0;

    void init()
    {
        IF_ASSERT(this->max <= 0) return;
        GS_ASSERT(this->null_id < 0 || (unsigned int)this->null_id >= this->max);

        this->objects = (ObjectState*)calloc(this->max, sizeof(ObjectState));
        for (unsigned int i=0; i<this->max; i++) this->objects[i].id = this->null_id;
    }

    public:
        IDType null_id;
        ObjectState* objects;        // the actual object array

        unsigned int max;           // maximum list size
        unsigned int ct;            // capacity

    ObjectState* create()
    {
        if (this->ct >= this->max) return NULL;

        for (unsigned int i=0; i<this->max; i++)
        {
            unsigned int index = (i+this->start+1)%this->max;
            if (this->objects[index].id == this->null_id)
            {
                this->ct++;
                this->start = index;
                new (&this->objects[index]) ObjectState((IDType)index);
                this->objects[index].id = (IDType)index;
                return &this->objects[index];
            }
        }
        return NULL;
    }

    ObjectState* create(IDType id)
    {
        if (id < 0 || (unsigned int)id >= this->max) return NULL;
        if (this->objects[id].id != this->null_id) return NULL;
        this->ct++;
        this->start = id;
        new (&this->objects[id]) ObjectState(id);
        this->objects[id].id = id;
        return &this->objects[id];
    }

    bool destroy(IDType id)
    {
        IF_ASSERT(id < 0 || (unsigned int)id >= this->max) return false;
        if (this->objects[id].id == this->null_id) return false;
        this->objects[id].ObjectState::~ObjectState();
        this->objects[id].id = this->null_id;
        this->ct--;
        return true;
    }

    ObjectState* get(IDType id)
    {
        IF_ASSERT(id < 0 || (unsigned int)id >= this->max) return NULL;
        ObjectState* obj = &this->objects[id];
        if (obj->id == this->null_id) return NULL;
        return obj;
    }

    void print()
    {
        printf("%s list instantiated at %p\n", this->name(), this);
    }

    unsigned int space()
    {
        IF_ASSERT(this->ct > this->max) return 0;
        return this->max - this->ct;
    }

    void clear()
    {
        if (!this->ct) return;
        for (unsigned int i=0; i<this->max; i++)
            if (this->objects[i].id != this->null_id)
                this->destroy(this->objects[i].id);
    }

    virtual ~ObjectList()
    {
        if (this->objects != NULL)
        {
            for (unsigned int i=0; i<this->max; i++)
                if (this->objects[i].id != this->null_id)
                    this->objects[i].ObjectState::~ObjectState();
            free(this->objects);
        }
    }

    explicit ObjectList(unsigned int capacity) :
        start(0), null_id(-1), max(capacity), ct(0)
    {
        this->init();
    }

    explicit ObjectList(unsigned int capacity, IDType null_id) :
        start(0), null_id(null_id), max(capacity), ct(0)
    {
        this->init();
    }
};
