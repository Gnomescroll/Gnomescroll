#pragma once

template <class ObjectState, typename IDType=int>
class SimpleGrowingObjectList
{
    private:
    void grow_to(size_t new_size)
    {
        GS_ASSERT(new_size > this->max);
        ObjectState* tmp = (ObjectState*)realloc(this->objects, new_size * sizeof(*this->objects));
        IF_ASSERT(tmp == NULL) return;
        this->objects = tmp;
        this->max = new_size;
    }

    public:
        ObjectState* objects;
        size_t max;
        size_t count;
        IDType null_id;

    ObjectState* create()
    {
        if (this->count >= this->max)
            this->grow_to(this->max * 2);
        IF_ASSERT(this->count >= this->max)
            return NULL;
        new (&this->objects[this->count]) ObjectState;
        this->objects[this->count].id = IDType(this->count);
        return &this->objects[this->count++];
    }

    ObjectState* get(IDType id)
    {
        if (id < 0 || size_t(id) >= this->count)
            return NULL;
        return &this->objects[id];
    }

    SimpleGrowingObjectList(size_t start_size, IDType null_id) :
        objects(NULL), max(0), count(0), null_id(null_id)
    {
        this->grow_to(start_size);
    }

    ~SimpleGrowingObjectList()
    {
        if (this->objects != NULL)
            for (size_t i=0; i<this->count; i++)
                this->objects[i].~ObjectState();
        free(this->objects);
    }
};
