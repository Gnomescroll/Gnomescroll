#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace Item
{

// id/name conversion pointers
typedef int (*IDFromName)(const char*); 
typedef const char* (*NameFromID)(int); 

typedef void (*addDropCallback)(int);

struct ItemDropMeta
{
    unsigned short index;   //start index
    unsigned short num_drop;    //number of possible drops
};

struct ItemDropTable
{
    int item_type;
    int drop_entries;                //number of entries
    float* drop_probabilities;      //index into probability table
    float* drop_cumulative_probabilities;
    int* item_drop_num;                 //number of items to drop
};


class ItemDropConfig
{
    public:
    
        char* name;
        int max;

        IDFromName get_id_from_name;
        NameFromID get_name_from_id;
        addDropCallback add_drop_callback;
        
        int _table_index;
        float probability_table[4096];
        float cumulative_probability_table[4096];
        int item_drop_num_table[4096];

        struct ItemDropMeta meta_drop_table[256];

        int item_drop_table_index;
        struct ItemDropTable item_drop_table[512];

        int _current_drop_id;
        struct ItemDropTable* _current_cide;
        int _current_drop_num;

    void def_drop(const char* name);
    void add_drop(const char* item_name, int drop_entries);
    void set_drop(float drop_probability, int drops);
    void drop_always(const char* item_name, int drops);
    void drop_always(const char* item_name);

    void save_to_file();
    void end();

    void init(const char* name, int max)
    {
        GS_ASSERT(name != NULL && name[0] != '\0');
        this->name = (char*)malloc((strlen(name)+1) * sizeof(char));
        strcpy(this->name, name);

        this->max = max;
    }

    ItemDropConfig() :
    name(NULL), max(0),
    get_id_from_name(NULL), get_name_from_id(NULL), add_drop_callback(NULL),
    _table_index(0), item_drop_table_index(0),
    _current_drop_id(0), _current_cide(NULL), _current_drop_num(1)
    {
        memset(this->probability_table, 0, sizeof(this->probability_table));
        memset(this->cumulative_probability_table, 0, sizeof(this->cumulative_probability_table));
        memset(this->item_drop_num_table, 0, sizeof(this->item_drop_num_table));

        memset(this->meta_drop_table, 0, sizeof(this->meta_drop_table));
        memset(this->item_drop_table, 0, sizeof(this->item_drop_table));
    }
    
    ~ItemDropConfig()
    {
        if (this->name != NULL) free(this->name);
    }
};

}   // Item
