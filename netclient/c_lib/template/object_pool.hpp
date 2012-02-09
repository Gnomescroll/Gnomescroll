#pragma once

//keeps track of alloc and dealloc count for objects
#define OBJECT_POOL_DEBUG 0

//enabling this will turn off object pools and use malloc and free
#define OBJECT_POOL_DEBUG_BATCH 0

#if OBJECT_POOL_DEBUG
    #define OBJECT_POOL_OBJECT_MACRO int allocated;
#else
    #define OBJECT_POOL_OBJECT_MACRO
#endif

template <class Base, class Object, int BUFFER_POOL_SIZE>
class Object_pool {
    private:
        int batch_num;
        int instance_used;
        //batch malloc/link method
        void batch_alloc();

        //garabage collection
        Object** alloc_list;
        int alloc_list_index;
        int alloc_list_max_size;
        //virtual char* name() { static char* x = (char*) "Error: generic object pool"; return x; }

    public:

    Object* first;
    Object* last;

    inline Object* acquire() __attribute__((always_inline));

    inline void retire(Object* nmb) __attribute__((always_inline));

    Object_pool();
    ~Object_pool();
};

template <class Base, class Object, int BUFFER_POOL_SIZE>
Object_pool<Base, Object, BUFFER_POOL_SIZE>::Object_pool()
{
    //batch_num = 0;
    //first = NULL;
    //batch_alloc();
}

template <class Base, class Object, int BUFFER_POOL_SIZE>
Object_pool<Base, Object, BUFFER_POOL_SIZE>::~Object_pool()
{
    /*
        Warning: batch_alloc() must be called at least once
    */
    for(int i=0; i<alloc_list_index; i++ ) delete[] alloc_list[i];
    delete[] alloc_list;
}


template <class Base, class Object, int BUFFER_POOL_SIZE>
void Object_pool<Base, Object, BUFFER_POOL_SIZE>::batch_alloc()
{
    static int inited = 0;
    if(inited == 0 )
    {
        inited = 1;

    /*
        This is the constructor
    */
        batch_num = 0;
        first = NULL;

        const int INITIAL_SIZE = 4;
        alloc_list = new Object*[INITIAL_SIZE];
        alloc_list_max_size = INITIAL_SIZE;
        alloc_list_index = 0;
    }

    batch_num++;

    Object* ar = new Object[BUFFER_POOL_SIZE];

    #if OBJECT_POOL_DEBUG
        for(int i=0; i<BUFFER_POOL_SIZE; i++) ar[i].allocated = 0;
    #endif

    first = &ar[0];

    for(int i=0;i<BUFFER_POOL_SIZE-1; i++)
    {
        ar[i].next = &ar[i+1];
    }
    ar[BUFFER_POOL_SIZE-1].next = NULL;

    //static char* _name = name();
    printf("%s: Batch Alloc: %i n_elements: %i \n", Base::name(), batch_num, BUFFER_POOL_SIZE);

    alloc_list[alloc_list_index] = ar;
    alloc_list_index++;

    if( alloc_list_index >= alloc_list_max_size)
    {
        if(alloc_list_index > alloc_list_max_size) printf("%s: Batch Alloc: ERROR!! alloc_list error!\n", Base::name() );
        printf("%s: Batch Alloc: possible memory leak!\n", Base::name() );
        Object** tmp = new Object*[2*alloc_list_max_size];
        for(int i=0; i < alloc_list_max_size; i++) tmp[i] = alloc_list[i];
        delete[] alloc_list;
        alloc_list = tmp;
        alloc_list_max_size *= 2;
    }

}

template <class Base, class Object, int BUFFER_POOL_SIZE>
Object* Object_pool<Base, Object, BUFFER_POOL_SIZE>::acquire()
{
    #if OBJECT_POOL_DEBUG_BATCH 
        Object* tmp2 = (Object*) malloc(sizeof(Object));
        tmp2->next = NULL; //debug
        return tmp2;
    #endif

    if(first == NULL) 
    {
        batch_alloc();       
    }
    Object* tmp = first;
    first = first->next;

    #if OBJECT_POOL_DEBUG 
        if(tmp->allocated != 0)
        {
            //static const char* _name = name();
            printf("%s: Memory Pool Acquire Error, allocated= %i, object= %lx \n", Base::name(), tmp->allocated, (long) tmp );
            //int segfault = *((int*) NULL);
        }
        tmp->allocated++;
    #endif
    return tmp;
}

template <class Base, class Object, int BUFFER_POOL_SIZE>
void Object_pool<Base, Object, BUFFER_POOL_SIZE>::retire(Object* nmb)
{
    #if OBJECT_POOL_DEBUG_BATCH
        free(nmb);
        return;
    #endif

    #if OBJECT_POOL_DEBUG
        if(nmb->allocated != 1)
        {
            //static const char* _name = name();
            printf("%s: Memory Pool Retire Error, allocated= %i, object= %lx \n", Base::name(), nmb->allocated, (long) nmb );
            int segfault = *((int*) NULL);
            printf("sefault= %i", segfault);
        }
        nmb->allocated--;
    #endif
    nmb->next = first;
    first = nmb;
}
