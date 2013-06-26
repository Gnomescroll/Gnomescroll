#pragma once

template <class ObjectState, IDType=int>
class SimpleGrowingObjectList
{
    private:
    void grow_to(size_t max);

    public:
        ObjectState* objects;
        size_t max;
        size_t count;
        IDType null_id;

    inline ObjectState* create();
    inline ObjectState* get(IDType id);

    SimpleGrowingObjectList(size_t start_size, IDType null_id);
    ~SimpleGrowingObjectList();
};

template <class ObjectState>
SimpleGrowingObjectList<ObjectState>::SimpleGrowingObjectList(size_t start_size, IDType null_id) :
    objects(NULL), max(0), count(0), null_id(null_id)
{
    this->resize_to(start_size);
}

template <class ObjectState>
SimpleGrowingObjectList<ObjectState>::~SimpleGrowingObjectList()
{
    if (this->objects != NULL)
        for (size_t i=0; i<this->count; i++)
            this->objects[i].~ObjectState();
    free(this->objects);
}

template <class ObjectState>
inline void SimpleGrowingObjectList::grow_to(size_t new_size)
{
    GS_ASSERT(new_size > this->max);
    ObjectState* tmp = (ObjectState*)realloc(this->objects, new_size * sizeof(*this->objects));
    IF_ASSERT(tmp == NULL) return;
    this->objects = tmp;
    this->max = new_size;
}

template <class ObjectState>
inline ObjectState* SimpleGrowingObjectList<ObjectState>::create()
{
    if (this->count >= this->max)
        this->grow_to(this->max * 2);
    IF_ASSERT(this->count >= this->max)
        return NULL;
    new (&this->objects[this->count]) ObjectState;
    this->objects[this->count].id = IDType(this->count);
    return &this->objects[this->count++];
}

template <class ObjectState>
inline ObjectState* SimpleGrowingObjectList<ObjectState>::get(IDType id)
{
    if (id < 0 || size_t(id) >= this->count)
        return NULL;
    return &this->objects[id];
}
