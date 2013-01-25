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
		int type_id;		//type of object
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
			ca[i].type_id = -1;
			ca[i].func_ptr	= NULL;
			ca[i].name  =  NULL;
		}
	}

	~PlantCallbacks()
	{

	}

	fptr get_callback(int type_id, const char* callback_name)
	{
		for(int i=0; i<cn; i++)
		{
			if(ca[i].type_id == type_id 
				&& strcmp(ca[i].name, ca[i].name) == 0 )
			{
				return ca[i].func_ptr;
			}
		}
		GS_ASSERT(false);
		printf("ERROR: PlantCallbacks, cannot find callback function %s for object type %d \n", callback_name, type_id);
		return NULL;
	}

	void set_callback(int type_id, const char* callback_name, fptr func_ptr)
	{
		ca[cn].type_id = type_id;
		ca[cn].func_ptr = func_ptr;
		ca[cn].name = (char*) callback_name; //this might not be safe
		cn++;
		GS_ASSERT(cn < MAX_CALLBACKS);
	}
};

class PlantCallbacks plant_callbacks;

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

void register_plant(int type_id, const char* plant_type_name, fptr init, fptr teardown, fptr tick)
{
	plant_callbacks.set_callback(type_id, "init",		init);
	plant_callbacks.set_callback(type_id, "teardown",	teardown);
	plant_callbacks.set_callback(type_id, "tick",		tick);
}

void init_data()
{

}

void teardown_data()
{

}

}