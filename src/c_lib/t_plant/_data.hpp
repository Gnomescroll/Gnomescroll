#pragma once


namespace t_plant
{

	typedef void (*fptr)();
/*
	This Stores a list of function pointers by name and object type
*/
class PlantCallbacks
{

	static const int MAX_CALLBACKS = 64;

	struct PlantCallback
	{
		int type;		//type of object
		fptr func_ptr;	//function pointer for callback
		char* name;		//name of callback
	};

	struct PlantCallback ca[MAX_CALLBACKS];   	//callback array
	int cn;										//callback index

	public:

	PlantCallbacks()
	{
		cn = 0;
		for(int i=0;i<MAX_CALLBACKS;i++)
		{
			ca[i].type = -1;
			ca[i].func_ptr	= NULL;
			ca[i].name  =  NULL;
		}
	}

	~PlantCallbacks()
	{

	}

	fptr get_callback(int object_type, const char* callback_name)
	{
		for(int i=0; i<cn; i++)
		{
			if(ca[i].type == object_type 
				&& strcmp(ca[i].name, ca[i].name) == 0 )
			{
				return ca[i].func_ptr;
			}
		}
		GS_ASSERT(false);
		printf("ERROR: PlantCallbacks, cannot find callback function %s for object type %d \n", callback_name, object_type);
		return NULL;
	}

	void set_Callback(int object_type, const char* callback_name, fptr func_ptr)
	{
		ca[cn].type = object_type;
		ca[cn].func_ptr = func_ptr;
		ca[cn].name = callback_name; //this might not be safe
		cn++;
		GS_ASSERT_ABBORT(cn < MAX_CALLBACKS);
	}
};

class PlantArray
{
	public:

	struct PLANET_STRUCT
	{
		int type;
		void* data_ptr;
	};

	static const int PLANT_ARRAY_MAX = 1024;
	struct PLANET_STRUCT* array;
	int index;

	PlantArray()
	{
		index = 0;
		array = new struct PLANET_STRUCT[PLANT_ARRAY_MAX];
		//for(int i=0; i<PLANT_ARRAY_MAX; i++)
		//	array[i] = NULL;
	}

	~PlantArray()
	{

	}


};


class PlantArray plant_array;

void init_data()
{

}

void teardown_data()
{

}

}