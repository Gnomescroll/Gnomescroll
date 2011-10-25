#pragma once 

template <class Object_state, int max_n=1024>
class Object_list {
        private:
                int id_c;
                const char* name() { return "Object"; }
        public:
                static const int n_max = max_n;
                int num;
                Object_state* a[max_n];

                Object_list(); //default constructor
                Object_state* get(int id);
                Object_state* create();         //object auto id
                Object_state* create(int id);   //create object with id
                void destroy(int _id);

                void draw();    //overide in template specilization on client
                void draw(int exclude_id);
                void client_tick(); //override on template specilization
                void server_tick(); //override on template specilization
        };

//template <class T>
//Stack<T>::Stack(int s)

template <class Object_state, int max_n> 
Object_list<Object_state, max_n>::Object_list()
{
    num = 0;
    id_c = 0;
    int i;
    for(i=0;i<max_n;i++) a[i] = NULL;
}


template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::get(int id)
{
    if((id < 0) || (id >= n_max)) {
        printf("%s id error: id=%i\n", name() ,id);
        return NULL;
    } 
    if(a[id] == NULL) {
        printf("%s get error: object is null, id=%i\n",name(), id);
        return NULL;
    }
    return a[id];
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create() {
        int i;
        int id = id_c;
        id_c++;
        for(i=0; i<n_max;i++) {
            id = (i+id)%n_max;
            if(a[id] == NULL) break;
        }
        if(i==1024) {
            printf("%s_list Error: cannot create object, object limit exceeded\n", name() );
            return NULL;
        }
        num++;
        a[id] = new Object_state(id);
        printf("%s_list: Created object %i\n", name(), id);
        return a[id];
}

template <class Object_state, int max_n>
Object_state* Object_list<Object_state, max_n>::create(int id) {
    if(a[id] == NULL) {
        a[id] = new Object_state(id);
        printf("%s_list: Created object from id: %i\n", name(), id);
        return a[id];
    } else {
        printf("%s_list: Cannot Create object from id; id is in use: %i\n", name(), id);
        return NULL;
    }
}


template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::destroy(int id) {
    if(a[id]==NULL) {
        printf("%s_list: Cannot delete object: object is null\n", name() );
        return;
    }
    delete a[id];
    a[id] = NULL;
    num--;
    printf("%s_list: Deleted object %i\n",name(), id);
    //printf("Object_list::delete_agent not implemented\n");
}


template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::draw() {
    
    #ifdef DC_CLIENT
    int i;
    for (i=0; i<n_max;i++) {
        if (a[i]==NULL) continue;
        a[i]->draw();
    }
    #endif
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::draw(int exclude_id) {
    
    #ifdef DC_CLIENT
    int i;
    for (i=0; i<n_max;i++) {
        if (a[i]==NULL) continue;
        if (i == exclude_id) continue;
        a[i]->draw();
    }
    #endif
}


template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::client_tick() {
    int i;
    for(i=0;i<n_max;i++){
        if(a[i]==NULL) continue;
        a[i]->client_tick();
    }   
}

template <class Object_state, int max_n>
void Object_list<Object_state, max_n>::server_tick() {
    int i;
    for(i=0;i<n_max;i++){
        if(a[i]==NULL) continue;
        a[i]->server_tick();
    }   
}
