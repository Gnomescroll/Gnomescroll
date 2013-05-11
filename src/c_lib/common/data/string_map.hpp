#pragma once


//map string to void*
//returns NULL on failure

namespace data
{


class StringMap
{	
public:

	struct Entry
	{
		char* key;
		void* value;
	};

	struct Entry* ea;
	int ean;
	int eam;

	StringMap()
	{
		ean = 0;
		eam = 1024;
		ea = (struct Entry*) malloc(sizeof(struct Entry)*eam);

		for(int i=0;i<eam;i++)
		{
			ea[i].key = NULL;
			ea[i].value = NULL;
		}
	}

	~StringMap()
	{

		for(int i=0;i<eam;i++)
		{
			if(ea[i].key != NULL)
				free(ea[i].key);
		}
		free(ea);
	}

	void insert(const char* key, void* value)
	{
		GS_ASSERT(key != NULL);
		for(int i=0; i<ean;i++)
		{
			if(strcmp(ea[i].key, key) == 0)
			{
				GS_ASSERT( false);
				return;
			}
			
			if(ean == eam)
			{
				GS_ASSERT(false);
				return;
			}

			ea[i].key = strdup(key);
			ea[i].value = value;
		}
	}

	void* get(const char* key)
	{
		for(int i=0; i<ean;i++)
		{
			if(strcmp(ea[i].key, key) == 0)
				return ea[i].value;
		}
		return NULL;
	}
};

}
