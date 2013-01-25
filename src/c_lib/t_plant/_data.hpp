#pragma once


namespace t_plant
{

	//typedef void (*fptr)();
	typedef void (*fptr_void)(void*);

/*
	This Stores a list of function pointers by name and object type
*/
class PlantCallbacks
{

	static const int MAX_CALLBACKS = 64;

	struct PlantCallback
	{
		int type_id;		//type of object
		fptr_void func_ptr;	//function pointer for callback
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

	fptr_void get_callback(int type_id, const char* callback_name)
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

	void set_callback(int type_id, const char* callback_name, fptr_void func_ptr)
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

	struct PlantStruct
	{
		int type;
		void* data_ptr;
	};

	static const int PLANT_ARRAY_MAX = 1024;
	struct PlantStruct* array;
	int index;

	PlantArray()
	{
		index = 0;
		array = new struct PlantStruct[PLANT_ARRAY_MAX];
		//for(int i=0; i<PLANT_ARRAY_MAX; i++)
		//	array[i] = NULL;
	}

	~PlantArray()
	{

	}


};

class PlantTypeArray
{
	public:

	struct PlantTypeStruct
	{
		//int type_id;
		int struct_size;
		char* type_name;
	};

	static const int PLANT_TYPE_MAX = 32;
	struct PlantTypeStruct* array;
	int index;

	PlantTypeArray()
	{
		//index = 0;
		array = new struct PlantTypeStruct[PLANT_TYPE_MAX];
		for(int i=0; i<PLANT_ARRAY_MAX; i++)
		{
			array[i].struct_size = 0;
			array[i].type_name = NULL;
		}
	}

	~PlantTypeArray()
	{

	}

	int get_type_id(const char* type_name)
	{
		for(int i=0; i<PLANT_ARRAY_MAX; i++)
		{
			if(strcmp(array[i].type_name, type_name) ==0)
				return i;
		}
		printf("ERROR: PlantTypeArray,  get_type_id, plant %s does not exist \n", type_name);
		GS_ABORT();
	}
};

class PlantTypeArray plant_type_array;

//save name array somewhere
void register_plant_meta(int type_id, const char* plant_type_name, int struct_size)
{
	GS_ASSERT(type_id < PLANT_TYPE_MAX);
	plant_type_array.array[type_id].struct_size = struct_size;
	plant_type_array.array[type_id].name = (char*) plant_type_name;
}

void register_plant_function(const char* plant_name, const char* function_name, fptr_void function_ptr)
{
	int type_id = plant_type_array.get_type_id(plant_name);
	plant_callbacks.set_callback(type_id, function_name, function_ptr);
}

void init_data()
{

}

void teardown_data()
{

}

}