#pragma once

template<typename Thing, typename ThingID>
class UnmanagedObjectList
{
    /* Behaves similarly to object list, except the objects are created
     * separately from the list and (un)registered with it.
     */

    private:
        size_t _index;

    public:
        ThingID null_id;
        size_t count;
        size_t max;

        Thing** objects;

    bool is_valid_id(ThingID id) const
    {
        return (id >= 0 && id < ThingID(this->max));
    }

    bool full() const
    {
        return (this->count >= this->max);
    }

    ThingID add(Thing* thing)
    {
        IF_ASSERT(thing->id != this->null_id)
            return thing->id;
        if (this->full())
            return this->null_id;

        for (size_t i=0; i<this->max; i++)
        {
            size_t index = (this->_index + i) % this->max;
            if (this->objects[index] == NULL)
            {
                this->objects[index] = thing;
                thing->id = ThingID(index);
                this->_index = (i+1) % this->max;
                this->count++;
                return thing->id;
            }
        }
        GS_ASSERT(false);
        return this->null_id;
    }

    void remove(ThingID id)
    {
        IF_ASSERT(this->count <= 0) return;
        IF_ASSERT(!this->is_valid_id(id)) return;
        GS_ASSERT(this->objects[id] != NULL);
        this->objects[id] = NULL;
        this->count--;
    }

    UnmanagedObjectList<Thing, ThingID>(size_t size, ThingID null_id) :
        _index(0), null_id(null_id), count(0), max(size)
    {
        this->objects = new Thing*[size];
        GS_ASSERT(null_id >= size);
    }

    ~UnmanagedObjectList<Thing, ThingID>()
    {
        delete[] this->objects;
    }
};
