#pragma once

template <class ObjectState, size_t hard_cap>
class SimpleObjectList
{
    private:
        virtual const char* name() = 0;

    public:
        ObjectState a[hard_cap];
        static const size_t n_max = hard_cap;
        size_t num;

    inline ObjectState* create();         //object auto id
    inline void destroy(int _id);

    void print()
    {
        printf("%s list instantiated at %p\n", this->name(), this);
    }

    SimpleObjectList(); //default constructor
    virtual ~SimpleObjectList(); //default deconstructor
};

template <class ObjectState, size_t hard_cap>
SimpleObjectList<ObjectState, hard_cap>::SimpleObjectList() :
    num(0)
{
    for (size_t i=0; i<hard_cap; i++) this->a[i].id = i;
}

template <class ObjectState, size_t hard_cap>
SimpleObjectList<ObjectState, hard_cap>::~SimpleObjectList()
{
}

template <class ObjectState, size_t hard_cap>
inline ObjectState* SimpleObjectList<ObjectState, hard_cap>::create()
{
    if (this->num >= hard_cap)
    {
        #if !PRODUCTION
        printf("%s list: max reached\n", this->name());
        #endif
        return NULL;
    }
    return &this->a[this->num++];
}


template <class ObjectState, size_t hard_cap>
inline void SimpleObjectList<ObjectState, hard_cap>::destroy(int index)
{
    GS_ASSERT(this->num > 0);
    size_t uindex = index;
    GS_ASSERT(index >= 0 && uindex < this->n_max);
    GS_ASSERT(uindex < this->num);
    if (index < 0 || uindex >= this->n_max || uindex >= this->num) return;
    this->num--;
    if (uindex != this->num)
        this->a[uindex] = this->a[this->num]; // swap last good object with dead object
    this->a[this->num].reset();   // reset dead object
    this->a[this->num].id = this->num;
    this->a[uindex].id = index; // update id of swapped object
}
