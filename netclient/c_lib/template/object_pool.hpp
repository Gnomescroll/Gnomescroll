
static const bool tDEBUG = true;

template <class Object, int BUFFER_POOL_SIZE>
class Object_pool {
    private:
        int batch_num;
        int instance_used;
        //batch malloc/link method
        void batch_alloc();

    public:
    Object* first;
    Object* last;

    Object* acquire() ;

    void retire(Object* nmb);

    Object_pool();

};


//template <class Object, int BUFFER_POOL_SIZE=1024>
//class Object_pool {

template <class Object, int BUFFER_POOL_SIZE>
Object_pool<Object, BUFFER_POOL_SIZE>::Object_pool()
{
    
    batch_num = 0;
    first = NULL;
    last = NULL;
    /*
    Object* ar = (Object*) malloc(BUFFER_POOL_SIZE*sizeof(Object));
    first = &ar[0];
    int i;
    for(i=0;i<BUFFER_POOL_SIZE-1; i++){
        ar[i].next = &ar[i+1];
    }
    ar[BUFFER_POOL_SIZE-1].next = NULL;
    last = &ar[BUFFER_POOL_SIZE-1];
    */
}

//template <class Object_state, int max_n> 
//Object_list<Object_state, max_n>::Object_list(){


template <class Object, int BUFFER_POOL_SIZE>
void Object_pool<Object, BUFFER_POOL_SIZE>::batch_alloc()
{
    batch_num++;
    printf("Batch Alloc: %i \n", batch_num);
    //Object* ar = (Object*) malloc(BUFFER_POOL_SIZE*sizeof(Object));
    Object* ar = new Object[BUFFER_POOL_SIZE];
    
    first = &ar[0];

    int i;
    for(i=0;i<BUFFER_POOL_SIZE-1; i++)
    {
        ar[i].next = &ar[i+1];
    }

    ar[BUFFER_POOL_SIZE-1].next = NULL;
    last = &ar[BUFFER_POOL_SIZE-1];
}

template <class Object, int BUFFER_POOL_SIZE>
Object* Object_pool<Object, BUFFER_POOL_SIZE>::acquire()
{
    printf("alloc\n");

    //if(tDEBUG) { if(first == NULL) printf("tError1 \n");}

    //if first==last, assuming, that first.next == NULL
    if(first == NULL) 
    {
        printf("first0>next is null\n");
        //first = (struct Object*) malloc(sizeof(struct Object))
        //first.next = NULL;
        //last = first; 
        
        batch_alloc();       
    }
    //if(first == last) printf("tError2 \n");

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
