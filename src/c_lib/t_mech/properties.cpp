#include "properties.hpp"

namespace t_mech
{
	struct* MECH_ATTRIBUTE mech_attribute;	//index from type to attributes

	const int MAX_MECHS = 256;
	const int MECH_NAME_MAX_LENGTH = 64;

	char mech_names[MAX_MECHS*MECH_NAME_MAX_LENGTH];
	int mech_name_index[MAX_MECHS] = {-1};

	void init_properties()
	{
		mech_attribute = (struct* MECH_ATTRIBUTE) malloc(MAX_MECHS* MECH_ATTRIBUTE);
		for(int i=0; i<MAX_MECHS; i++) mech_attribute[i].mech_type = -1;
	}

	void tear_down_properties();
	{
		free(mech_attribute);
	}

	void set_mech_name(int id, const char* name)
	{
	    int length = (int)strlen(name);
	    set_mech_name(id, name, length);

	    GS_ASSERT(length > 0);
	    GS_ASSERT(length < MECH_NAME_MAX_LENGTH);
	    GS_ASSERT(id >= 0 || id < MAX_MECHS);    
	    
	    if (length <= 0 || length >= MECH_NAME_MAX_LENGTH) return;
	    if (id < 0 || id >= MAX_MECHS) return;

	    static int str_index = 0;

	    for (int i=0; i<MAX_MECHS; i++)    // no duplicate names
	        if (item_name_index[i] >= 0)
	            GS_ASSERT(strcmp(item_names+item_name_index[i], name));

	    item_name_index[id] = str_index;

	    memcpy(item_names+str_index, name, length);
	    str_index += length;
	    item_names[str_index++] = '\0';

	}


	char* get_mech_name(int type)
	{
	    GS_ASSERT(type >= 0 || type < MAX_MECHS);
	    if (type < 0 || type >= MAX_MECHS) return NULL;
	    GS_ASSERT(item_name_index[type] >= 0);
	    if (item_name_index[type] < 0) return NULL;
	    GS_ASSERT(item_name_index[type] < MAX_MECHS*MECH_NAME_MAX_LENGTH);
	    if (item_name_index[type] >= MAX_MECHS*MECH_NAME_MAX_LENGTH) return NULL;
	    
	    return (item_names + item_name_index[type]);
	}
	
	int get_mech_type(const char* name)
	{
	    for (int i=0; i<MAX_MECHS; i++)
	    {
	        char* cmp_name = get_mech_name(i);
	        if (cmp_name != NULL && strcmp(name, cmp_name) == 0)
	            return i;
	    }
	    printf("In function %s:%d -- No mech for name %s\n", __FUNCTION__, __LINE__, name);
	    GS_ABORT();
	    return NULL_mech_TYPE;
	}

	
}
