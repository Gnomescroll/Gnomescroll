#pragma once

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/macros.hpp>

int GS_RENAME(const char* src, const char* dst);
int GS_MKDIR(const char* dir, int permissions);

char* read_file_to_buffer(const char* file_name, size_t* size);
char* read_binary_file_to_buffer(const char* filename, size_t* size);

off_t fsize(const char *filename);

int count_lines(char* buffer);
int count_lines(char* buffer, int* longest);

void free_read_lines(char** readlines, int lines);
char** read_lines(char* buffer);

void create_path(const char* path);
void create_path_to_file(const char* fn);

bool file_exists(const char* filename);

bool save_tmp_file(const char* fn, const char* fn_tmp, const char* fn_bak);

// reads lines of a file into fixed width per-line continuous array
// return false on error (e.g., a line is wider than width)
bool read_fixed_lines(const char* buf, const size_t width, char** lines, size_t* count);


/*
    Line Reader Class
    - reads in files one line at time
*/

#if 0
class LineReader
{
	LineReader()
	{

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

	void load_file(const char* mesh_file)
	{
	    size_t buffer_size = 0;
	    char* buffer = read_file_to_buffer(filename, &buffer_size);
	    IF_ASSERT(buffer == NULL) return;
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
	}

	char* get_line()
	{


	}
};
#endif
