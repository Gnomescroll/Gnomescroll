

//reference implementation
/*

class Channel_message {
        
    public:
        int length;
        char* buffer;
        //dont need sequence number explicitly
    Channel_message() {
        buffer = NULL;
        length = 0;
        //sequence = -1;
    }
};


static const int SEQUENCE_BUFFER_SIZE = 32;

class Sequence_buffer_element {
    public:
        Sequence_buffer_element* next;
        Channel_message cm[SEQUENCE_BUFFER_SIZE];

    Sequence_buffer_element() 
    {
        next = NULL;
        for(int i=0; i<SEQUENCE_BUFFER_SIZE; i++) cm[i].buffer = NULL;
    }
};

static int _total_packets = 0;

class Python_channel_in {
    public:
    Sequence_buffer_element* read_sb;
    int read_index;
    int lowest_sequence;
    int size;

    Python_channel_in()
    {
        lowest_sequence = 0;
        read_index = 0;
        size = 0;
        read_sb = new Sequence_buffer_element;
    }

    void insert(char* buff, int length, int sequence)
    {
        //printf("py_in packet %i: seq=%i, len=%i lowest_sequence=%i read_index=%i \n", _total_packets, sequence, length, lowest_sequence, read_index);
        //if sequence is expected packet, use fast path and dont store anything
        if(lowest_sequence == sequence)
        {
            pop(buff, length);
            return;
        } 

        Sequence_buffer_element* sbe = read_sb;
        int index = lowest_sequence+1; //use modulo
        int _read_index = read_index;

        while(index != sequence) 
        {
            index++; //update index, MOD SOMETHING
            _read_index++;
            if(_read_index == SEQUENCE_BUFFER_SIZE)
            {
                _read_index = 0;
                if(sbe->next == NULL ) 
                {
                    sbe->next = new Sequence_buffer_element; //use pool
                    sbe->next->next = NULL; //new head
                }
                sbe = sbe->next;
            }
        }
        size++; 
        //printf("insert: index= %i, read_index= %i, %i elements \n", index, read_index, size);
        Channel_message* cm = &sbe->cm[_read_index];
        cm->buffer = new char[length];
        cm->length = length;
        memcpy (cm->buffer, buff, length);
    }

    void pop(char* buff, int length) 
    {
        //process cm
        {
            int _n = 0;
            int length;
            uint16_t length;
            UNPACK_uint16_t(&length, buff, &_n);
        }
        //printf("1 python packet processed:: seq= %i \n", lowest_sequence);
        lowest_sequence++; //USE MODULO

        if(size == 0)  return;  //size equals zero: fast path return
        Channel_message* cm;
        Sequence_buffer_element* sbe = read_sb;
        while(sbe->cm[read_index].buffer != NULL)
        {
            size--; 
            cm = &sbe->cm[read_index];
            //process cm
            delete cm->buffer;
            cm->buffer = NULL; //clear message
            //printf("2 python packet processed: seq= %i, %i left in buffer \n", lowest_sequence, size);
            read_index++;
            lowest_sequence++;  //USE MODULO
            if(read_index == SEQUENCE_BUFFER_SIZE)
            {
                read_index = 0;
                Sequence_buffer_element* tmp = read_sb;
                read_sb = read_sb->next;
                delete tmp; //use object pool
            }
        }
        if(size==0) read_index = 0;
    }
};
*/