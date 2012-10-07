#pragma once 
 
template <class ObjectState, unsigned int hard_cap>
class Simple_object_list
{
    private:
        virtual const char* name() = 0;
        
    public:
        ObjectState a[hard_cap];

        static const unsigned int n_max = hard_cap;
        unsigned int num;

        Simple_object_list(); //default constructor
        virtual ~Simple_object_list(); //default deconstructor

        inline ObjectState* create();         //object auto id
        
        inline void destroy(int _id);

        inline void print();
};

template <class ObjectState, unsigned int hard_cap> 
Simple_object_list<ObjectState, hard_cap>::Simple_object_list()
: num(0)
{
    for (unsigned int i=0; i<hard_cap; i++) this->a[i].id = i;
}

template <class ObjectState, unsigned int hard_cap> 
Simple_object_list<ObjectState, hard_cap>::~Simple_object_list()
{
}

template <class ObjectState, unsigned int hard_cap> 
inline void Simple_object_list<ObjectState, hard_cap>::print()
{
    printf("%s list instantiated at %p\n", this->name(), this);
}

template <class ObjectState, unsigned int hard_cap>
inline ObjectState* Simple_object_list<ObjectState, hard_cap>::create() 
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


template <class ObjectState, unsigned int hard_cap>
inline void Simple_object_list<ObjectState, hard_cap>::destroy(int index)
{
    GS_ASSERT(this->num > 0);
    unsigned int uindex = index;
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
