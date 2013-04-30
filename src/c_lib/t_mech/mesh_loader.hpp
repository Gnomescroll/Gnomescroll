#pragma once



namespace t_mech
{

/*
	Generalized loader for mesh objects
	- 16x16 texture sheet
	- vertex positions
	- vertex locations
*/

class MeshInstance
{
	struct Vertex
	{
	    float x,y,z; //0
	    float tx,ty; //12
	};

	struct Vertex* va;
	int van;

	MeshInstance()
	{
		va = (struct Vertex*) malloc(2048*sizeof(struct Vertex));
		van = 0;
	}

	~MeshInstance()
	{
		if(va != NULL) free(va);
	}

	void append_vertex(float x, float y, float z, float tx, float ty)
	{
		GS_ASSERT(van < 2048);

		va[van].x = x;
		va[van].y = y;
		va[van].z = z;
		va[van].tx = tx;
		va[van].ty = ty;
		van++;
	}
};

class MeshLoader
{
	MeshLoader()
	{

	}

	~MeshLoader()
	{


	}

    void process_line(const char* input_line, bool silent)
    {
		char* var_name = new char[256];
		int ret;

		int txoff;
		int tyoff;
		float x,y,z;
		float tx, ty;

        ret = sscanf(input_line, "%f, %f, %f; %d %d %f %f", &x,&y,&z, &txoff, &tyoff, &tx, &ty);
        if (ret != 7)
        {
        	assert(false);
        }

        printf("%0.2f %0.2f %0.f, %d %d %0.2f %0.2f \n", x,y,z, txoff, tyoff, tx,ty);
    }

	bool is_whitespace(char* line)
	{
		int i=0;
		while(line[i] != 0x00)
		{
			if(isspace(line[i])  != 0) //returns non-zero for whitespace characters
				return false;
		}
		return true;
	}

	MeshInstance* load_mesh(const char* filename)
	{
		bool silent = false;

        size_t buffer_size = 0;
        char* buffer = read_file_to_buffer(filename, &buffer_size);
        IF_ASSERT(buffer == NULL) return NULL;
        size_t marker = 0;
        size_t offset = 0;
        char* tmp_str = new char[1024];
        while (marker < buffer_size)
        {
            GS_ASSERT(marker < buffer_size);
            if (buffer[marker] != '\n')
            {
                marker++;
                continue;
            }
            for (int i=0; i<1024; i++)
                tmp_str[i] = 0x00;
            memcpy(tmp_str, buffer+offset, marker - offset);
            tmp_str[marker - offset] = 0x00;
            process_line(tmp_str, silent);
            marker++;
            offset = marker;
        }
        free(buffer);
        delete[] tmp_str;

        return NULL;
	}

};

}
