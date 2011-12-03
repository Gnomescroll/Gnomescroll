#pragma once

template <class Object, int BUFFER_POOL_SIZE>
class Object_pool {
    private:
        int batch_num;
        int instance_used;
        //batch malloc/link method
        void batch_alloc();

    public:

    virtual char* name() = 0;

    Object* first;
    Object* last;

    inline Object* acquire() __attribute__((always_inline));

    inline void retire(Object* nmb) __attribute__((always_inline));

    Object_pool();

};

template <class Object, int BUFFER_POOL_SIZE>
Object_pool<Object, BUFFER_POOL_SIZE>::Object_pool()
{
    batch_num = 0;
    first = NULL;
}

template <class Object, int BUFFER_POOL_SIZE>
void Object_pool<Object, BUFFER_POOL_SIZE>::batch_alloc()
{
    batch_num++;
    printf("%s: Batch Alloc: %i n_elements: %i \n", name(), batch_num, BUFFER_POOL_SIZE);

    Object* ar = new Object[BUFFER_POOL_SIZE];
    first = &ar[0];

    for(int i=0;i<BUFFER_POOL_SIZE-1; i++)
    {
        ar[i].next = &ar[i+1];
    }

    ar[BUFFER_POOL_SIZE-1].next = NULL;
}

template <class Object, int BUFFER_POOL_SIZE>
Object* Object_pool<Object, BUFFER_POOL_SIZE>::acquire()
{
    if(first == NULL) 
    {
        batch_alloc();       
    }
    Object* tmp = first;
    first = first->next;
    return tmp;
}

template <class Object, int BUFFER_POOL_SIZE>
void Object_pool<Object, BUFFER_POOL_SIZE>::retire(Object* nmb)
{
    nmb->next = first;
    first = nmb;
}
