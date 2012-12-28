#pragma once

/*
	This class 
*/

class ConfigFileLoader
{
	private:

	typedef enum
	{
	    CONFIG_TYPE_NONE = 0,
	    CONFIG_TYPE_FLOAT, 
	    CONFIG_TYPE_INT, 
	   	CONFIG_TYPE_COLOR,

	} CONFIG_TYPE;

	struct ConfigValue
	{
		CONFIG_TYPE type;
		char* name;
		//void* value;	//last known value
		void* ptr;		//address to writeback to
	};

	static const int cvm = 128;
	struct ConfigValue cva[cvm];
	int cvn;

	public:

	ConfigFileLoader()
	{
		cvn = 0;
		for(int i=0; i<cvm; i++)
		{
			cva[i].type = CONFIG_TYPE_NONE;
			cva[i].name = NULL;
			//cva[i].value = NULL;
			cva[i].ptr  = NULL;
		}
	}

	~ConfigFileLoader()
	{

	}


	void process_line(const char* input_line)
	{
		if(strlen(input_line) <= 3)
			return;
		//printf("scan: %s \n", input_line);

		char* var_name = new char[256];
		char* rest = new char[256];

		float value_float = 0.0f;
		int   value_int = 0;
		int   value_r,value_g,value_b,value_a;
		//int read = 0;


		int ret = sscanf(input_line, "%s = %s", var_name, rest);

		if(ret == -1)
		{
			printf("ConfigFileLoader, process_line error: scanf fail: ret= %d \n", ret);
			delete[] var_name;
			delete[] rest;
			return;
		}

		int index = -1;
		for(int i=0; i<cvn; i++)
		{
			//printf("!!! %i: %s \n", i, cva[i].name);
			if( strcmp(cva[i].name, var_name) == 0)
			{
				index = i;
				break;
			}
		}

		if(index == -1)
		{
			printf("ConfigFileLoader, process_line error: '%s' does not match any defined key \n", var_name);
			delete[] var_name;
			delete[] rest;
			return;
		}

		switch(cva[index].type)
		{
			case CONFIG_TYPE_NONE:
				printf("ConfigFileLoader ERROR: var_type= CONFIG_TYPE_NONE \n");
				GS_ASSERT_ABORT(false);
				break;
			case CONFIG_TYPE_FLOAT:
				ret = sscanf(input_line, "%s = %f", var_name, &value_float);
				if(ret != 2 )
				{
					printf("ConfigFileLoader CONFIG_TYPE_FLOAT input_line error: %s \n", input_line);
					printf("ConfigFileLoader CONFIG_TYPE_FLOAT proces_line: var_name= %s ret= %i value= %f \n", 
					var_name, ret, value_float);
					break;
				}

				if(*((float*)cva[index].ptr) != value_float)
				{
					*((float*)cva[index].ptr) = value_float;
					printf("Set float: %s to %f \n", var_name, value_float);
				}	
				break;

			case CONFIG_TYPE_INT:
				ret = sscanf(input_line, "%s = %d", var_name, &value_int);
				if(ret != 2 )
				{
					printf("ConfigFileLoader CONFIG_TYPE_INT input_line error: %s \n", input_line);
					printf("ConfigFileLoader CONFIG_TYPE_INT proces_line: var_name= %s ret= %d value= %d \n", 
					var_name, ret, value_int);
					break;
				}

				if( *((int*)cva[index].ptr) != value_int)
				{
					*((int*)cva[index].ptr) = value_int;
					printf("Set int: %s to %d \n", var_name, value_int);
				}
				break;

			case CONFIG_TYPE_COLOR:
				ret = sscanf(input_line, "%s = %d %d %d %d", var_name, &value_r, &value_g,&value_b,&value_a);
				if(ret != 5 )
				{
					printf("ConfigFileLoader CONFIG_TYPE_COLOR input_line error: %s \n", input_line);
					printf("ConfigFileLoader CONFIG_TYPE_COLOR proces_line: var_name= %s ret= %d value= %d %d %d %d \n", 
					var_name, ret, 
					value_r,value_g,value_b,value_a);
					break;
				}
				((unsigned char*)(&value_int))[0] = value_r;
				((unsigned char*)(&value_int))[1] = value_g;
				((unsigned char*)(&value_int))[2] = value_b;
				((unsigned char*)(&value_int))[3] = value_a;

				if( *((int*)cva[index].ptr) != value_int)
				{
					*((int*)cva[index].ptr) = value_int;
					printf("Set color: %s to %d %d %d %d \n", var_name, value_r,value_g,value_b,value_a);
				}
				break;

			default:
			printf("ConfigFileLoader ERROR: default wtf\n");
			GS_ASSERT_ABORT(false);

		}

		delete[] var_name;
		delete[] rest;
		//printf("input_line: %s \n", input_line);
		//printf("proces_line: var_name= %s ret= %i read= %i value= %f \n", 
		//	var_name, ret, read, value);

	}

	void load_file(const char* filename)
	{
		size_t buffer_size;
		char* buffer = read_file_to_buffer(filename, &buffer_size);	

		if(buffer_size == 0)
		{
			GS_ASSERT(false);
			return;
		}
		size_t marker = 0;
		size_t offset = 0;

		char* tmp_str = new char[1024];

		while(marker < buffer_size)
		{
			GS_ASSERT(marker < buffer_size);

			if( buffer[marker] != '\n')
			{
				marker++;
				continue;
			}

			for(int i=0; i<1024; i++) 
				tmp_str[i] = 0x00;
			memcpy(tmp_str, buffer+offset, marker - offset);
			tmp_str[marker - offset] = 0x00;
			process_line(tmp_str);

			marker++;
			offset = marker;
		}


		delete[] tmp_str;
	}

	void name_creation_check(const char* var_name)
	{
		for(int i=0; i<cvn; i++)
		{
			if( strcmp(cva[i].name, var_name) == 0)
			{
				printf("ERROR: ConfigFileLoader, set_float key= '%s' already exists \n", var_name);
				GS_ABORT();
			}
		}
	}
	void set_float(const char* var_name, float* var_loc)
	{
		name_creation_check(var_name);

		cva[cvn].name = new char[strlen(var_name)+1];
		memcpy(cva[cvn].name,var_name, strlen(var_name));
		cva[cvn].name[strlen(var_name)] = 0x00;
		cva[cvn].ptr = (void*) var_loc;
		cva[cvn].type = CONFIG_TYPE_FLOAT;
		//printf("Added %s, %d \n", var_name, cvn);
		cvn++;
	}

	void set_int(const char* var_name, int* var_loc)
	{
		name_creation_check(var_name);

		cva[cvn].name = new char[strlen(var_name)+1];
		memcpy(cva[cvn].name,var_name, strlen(var_name));
		cva[cvn].name[strlen(var_name)] = 0x00;
		cva[cvn].ptr = (void*) var_loc;
		cva[cvn].type = CONFIG_TYPE_INT;
		//printf("Added %s, %d \n", var_name, cvn);
		cvn++;
	}

	void set_color(const char* var_name, char* var_loc)
	{
		name_creation_check(var_name);

		cva[cvn].name = new char[strlen(var_name)+1];
		memcpy(cva[cvn].name,var_name, strlen(var_name));
		cva[cvn].name[strlen(var_name)] = 0x00;
		cva[cvn].ptr = (void*) var_loc;
		cva[cvn].type = CONFIG_TYPE_COLOR;
		//printf("Added %s, %d \n", var_name, cvn);
		cvn++;
	}
};
